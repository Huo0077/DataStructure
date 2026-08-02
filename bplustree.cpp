// ============================================================
//       B+ 树（B+ Tree）—— 面向页式存储的有序索引
// ============================================================
//
// 【什么是 B+ 树？】
//   B+ 树是 B 树家族的重要变体：内部节点负责路由，实际记录放在叶节点，
//   且叶节点通过链表连接。它特别适合磁盘或页式存储中的范围查询。
//   不同数据库的具体索引实现各不相同；本文件聚焦可观察的 B+ 树核心不变量。
//
// 【B+ 树与 B 树的关键区别】
//
//   ┌──────────────┬──────────────────┬──────────────────────┐
//   │              │     B 树          │      B+ 树            │
//   ├──────────────┼──────────────────┼──────────────────────┤
//   │ 数据存储      │ 内部节点也存数据   │ 数据只存在叶节点       │
//   │ 内部节点 key  │ 每个 key 唯一      │ 内部节点 key 是路由用的 │
//   │ 叶节点结构    │ 独立，无链表        │ 通过链表相连，支持范围查询│
//   │ 查找效率      │ O(log n)，但路径各异│ 任何查找都要走到叶节点   │
//   │ 范围查询      │ 需要中序遍历        │ 找到起点后沿链表扫描 O(k)│
//   └──────────────┴──────────────────┴──────────────────────┘
//
// 【B+ 树的结构】
//
//   内部节点（只用于路由）：
//     ┌─────┬───┬─────┬───┬─────┐
//     │ k0  │ │ k1  │ │ k2  │         k0 < k1 < k2（路由 key）
//     └──┬──┴───┴──┬──┴───┴──┬──┘
//        │         │         │
//     子树0      子树1      子树2      子树i 中所有 key ≥ keys[i] 且 < keys[i+1]
//     <k0      ≥k0,<k1    ≥k1,<k2
//
//   叶节点（存储实际数据，用链表相连）：
//     ┌─────┬───┬─────┬───┬─────┐     ┌─────┬───┬─────┐
//     │ k0,v│ → │ k1,v│ → │ k2,v│ →→→ │ n0,v│ → │ n1,v│ →→→ NULL
//     └─────┴───┴─────┴───┴─────┘     └─────┴───┴─────┘
//
// 【为什么数据库偏爱 B+ 树？】
//
//   1. 范围查询高效：找到起始 key 后只需沿叶节点链表扫描，
//      不需要像 B 树那样做中序遍历（要反复访问内部节点）。
//      在 SQL 中，SELECT * FROM t WHERE id BETWEEN 100 AND 200
//      就是利用 B+ 树的范围扫描能力。
//
//   2. I/O 更友好：所有数据在叶节点，内部节点更"瘦"（只存 key），
//      一个磁盘页可以装更多内部节点 key，树更矮，I/O 次数更少。
//
//   3. 查询性能稳定：任何查找都必须走到叶节点，所以查找代价恒定。
//      而 B 树可能在内部节点就找到数据，虽然更快但不稳定。
//
//   4. 聚集索引（Clustered Index）：InnoDB 的 B+ 树叶节点直接
//      存储行数据（主键索引），而非聚集索引的叶节点存储主键值。
//
//   MySQL InnoDB 索引结构简化示意：
//
//     内部节点（只存 key）:               [10]        [30]
//                                       /     \      /   ╲
//     内部节点:                    [1,5]    [15,20] [35,40] [50,60]
//                                / |  \    /  |  \   ...
//     叶节点（存数据+链表）:    [1,v][5,v] [15,v][20,v] ...
//                              ↕ 互相链接（双向链表）
//
// 【B+ 树的关键设计点】
//
//   1. 内部节点的 key 是叶节点 key 的副本（路由作用）
//      约定：child[i] 的子树中最小 key ≥ keys[i-1]（如果 i>0）
//      且 child[i] 子树中所有 key < keys[i]（如果 i < key 个数）
//
//   2. 叶节点之间有双向链表（next 和 prev 指针）
//      支持顺序扫描和逆序扫描
//
//   3. 插入分裂时，内部节点的分裂和叶节点的分裂有所不同：
//      叶节点分裂：中间 key "提升"到父节点（B+ 树中提升的是右半的第一个 key）
//      内部节点分裂：和 B 树一样，中间 key 提升到父节点
//
//   4. 删除时的下溢处理与 B 树类似，但叶节点的借用/合并
//      需要通过链表指针维护相邻关系
//
// 【插入分裂示例（阶 m=4）】
//
//   叶节点满（已有 3 个 key）插入新 key → 分裂：
//
//     分裂前叶节点: [10, 20, 30, 40(新)]
//     分裂后:
//       左叶: [10, 20]          右叶: [30, 40]
//       父节点新增路由 key: 30（右叶的最小 key）
//
//   注意：B+ 树分裂叶节点时，会把 右半的最小 key 提升到父节点，
//   这个 key 仍然保留在右叶节点中（不像 B 树那样移走）。
//   这就是为什么 B+ 树的内部节点 key 是"副本"而非"原件"。
//
// 【容量约定】
//   教材对 "阶"、"度" 与最小占用率的记法并不完全统一。本实现直接使用：
//     - maxLeafKeys / maxInternalKeys：节点可容纳的 key 上限
//     - minLeafKeys / minInternalKeys：非根节点的最小 key 数
//   这样可以把代码中的分裂、借位和合并条件与容量约束一一对应。
//
// ============================================================

#include<iostream>
#include<vector>
#include<queue>
#include<algorithm>
using namespace std;

// 【图示约定与阅读地图】
//   内部节点只保存路由键，叶节点保存实际 key；父节点中的路由键对应右侧子树的下界。
//   叶节点 next / prev 链必须与树结构同步维护，否则单点查找可能正确而范围查询会出错。
//   阅读插入时先看叶分裂，再看内部节点分裂；阅读删除时按“借位 -> 合并 -> 根缩短”追踪。

// ==================== B+ 树叶节点 ====================

template<typename T>
class BPlusLeafNode;

template<typename T>
class BPlusInternalNode;

// ---- 前置声明 ----
template<typename T>
class BPlusNode
{
public:
    bool isLeaf;
    BPlusNode(bool leaf) : isLeaf(leaf) {}
    virtual ~BPlusNode() {}
};

// ==================== 叶节点 ====================

template<typename T>
class BPlusLeafNode : public BPlusNode<T>
{
public:
    vector<T> keys;                    // 存储的 key 数组（有序）
    BPlusLeafNode<T>* next;            // 指向下一个叶节点（链表！）
    BPlusLeafNode<T>* prev;            // 指向前一个叶节点（双向链表）

    BPlusLeafNode()
        : BPlusNode<T>(true), next(nullptr), prev(nullptr) {}

    // 在有序位置插入 key
    void insertKey(const T& key)
    {
        auto it = lower_bound(keys.begin(), keys.end(), key);
        keys.insert(it, key);
    }

    // 删除 key
    void removeKey(const T& key)
    {
        auto it = lower_bound(keys.begin(), keys.end(), key);
        if (it != keys.end() && *it == key)
        {
            keys.erase(it);
        }
    }

    int size() const { return (int)keys.size(); }
};

// ==================== 内部节点 ====================

template<typename T>
class BPlusInternalNode : public BPlusNode<T>
{
public:
    vector<T> keys;                               // 路由 key（有序）
    vector<BPlusNode<T>*> children;               // 子节点（children.size() = keys.size() + 1）

    BPlusInternalNode()
        : BPlusNode<T>(false) {}

    // 查找 key 应该去的子节点索引
    // 返回第一个 keys[i] > key 的 i
    int findChildIndex(const T& key) const
    {
        int idx = 0;
        while (idx < (int)keys.size() && keys[idx] <= key)
        {
            idx++;
        }
        return idx;
    }
};

// ==================== B+ 树类 ====================

template<typename T>
class BPlusTree
{
private:
    BPlusNode<T>* root;
    BPlusLeafNode<T>* leftmostLeaf;   // 最左叶节点（链表的头，用于范围扫描）
    int maxLeafKeys;                   // 叶节点最多 key 数 = 2d
    int minLeafKeys;                   // 叶节点最少 key 数 = d
    int maxInternalKeys;               // 内部节点最多 key 数 = 2d
    int minInternalKeys;               // 内部节点最少 key 数 = d

    // ----------------------------------------------------------
    // 分裂叶节点（核心操作！）
    //
    // 当叶节点 key 数超过 maxLeafKeys 时调用。
    //
    // 操作步骤：
    //   1. 创建新的右叶节点
    //   2. 把左叶的一半 key（右半部分）搬到右叶
    //   3. 维护双向链表指针
    //   4. 把右叶的最小 key 提升到父节点（作为路由 key）
    //
    //   关键细节：提升的 key 仍保留在右叶中（副本角色）。
    //
    //   示例（maxLeafKeys=3, 叶节点已满，插入新 key 后达到 4 个）：
    //
    //     分裂前: [10, 20, 30, 40]
    //     分裂后:
    //       leftLeaf: [10, 20]   rightLeaf: [30, 40]
    //       父节点插入路由 key: 30
    //
    //   命名约定（与 B 树的 splitChild 对比）：
    //     B 树：中间 key 提升到父节点，不保留在子节点中
    //     B+ 树：右半最小 key 提升到父节点，同时保留在右叶节点
    // ----------------------------------------------------------
    BPlusLeafNode<T>* splitLeaf(BPlusLeafNode<T>* leftLeaf,
                                 BPlusInternalNode<T>* parent, int childIdx)
    {
        BPlusLeafNode<T>* rightLeaf = new BPlusLeafNode<T>();
        int splitPoint = leftLeaf->size() / 2;  // 分裂点

        // 右半部分搬到右叶
        for (int i = splitPoint; i < leftLeaf->size(); i++)
        {
            rightLeaf->keys.push_back(leftLeaf->keys[i]);
        }

        // 裁剪左叶
        leftLeaf->keys.resize(splitPoint);

        // 维护双向链表
        rightLeaf->next = leftLeaf->next;
        rightLeaf->prev = leftLeaf;
        leftLeaf->next = rightLeaf;
        if (rightLeaf->next != nullptr)
        {
            rightLeaf->next->prev = rightLeaf;
        }

        // 在父节点插入路由 key（右叶的最小 key）
        T promoteKey = rightLeaf->keys[0];
        parent->keys.insert(parent->keys.begin() + childIdx, promoteKey);
        parent->children.insert(parent->children.begin() + childIdx + 1, rightLeaf);

        return rightLeaf;
    }

    // ----------------------------------------------------------
    // 分裂内部节点
    //
    // 当内部节点 key 数超过 maxInternalKeys 时调用。
    // 与 B 树的分裂逻辑完全相同。
    // ----------------------------------------------------------
    BPlusInternalNode<T>* splitInternal(BPlusInternalNode<T>* leftNode,
                                         BPlusInternalNode<T>* parent, int childIdx)
    {
        BPlusInternalNode<T>* rightNode = new BPlusInternalNode<T>();
        int splitPoint = leftNode->keys.size() / 2;  // 中间 key 的索引

        // 把 splitPoint 右边的 key 搬到右节点
        for (int i = splitPoint + 1; i < (int)leftNode->keys.size(); i++)
        {
            rightNode->keys.push_back(leftNode->keys[i]);
        }
        // 把 splitPoint 右边的子节点搬到右节点
        for (int i = splitPoint + 1; i < (int)leftNode->children.size(); i++)
        {
            rightNode->children.push_back(leftNode->children[i]);
        }

        // 中间 key 提升到父节点
        T promoteKey = leftNode->keys[splitPoint];
        parent->keys.insert(parent->keys.begin() + childIdx, promoteKey);
        parent->children.insert(parent->children.begin() + childIdx + 1, rightNode);

        // 裁剪左节点
        leftNode->keys.resize(splitPoint);
        leftNode->children.resize(splitPoint + 1);

        return rightNode;
    }

    // ----------------------------------------------------------
    // 递归插入（核心函数）
    //
    // 流程：
    //   1. 如果是叶节点 → 直接插入 key
    //   2. 如果是内部节点 → 找到子节点，如果子节点满就先分裂，
    //      然后递归
    //   3. 回溯时检查当前节点是否需要分裂
    //
    // 分裂通过 parent 和 root 的修改向上传播；调用者无需接收返回节点。
    // ----------------------------------------------------------
    void insertHelper(BPlusNode<T>* node, const T& key,
                      BPlusInternalNode<T>* parent, int childIdx)
    {
        if (node->isLeaf)
        {
            // ---- 叶节点：直接插入 ----
            BPlusLeafNode<T>* leaf = static_cast<BPlusLeafNode<T>*>(node);
            leaf->insertKey(key);

            // 检查是否需要分裂
            if (leaf->size() > maxLeafKeys)
            {
                if (parent == nullptr)
                {
                    // 叶节点也是根节点！需要创建父节点
                    parent = new BPlusInternalNode<T>();
                    parent->children.push_back(leaf);
                    root = parent;
                }
                splitLeaf(leaf, parent, childIdx);
                return;
            }
            return;
        }
        else
        {
            // ---- 内部节点：路由到子节点 ----
            BPlusInternalNode<T>* internal = static_cast<BPlusInternalNode<T>*>(node);
            int idx = internal->findChildIndex(key);
            BPlusNode<T>* child = internal->children[idx];

            // 如果子节点满了，先分裂
            bool childIsLeaf = child->isLeaf;
            bool childFull = false;
            if (childIsLeaf)
            {
                BPlusLeafNode<T>* leafChild = static_cast<BPlusLeafNode<T>*>(child);
                childFull = (leafChild->size() == maxLeafKeys);
            }
            else
            {
                BPlusInternalNode<T>* intChild = static_cast<BPlusInternalNode<T>*>(child);
                childFull = ((int)intChild->keys.size() == maxInternalKeys);
            }

            if (childFull)
            {
                if (childIsLeaf)
                {
                    splitLeaf(static_cast<BPlusLeafNode<T>*>(child), internal, idx);
                }
                else
                {
                    splitInternal(static_cast<BPlusInternalNode<T>*>(child), internal, idx);
                }
                // 分裂后可能改变了路由方向
                if (key > internal->keys[idx])
                {
                    idx++;
                }
            }

            // 递归到子节点
            insertHelper(internal->children[idx], key, internal, idx);

            // 回溯：检查当前内部节点是否需要分裂
            if ((int)internal->keys.size() > maxInternalKeys)
            {
                if (parent == nullptr)
                {
                    // 当前节点是根 → 创建新根
                    BPlusInternalNode<T>* newRoot = new BPlusInternalNode<T>();
                    newRoot->children.push_back(internal);
                    root = newRoot;
                    splitInternal(internal, newRoot, 0);
                }
                else
                {
                    splitInternal(internal, parent, childIdx);
                    return;
                }
            }
            return;
        }
    }

    // ----------------------------------------------------------
    // 查找 key（需要走到叶节点）
    // B+ 树的任何查找都必须走到叶节点！
    // ----------------------------------------------------------
    BPlusLeafNode<T>* searchLeaf(const T& key) const
    {
        BPlusNode<T>* node = root;
        if (node == nullptr) return nullptr;

        while (!node->isLeaf)
        {
            BPlusInternalNode<T>* internal = static_cast<BPlusInternalNode<T>*>(node);
            int idx = internal->findChildIndex(key);
            node = internal->children[idx];
        }
        return static_cast<BPlusLeafNode<T>*>(node);
    }

    // ----------------------------------------------------------
    // 中序遍历（利用叶节点链表，而非递归！）
    // 这是 B+ 树相比 B 树的优势——顺序扫描只需遍历链表
    // ----------------------------------------------------------
    void inorderByList() const
    {
        BPlusLeafNode<T>* leaf = leftmostLeaf;
        while (leaf != nullptr)
        {
            for (const T& key : leaf->keys)
            {
                cout << key << " ";
            }
            leaf = leaf->next;
        }
    }

    // ----------------------------------------------------------
    // 范围查询（B+ 树的核心优势！）
    // 找到 [low, high] 范围内的所有 key
    // ----------------------------------------------------------

public:
    void rangeSearch(const T& low, const T& high) const
    {
        BPlusLeafNode<T>* leaf = searchLeaf(low);
        if (leaf == nullptr) return;

        cout << "范围 [" << low << ", " << high << "] 的 key: ";
        bool found = false;
        while (leaf != nullptr)
        {
            for (const T& key : leaf->keys)
            {
                if (key >= low && key <= high)
                {
                    cout << key << " ";
                    found = true;
                }
                if (key > high) break;
            }
            if (!leaf->keys.empty() && leaf->keys.back() > high) break;
            leaf = leaf->next;
        }
        if (!found) cout << "(无)";
        cout << endl;
    }

    // ----------------------------------------------------------
    // 递归销毁
    // ----------------------------------------------------------
    void destroyTree(BPlusNode<T>* node)
    {
        if (node == nullptr) return;
        if (!node->isLeaf)
        {
            BPlusInternalNode<T>* internal = static_cast<BPlusInternalNode<T>*>(node);
            for (auto child : internal->children)
            {
                destroyTree(child);
            }
        }
        delete node;
    }

    // ----------------------------------------------------------
    // 获取子树最小 key（用于删除时找后继）
    // ----------------------------------------------------------
    T getMinKey(BPlusNode<T>* node) const
    {
        while (!node->isLeaf)
        {
            BPlusInternalNode<T>* internal = static_cast<BPlusInternalNode<T>*>(node);
            node = internal->children[0];
        }
        BPlusLeafNode<T>* leaf = static_cast<BPlusLeafNode<T>*>(node);
        return leaf->keys[0];
    }

    // ----------------------------------------------------------
    // 叶节点下溢修复：从左兄弟借 key
    // ----------------------------------------------------------
    bool borrowFromLeftLeaf(BPlusInternalNode<T>* parent, int idx,
                            BPlusLeafNode<T>* leaf, BPlusLeafNode<T>* leftSibling)
    {
        if (leftSibling->size() <= minLeafKeys) return false;

        // 左兄弟最大的 key 移到当前叶节点的最前面
        T borrowedKey = leftSibling->keys.back();
        leftSibling->keys.pop_back();
        leaf->keys.insert(leaf->keys.begin(), borrowedKey);

        // 更新父节点中的路由 key（当前叶节点的最小 key 变化了）
        parent->keys[idx - 1] = leaf->keys[0];
        return true;
    }

    // ----------------------------------------------------------
    // 叶节点下溢修复：从右兄弟借 key
    // ----------------------------------------------------------
    bool borrowFromRightLeaf(BPlusInternalNode<T>* parent, int idx,
                             BPlusLeafNode<T>* leaf, BPlusLeafNode<T>* rightSibling)
    {
        if (rightSibling->size() <= minLeafKeys) return false;

        // 右兄弟最小的 key 移到当前叶节点的最后面
        T borrowedKey = rightSibling->keys[0];
        rightSibling->keys.erase(rightSibling->keys.begin());
        leaf->keys.push_back(borrowedKey);

        // 更新父节点中右兄弟的路由 key
        parent->keys[idx] = rightSibling->keys[0];
        return true;
    }

    // ----------------------------------------------------------
    // 内部节点下溢修复：从左兄弟借
    // ----------------------------------------------------------
    bool borrowFromLeftInternal(BPlusInternalNode<T>* parent, int idx,
                                BPlusInternalNode<T>* node,
                                BPlusInternalNode<T>* leftSibling)
    {
        if ((int)leftSibling->keys.size() <= minInternalKeys) return false;

        // 通过父节点 key 中转：父 key 下移，左兄弟最大 key 上移
        node->keys.insert(node->keys.begin(), parent->keys[idx - 1]);
        node->children.insert(node->children.begin(), leftSibling->children.back());

        parent->keys[idx - 1] = leftSibling->keys.back();
        leftSibling->keys.pop_back();
        leftSibling->children.pop_back();
        return true;
    }

    // ----------------------------------------------------------
    // 内部节点下溢修复：从右兄弟借
    // ----------------------------------------------------------
    bool borrowFromRightInternal(BPlusInternalNode<T>* parent, int idx,
                                 BPlusInternalNode<T>* node,
                                 BPlusInternalNode<T>* rightSibling)
    {
        if ((int)rightSibling->keys.size() <= minInternalKeys) return false;

        node->keys.push_back(parent->keys[idx]);
        node->children.push_back(rightSibling->children[0]);

        parent->keys[idx] = rightSibling->keys[0];
        rightSibling->keys.erase(rightSibling->keys.begin());
        rightSibling->children.erase(rightSibling->children.begin());
        return true;
    }

    // ----------------------------------------------------------
    // 合并叶节点
    // 把右边叶节点合并到左边叶节点，删除右边叶节点
    // ----------------------------------------------------------
    void mergeLeaves(BPlusInternalNode<T>* parent, int leftIdx)
    {
        BPlusLeafNode<T>* left = static_cast<BPlusLeafNode<T>*>(parent->children[leftIdx]);
        BPlusLeafNode<T>* right = static_cast<BPlusLeafNode<T>*>(parent->children[leftIdx + 1]);

        // 把右叶的所有 key 搬到左叶
        for (const T& key : right->keys)
        {
            left->keys.push_back(key);
        }

        // 维护链表
        left->next = right->next;
        if (right->next != nullptr)
        {
            right->next->prev = left;
        }

        // 从父节点删除右叶的路由 key 和指针
        parent->keys.erase(parent->keys.begin() + leftIdx);
        parent->children.erase(parent->children.begin() + leftIdx + 1);

        delete right;
    }

    // ----------------------------------------------------------
    // 合并内部节点
    // 父 key 拉下来放中间，右节点合并到左节点
    // ----------------------------------------------------------
    void mergeInternalNodes(BPlusInternalNode<T>* parent, int leftIdx)
    {
        BPlusInternalNode<T>* left =
            static_cast<BPlusInternalNode<T>*>(parent->children[leftIdx]);
        BPlusInternalNode<T>* right =
            static_cast<BPlusInternalNode<T>*>(parent->children[leftIdx + 1]);

        // 父 key 拉下来
        left->keys.push_back(parent->keys[leftIdx]);

        // 右节点的 key 和子节点都搬过来
        for (const T& key : right->keys)
        {
            left->keys.push_back(key);
        }
        for (auto child : right->children)
        {
            left->children.push_back(child);
        }

        // 从父节点删除
        parent->keys.erase(parent->keys.begin() + leftIdx);
        parent->children.erase(parent->children.begin() + leftIdx + 1);

        delete right;
    }

    // ----------------------------------------------------------
    // 修复下溢的叶节点
    // ----------------------------------------------------------
    void fixLeafUnderflow(BPlusInternalNode<T>* parent, int idx,
                          BPlusLeafNode<T>* leaf)
    {
        // 尝试从左兄弟借
        if (idx > 0)
        {
            BPlusLeafNode<T>* left =
                static_cast<BPlusLeafNode<T>*>(parent->children[idx - 1]);
            if (borrowFromLeftLeaf(parent, idx, leaf, left)) return;
        }
        // 尝试从右兄弟借
        if (idx < (int)parent->children.size() - 1)
        {
            BPlusLeafNode<T>* right =
                static_cast<BPlusLeafNode<T>*>(parent->children[idx + 1]);
            if (borrowFromRightLeaf(parent, idx, leaf, right)) return;
        }
        // 都不行 → 合并
        if (idx > 0)
        {
            mergeLeaves(parent, idx - 1);
        }
        else
        {
            mergeLeaves(parent, idx);
        }
    }

    // ----------------------------------------------------------
    // 修复下溢的内部节点
    // ----------------------------------------------------------
    void fixInternalUnderflow(BPlusInternalNode<T>* parent, int idx,
                              BPlusInternalNode<T>* node)
    {
        if (idx > 0)
        {
            BPlusInternalNode<T>* left =
                static_cast<BPlusInternalNode<T>*>(parent->children[idx - 1]);
            if (borrowFromLeftInternal(parent, idx, node, left)) return;
        }
        if (idx < (int)parent->children.size() - 1)
        {
            BPlusInternalNode<T>* right =
                static_cast<BPlusInternalNode<T>*>(parent->children[idx + 1]);
            if (borrowFromRightInternal(parent, idx, node, right)) return;
        }

        if (idx > 0)
        {
            mergeInternalNodes(parent, idx - 1);
        }
        else
        {
            mergeInternalNodes(parent, idx);
        }
    }

    // ----------------------------------------------------------
    // 递归删除
    //
    // 流程：
    //   1. 在内部节点中层层往下找，如果子节点可能下溢就提前修
    //   2. 到达叶节点 → 删除 key
    //   3. 回溯检查是否需要修复下溢
    // ----------------------------------------------------------
    void removeHelper(BPlusNode<T>* node, const T& key,
                      BPlusInternalNode<T>* parent, int childIdx)
    {
        if (node == nullptr) return;

        if (node->isLeaf)
        {
            // 到达叶节点：直接删除 key
            BPlusLeafNode<T>* leaf = static_cast<BPlusLeafNode<T>*>(node);
            leaf->removeKey(key);
            return;
        }

        // 内部节点：找子节点路由
        BPlusInternalNode<T>* internal = static_cast<BPlusInternalNode<T>*>(node);
        int idx = internal->findChildIndex(key);
        BPlusNode<T>* child = internal->children[idx];

        // 如果子节点 key 数刚好在临界点，可能需要提前处理
        if (child->isLeaf)
        {
            BPlusLeafNode<T>* leafChild = static_cast<BPlusLeafNode<T>*>(child);
            if (leafChild->size() <= minLeafKeys)
            {
                fixLeafUnderflow(internal, idx, leafChild);
                // 合并可能改变子节点布局，重新定位
                if (idx >= (int)internal->children.size())
                {
                    idx = (int)internal->children.size() - 1;
                }
                if (idx > 0 && key < internal->keys[idx - 1])
                {
                    idx--;
                }
            }
        }
        else
        {
            BPlusInternalNode<T>* intChild = static_cast<BPlusInternalNode<T>*>(child);
            if ((int)intChild->keys.size() <= minInternalKeys)
            {
                fixInternalUnderflow(internal, idx, intChild);
                if (idx >= (int)internal->children.size())
                {
                    idx = (int)internal->children.size() - 1;
                }
            }
        }

        removeHelper(internal->children[idx], key, internal, idx);

        // 回溯：如果当前内部节点 key 太少（且不是根），修复
        if (parent != nullptr && (int)internal->keys.size() < minInternalKeys)
        {
            fixInternalUnderflow(parent, childIdx, internal);
        }
    }

    // ----------------------------------------------------------
    // 打印树结构
    // ----------------------------------------------------------
    void printHelper(BPlusNode<T>* node, const string& prefix, bool isLast) const
    {
        if (node == nullptr) return;

        cout << prefix;
        cout << (isLast ? "└── " : "├── ");

        if (node->isLeaf)
        {
            BPlusLeafNode<T>* leaf = static_cast<BPlusLeafNode<T>*>(node);
            cout << "叶[";
            for (int i = 0; i < leaf->size(); i++)
            {
                cout << leaf->keys[i];
                if (i < leaf->size() - 1) cout << ", ";
            }
            cout << "]";
            if (leaf->next) cout << " →";
            cout << endl;
        }
        else
        {
            BPlusInternalNode<T>* internal = static_cast<BPlusInternalNode<T>*>(node);
            cout << "内[";
            for (int i = 0; i < (int)internal->keys.size(); i++)
            {
                cout << internal->keys[i];
                if (i < (int)internal->keys.size() - 1) cout << ", ";
            }
            cout << "]" << endl;

            string newPrefix = prefix + (isLast ? "    " : "│   ");
            for (int i = 0; i < (int)internal->children.size(); i++)
            {
                printHelper(internal->children[i], newPrefix,
                            i == (int)internal->children.size() - 1);
            }
        }
    }

public:
    // ==================== 构造与析构 ====================

    BPlusTree(int maxLeafKeys_, int maxInternalKeys_)
        : root(nullptr), leftmostLeaf(nullptr)
        , maxLeafKeys(maxLeafKeys_), maxInternalKeys(maxInternalKeys_)
    {
        // 最少 key 数 = 上限/2（下取整）
        minLeafKeys = maxLeafKeys / 2;
        minInternalKeys = maxInternalKeys / 2;
    }

    BPlusTree() : BPlusTree(4, 4) {}  // 默认：叶节点最多 4 个 key，内部节点最多 4 个 key

    ~BPlusTree()
    {
        destroyTree(root);
    }

    // ==================== 公有接口 ====================

    void insert(const T& key)
    {
        // 特殊情况：树为空
        if (root == nullptr)
        {
            root = new BPlusLeafNode<T>();
            static_cast<BPlusLeafNode<T>*>(root)->insertKey(key);
            leftmostLeaf = static_cast<BPlusLeafNode<T>*>(root);
            return;
        }

        // 如果只有一个叶节点（根就是叶）
        if (root->isLeaf)
        {
            BPlusLeafNode<T>* leaf = static_cast<BPlusLeafNode<T>*>(root);
            leaf->insertKey(key);
            if (leaf->size() > maxLeafKeys)
            {
                // 分裂根叶节点
                BPlusInternalNode<T>* newRoot = new BPlusInternalNode<T>();
                newRoot->children.push_back(leaf);
                root = newRoot;
                splitLeaf(leaf, newRoot, 0);
            }
            return;
        }

        insertHelper(root, key, nullptr, 0);
    }

    void remove(const T& key)
    {
        if (root == nullptr) return;

        if (root->isLeaf)
        {
            // 根就是叶节点，直接删除
            BPlusLeafNode<T>* leaf = static_cast<BPlusLeafNode<T>*>(root);
            leaf->removeKey(key);
            if (leaf->size() == 0)
            {
                delete leaf;
                root = nullptr;
                leftmostLeaf = nullptr;
            }
            return;
        }

        removeHelper(root, key, nullptr, 0);

        // 如果根变成空壳（没有 key），降低高度
        if (!root->isLeaf)
        {
            BPlusInternalNode<T>* rootInternal = static_cast<BPlusInternalNode<T>*>(root);
            if (rootInternal->keys.empty())
            {
                BPlusNode<T>* oldRoot = root;
                root = rootInternal->children[0];
                delete oldRoot;
            }
        }
    }

    bool search(const T& key) const
    {
        BPlusLeafNode<T>* leaf = searchLeaf(key);
        if (leaf == nullptr) return false;
        for (const T& k : leaf->keys)
        {
            if (k == key) return true;
        }
        return false;
    }

    void print() const
    {
        cout << "\n========== B+ 树结构 ==========" << endl;
        if (root == nullptr)
        {
            cout << "(空树)" << endl;
        }
        else
        {
            printHelper(root, "", true);
        }
        cout << "================================" << endl;

        // 打印叶节点链表
        cout << "叶节点链表: ";
        BPlusLeafNode<T>* leaf = leftmostLeaf;
        while (leaf != nullptr)
        {
            cout << "[";
            for (int i = 0; i < leaf->size(); i++)
            {
                cout << leaf->keys[i];
                if (i < leaf->size() - 1) cout << ",";
            }
            cout << "]";
            if (leaf->next) cout << " → ";
            leaf = leaf->next;
        }
        cout << endl;
    }

    void inorder() const
    {
        cout << "中序遍历（利用叶节点链表）：";
        inorderByList();
        cout << endl;
    }

    void BFS() const
    {
        if (root == nullptr) return;

        cout << "\n========== BFS 层序遍历 ==========" << endl;
        queue<BPlusNode<T>*> q;
        q.push(root);
        int level = 0;

        while (!q.empty())
        {
            int size = q.size();
            cout << "第 " << level << " 层：";
            for (int i = 0; i < size; i++)
            {
                BPlusNode<T>* node = q.front(); q.pop();

                if (node->isLeaf)
                {
                    BPlusLeafNode<T>* leaf = static_cast<BPlusLeafNode<T>*>(node);
                    cout << "叶[";
                    for (int j = 0; j < leaf->size(); j++)
                    {
                        cout << leaf->keys[j];
                        if (j < leaf->size() - 1) cout << ",";
                    }
                    cout << "]  ";
                }
                else
                {
                    BPlusInternalNode<T>* internal = static_cast<BPlusInternalNode<T>*>(node);
                    cout << "内[";
                    for (int j = 0; j < (int)internal->keys.size(); j++)
                    {
                        cout << internal->keys[j];
                        if (j < (int)internal->keys.size() - 1) cout << ",";
                    }
                    cout << "]  ";
                    for (auto child : internal->children)
                    {
                        q.push(child);
                    }
                }
            }
            cout << endl;
            level++;
        }
        cout << "==================================" << endl;
    }
};

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║    B+ 树（B+ Tree）测试           ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    BPlusTree<int> tree(4, 4);  // 叶节点最多 4 key，内部节点最多 4 key

    // ---------- 测试插入 ----------
    cout << "\n>>> 测试 1：插入操作" << endl;
    int testData[] = {20, 10, 30, 40, 50, 25, 35, 15, 5, 45, 55, 60, 12, 18, 28};
    cout << "插入序列：";
    for (int val : testData)
    {
        cout << val << " ";
    }
    cout << endl;

    for (int val : testData)
    {
        tree.insert(val);
    }
    tree.print();
    tree.BFS();

    // ---------- 测试查找 ----------
    cout << "\n>>> 测试 2：查找操作" << endl;
    cout << "查找 25：" << (tree.search(25) ? "找到" : "未找到") << endl;
    cout << "查找 100：" << (tree.search(100) ? "找到" : "未找到") << endl;

    // ---------- 测试范围查询（B+ 树的核心优势！）----------
    cout << "\n>>> 测试 3：范围查询" << endl;
    tree.rangeSearch(20, 50);
    tree.rangeSearch(10, 30);
    tree.rangeSearch(100, 200);

    // ---------- 中序遍历 ----------
    cout << "\n>>> 测试 4：中序遍历" << endl;
    tree.inorder();

    // ---------- 测试删除 ----------
    cout << "\n>>> 测试 5：删除操作" << endl;
    cout << "删除 5..." << endl;
    tree.remove(5);
    tree.print();

    cout << "\n删除 30..." << endl;
    tree.remove(30);
    tree.print();

    cout << "\n删除 50..." << endl;
    tree.remove(50);
    tree.print();

    tree.inorder();

    // ---------- 连续递增插入 ----------
    cout << "\n>>> 测试 6：连续递增插入 1~20" << endl;
    BPlusTree<int> tree2(4, 4);
    for (int i = 1; i <= 20; i++)
    {
        tree2.insert(i);
    }
    tree2.print();
    tree2.BFS();
    tree2.inorder();
    tree2.rangeSearch(5, 15);

    // ---------- 全部删除 ----------
    cout << "\n>>> 测试 7：全部删除" << endl;
    for (int i = 20; i >= 1; i--)
    {
        tree2.remove(i);
    }
    cout << "全部删除后：";
    tree2.print();

    cout << "\nB+ 树所有测试完成！" << endl;
    return 0;
}
