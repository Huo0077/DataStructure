// ============================================================
//       多路搜索树（M-way Search Tree）—— B 树 / B+ 树的基石
// ============================================================
//
// 【什么是多路搜索树？】
//   二叉搜索树（BST）每个节点只有 1 个 key 和 2 个子节点，
//   多路搜索树将其推广：每个节点可以有 m-1 个 key 和 m 个子节点。
//
//   这个 m 称为树的"阶"（order）。阶为 m 的多路搜索树满足：
//     - 每个内部节点最多有 m 个子节点
//     - 每个内部节点（除根外）至少有 ⌈m/2⌉ 个子节点
//     - 如果根不是叶子，它至少有 2 个子节点
//
// 【为什么需要多路搜索树？】
//   对于磁盘存储（数据库索引、文件系统），I/O 非常昂贵。
//   二叉树的每个节点只存 1 个 key，树很高，每次访问一层就要一次 I/O。
//   多路树每个节点存多个 key，树矮胖，一次 I/O 读取整个节点后
//   可以在内存中做二分查找，极大减少磁盘访问次数。
//
//   典型应用：
//     - B 树：数据库索引（MySQL InnoDB）、文件系统（NTFS, HFS+, Ext4）
//     - B+ 树：几乎所有现代数据库的默认索引结构
//     - 2-3 树、2-3-4 树：B 树的特殊形式（阶 = 3 或 4）
//
// 【节点结构（以 4 阶为例，即 2-3-4 树）】
//
//   内部节点示例（3 个 key，4 个子节点）：
//       ┌─────┬───┬─────┬───┬─────┐
//       │ key0│ │ key1│ │ key2│
//       └──┬──┴───┴──┬──┴───┴──┬──┘
//          │         │         │
//      child0   child1   child2   child3
//       (<key0)  (key0~key1) (key1~key2) (>key2)
//
//   性质：
//     child[i] 中所有 key < keys[i]
//     child[i+1] 中所有 key > keys[i]（即位于 keys[i] 和 keys[i+1] 之间）
//
// 【多路搜索树 → B 树的区别】
//   单纯的多路搜索树不强制平衡，B 树在它的基础上加了强制规则：
//     - 所有叶子必须在同一层（完美平衡）
//     - 插入/删除时主动分裂/合并节点来维持平衡
//
//   本文件演示的是"2-3-4 树"（m=4 的 B 树），这是最经典的多路平衡树，
//   也是理解红黑树另一种视角的绝佳桥梁（2-3-4 树与红黑树等价！）。
//
// 【2-3-4 树与红黑树的等价关系】
//
//   2-3-4 树节点          对应的红黑树结构
//   ─────────────────────────────────────
//   2-节点（1 key）   →   一个黑色节点
//   3-节点（2 keys）  →   黑色父 + 红色子（左或右）
//   4-节点（3 keys）  →   黑色父 + 两个红色子
//
//   示例（3-节点，keys = {10, 20}）：
//     ┌─────┬─────┐
//     │ 10  │ 20  │     对应的红黑树：
//     └──┬──┴──┬──┘
//       /     \          ● 10 (黑)         或者        ● 20 (黑)
//    <10   >10,<20 >20                               / \
//                                                   ○   ○
//                                                   10   (红)
//
//   这意味着：每棵 2-3-4 树都可以转换为一棵红黑树！
//   也解释了为什么红黑树的树高 ≤ 2*log(n+1)。
//
// 【B 树的分裂操作（核心）】
//
//   当向一个已满的节点（已有 m-1 个 key）插入时，必须分裂：
//
//   以 4 阶为例，节点满时有 3 个 key：
//     [k0, k1, k2]
//
//   插入新 key 后变成 4 个 key（先塞进去再分裂）：
//     [k0, k1, k2, k_new]
//
//   分裂：把中间的 key（k1）提升到父节点，
//         左边 [k0] 和右边 [k2, k_new] 各成一个新节点。
//
//    分裂前:           分裂后（k1 提升）:
//       P                  P + k1
//       │                 /      \
//   [k0,k1,k2,k]      [k0]    [k2,k]
//
//   如果父节点也满了，则递归向上分裂——最极端情况一直裂到根。
//
// 【B 树的合并/借用操作（删除时）】
//
//   当从节点中删除一个 key 导致节点 key 数小于 ⌈m/2⌉-1（即"下溢"），需要：
//     - 如果相邻兄弟有多余的 key → 从兄弟借一个（通过父节点中转）
//     - 如果兄弟也都刚好最小值 → 与兄弟合并
//   合并可能导致父节点也下溢 → 递归向上处理。
//
//   本文件为教学目的，实现了完整的 B 树（阶可配置）：
//     - insert(key)      插入 + 自动分裂
//     - remove(key)      删除 + 借用/合并
//     - search(key)      查找（利用节点内二分查找）
//     - traverse()       中序遍历（B 树的中序 = 全局有序）
//     - BFS()            层序遍历（观察树的层级结构）
//
// ============================================================

#include<iostream>
#include<vector>
#include<queue>
#include<algorithm>
using namespace std;

// ==================== B 树节点 ====================

template<typename T>
class BTreeNode
{
public:
    vector<T> keys;                  // 节点中存储的 key 数组（有序）
    vector<BTreeNode<T>*> children;  // 子节点指针数组（比 keys 多一个）
    bool isLeaf;                     // 是否为叶节点

    BTreeNode(bool leaf) : isLeaf(leaf) {}

    // ----------------------------------------------------------
    // 在节点内二分查找 key
    // 返回 key 的索引，如果不存在返回第一个 ≥ key 的位置
    // ----------------------------------------------------------
    int findKeyIndex(const T& key) const
    {
        int idx = 0;
        while (idx < (int)keys.size() && keys[idx] < key)
        {
            idx++;
        }
        return idx;
    }

    // ----------------------------------------------------------
    // 在非满叶节点中插入 key（保证有序）
    // ----------------------------------------------------------
    void insertAtLeaf(const T& key)
    {
        int i = (int)keys.size() - 1;
        keys.push_back(T());  // 占位
        // 从右向左移动比 key 大的元素，找到插入位置
        while (i >= 0 && keys[i] > key)
        {
            keys[i + 1] = keys[i];
            i--;
        }
        keys[i + 1] = key;
    }

    // ----------------------------------------------------------
    // 删除指定位置的 key（叶节点用）
    // ----------------------------------------------------------
    void removeKey(int idx)
    {
        keys.erase(keys.begin() + idx);
    }

    // ----------------------------------------------------------
    // 在指定位置插入子节点
    // ----------------------------------------------------------
    void insertChild(int idx, BTreeNode<T>* child)
    {
        children.insert(children.begin() + idx, child);
    }

    // ----------------------------------------------------------
    // 删除指定位置的子节点
    // ----------------------------------------------------------
    void removeChild(int idx)
    {
        children.erase(children.begin() + idx);
    }
};

// ==================== B 树类 ====================

template<typename T>
class BTree
{
private:
    BTreeNode<T>* root;
    int order;     // B 树的阶（m）：每个节点最多 m-1 个 key，最多 m 个子节点
    int minKeys;   // 除根外每个节点的最少 key 数 = ⌈m/2⌉ - 1

    // ----------------------------------------------------------
    // 分裂已满的子节点（核心操作！）
    //
    // 前置条件：child 已有 m-1 个 key（满），且刚被"溢出"插入
    //          所以 child 此时有 m 个 key（多了一个）
    //
    // 操作流程：
    //   1. 找到 child 中"中间位置"的 key（midIdx = ⌈m/2⌉ - 1）
    //   2. 创建右兄弟节点 rightSibling，把 midIdx 右边的 key 都挪过去
    //   3. 如果不是叶节点，子节点也一起搬
    //   4. 把中间的 key 提升到父节点（parent）
    //   5. 更新 parent 的子节点链接
    //
    //   示例（m=4，即 2-3-4 树，满=3 keys，分裂时中间索引=1）：
    //
    //     分裂前 parent:          分裂后 parent:
    //       [...]                    [... K1 ...]
    //         |                       /        \
    //    [K0, K1, K2, K_new]     [K0]        [K2, K_new]
    //    满+溢出                     ↑左半        ↑右半
    //
    //   mid = ⌈4/2⌉-1 = 1 → K1 作为中间 key 提升
    // ----------------------------------------------------------
    void splitChild(BTreeNode<T>* parent, int childIdx)
    {
        BTreeNode<T>* leftChild = parent->children[childIdx];
        int midIdx = (order - 1) / 2;  // 中间位置的 key 索引

        // 创建右兄弟节点（和左子节点同类型：同为叶或同为内部节点）
        BTreeNode<T>* rightSibling = new BTreeNode<T>(leftChild->isLeaf);

        // 把左子节点 midIdx 右边的 key 搬到右兄弟
        for (int i = midIdx + 1; i < (int)leftChild->keys.size(); i++)
        {
            rightSibling->keys.push_back(leftChild->keys[i]);
        }

        // 把左子节点 midIdx 右边的子节点也搬过去（如果不是叶节点）
        if (!leftChild->isLeaf)
        {
            for (int i = midIdx + 1; i < (int)leftChild->children.size(); i++)
            {
                rightSibling->children.push_back(leftChild->children[i]);
            }
        }

        // 提升中间 key 到父节点
        T promotedKey = leftChild->keys[midIdx];
        parent->keys.insert(parent->keys.begin() + childIdx, promotedKey);
        parent->children.insert(parent->children.begin() + childIdx + 1, rightSibling);

        // 裁剪左子节点（只保留 midIdx 左边的 key 和子节点）
        leftChild->keys.resize(midIdx);
        if (!leftChild->isLeaf)
        {
            leftChild->children.resize(midIdx + 1);
        }
    }

    // ----------------------------------------------------------
    // 递归插入（内部辅助函数）
    //
    // 核心逻辑：
    //   1. 如果当前是叶节点 → 直接插入（插入前可能满，需先分裂）
    //   2. 如果是内部节点 → 找到该去的子树，如果该子树满了先分裂，
    //      然后再递归向下插入
    //
    // 为什么在"向下走"时就分裂，而不是"向上回溯"时？
    //   因为 B 树的分裂是从下往上的，如果到了叶节点才分裂，
    //   叶节点满了就没地方放新的 key。所以需要"预分裂"——
    //   在向下查找的过程中，看到满的节点就先分裂，保证走到叶
    //   时一定有空间。
    // ----------------------------------------------------------
    void insertHelper(BTreeNode<T>* node, const T& key)
    {
        // 情况 A：当前节点是叶节点
        if (node->isLeaf)
        {
            node->insertAtLeaf(key);
            return;
        }

        // 情况 B：内部节点 —— 找到子节点，必要时先分裂再递归
        int idx = node->findKeyIndex(key);

        // 如果子节点满了（有 order-1 个 key），先分裂
        if ((int)node->children[idx]->keys.size() == order - 1)
        {
            splitChild(node, idx);
            // 分裂后，提升上来的 key 可能改变了该走的方向
            if (key > node->keys[idx])
            {
                idx++;
            }
        }

        insertHelper(node->children[idx], key);
    }

    // ----------------------------------------------------------
    // 查找 key（递归）
    // ----------------------------------------------------------
    BTreeNode<T>* searchHelper(BTreeNode<T>* node, const T& key) const
    {
        if (node == nullptr) return nullptr;

        int idx = node->findKeyIndex(key);

        // 在当前节点的 keys 中找到 key
        if (idx < (int)node->keys.size() && node->keys[idx] == key)
        {
            return node;
        }

        // 叶节点且没找到 → 不存在
        if (node->isLeaf) return nullptr;

        // 递归到子节点
        return searchHelper(node->children[idx], key);
    }

    // ----------------------------------------------------------
    // 获取子树中最小的 key（用于删除时找前驱/后继）
    // ----------------------------------------------------------
    T getMin(BTreeNode<T>* node) const
    {
        while (!node->isLeaf)
        {
            node = node->children[0];
        }
        return node->keys[0];
    }

    T getMax(BTreeNode<T>* node) const
    {
        while (!node->isLeaf)
        {
            node = node->children.back();
        }
        return node->keys.back();
    }

    // ----------------------------------------------------------
    // 从前驱子节点借 key（用于删除时的借用操作）
    // 从左边最大的兄弟借一个 key 过来
    // ----------------------------------------------------------
    void borrowFromLeft(BTreeNode<T>* node, int idx)
    {
        BTreeNode<T>* child = node->children[idx];
        BTreeNode<T>* leftSibling = node->children[idx - 1];

        // child 的所有 key 和 children 都右移一位，腾出位置
        child->keys.insert(child->keys.begin(), node->keys[idx - 1]);
        if (!child->isLeaf)
        {
            child->children.insert(child->children.begin(), leftSibling->children.back());
            leftSibling->children.pop_back();
        }

        // 父节点的 key 替换为左兄弟的最大 key
        node->keys[idx - 1] = leftSibling->keys.back();
        leftSibling->keys.pop_back();
    }

    // ----------------------------------------------------------
    // 从后继子节点借 key（从右边最小的兄弟借）
    // ----------------------------------------------------------
    void borrowFromRight(BTreeNode<T>* node, int idx)
    {
        BTreeNode<T>* child = node->children[idx];
        BTreeNode<T>* rightSibling = node->children[idx + 1];

        // child 末尾追加父节点的 key
        child->keys.push_back(node->keys[idx]);
        if (!child->isLeaf)
        {
            child->children.push_back(rightSibling->children[0]);
            rightSibling->children.erase(rightSibling->children.begin());
        }

        // 父节点的 key 替换为右兄弟的最小 key
        node->keys[idx] = rightSibling->keys[0];
        rightSibling->keys.erase(rightSibling->keys.begin());
    }

    // ----------------------------------------------------------
    // 合并子节点（删除时的核心操作）
    //
    // 将 node->children[idx] 和 node->children[idx+1] 合并，
    // 同时把父节点的 keys[idx] 也拉下来，放到中间。
    //
    //   合并前:            合并后:
    //     [Kp]               []
    //     /  \                |
    //   [a,b] [c]    →    [a, b, Kp, c]
    //
    // 注意：合并后父节点会减少一个 key，如果父节点也因此下溢，
    // 会在外层递归调用中继续处理。
    // ----------------------------------------------------------
    void mergeChildren(BTreeNode<T>* node, int idx)
    {
        BTreeNode<T>* left = node->children[idx];
        BTreeNode<T>* right = node->children[idx + 1];

        // 父节点的 key 拉下来放到中间
        left->keys.push_back(node->keys[idx]);

        // 把右兄弟的 key 和子节点全部搬到左兄弟
        for (T& key : right->keys)
        {
            left->keys.push_back(key);
        }
        for (BTreeNode<T>* child : right->children)
        {
            left->children.push_back(child);
        }

        // 从父节点删除 key 和右兄弟的指针
        node->keys.erase(node->keys.begin() + idx);
        node->children.erase(node->children.begin() + idx + 1);

        delete right;
    }

    // ----------------------------------------------------------
    // 修复下溢（underflow）
    //
    // 节点 key 数 < minKeys 时调用。
    // 三种策略（按优先级）：
    //   1. 向左兄弟借
    //   2. 向右兄弟借
    //   3. 与兄弟合并
    //
    // 如果合并了，父节点可能产生新的下溢，需要递归向上处理。
    // ----------------------------------------------------------
    void fixUnderflow(BTreeNode<T>* node, int idx)
    {
        BTreeNode<T>* child = node->children[idx];

        // 策略 1：向左兄弟借（兄弟 key 数 > minKeys）
        if (idx > 0 && (int)node->children[idx - 1]->keys.size() > minKeys)
        {
            borrowFromLeft(node, idx);
        }
        // 策略 2：向右兄弟借
        else if (idx < (int)node->children.size() - 1
                 && (int)node->children[idx + 1]->keys.size() > minKeys)
        {
            borrowFromRight(node, idx);
        }
        // 策略 3：合并
        else
        {
            if (idx > 0)
            {
                // 与左兄弟合并（child 是合并后的节点）
                mergeChildren(node, idx - 1);
            }
            else
            {
                // 与右兄弟合并
                mergeChildren(node, idx);
            }
        }
    }

    // ----------------------------------------------------------
    // 递归删除（内部辅助函数）
    //
    // 核心逻辑：
    //   1. 在内部节点中逐层找，保证往下走时子节点 key 数 ≥ minKeys+1
    //      （即保证不会在递归底层下溢）
    //   2. 如果是叶节点 → 直接删除 key
    //   3. 如果是内部节点 → 用前驱/后继替代后再递归删除前驱/后继
    //   4. 回到当前层时检查并修复下溢
    // ----------------------------------------------------------
    void removeHelper(BTreeNode<T>* node, const T& key)
    {
        int idx = node->findKeyIndex(key);

        // ---- 情况 A：key 在当前节点中 ----
        if (idx < (int)node->keys.size() && node->keys[idx] == key)
        {
            if (node->isLeaf)
            {
                // 叶节点：直接删除
                node->removeKey(idx);
            }
            else
            {
                // 内部节点：用前驱/后继替代
                // 优先从 key 多的子树找替代
                if ((int)node->children[idx]->keys.size() > minKeys)
                {
                    // 左子树 key 多 → 取前驱
                    T predecessor = getMax(node->children[idx]);
                    node->keys[idx] = predecessor;
                    removeHelper(node->children[idx], predecessor);
                }
                else if ((int)node->children[idx + 1]->keys.size() > minKeys)
                {
                    // 右子树 key 多 → 取后继
                    T successor = getMin(node->children[idx + 1]);
                    node->keys[idx] = successor;
                    removeHelper(node->children[idx + 1], successor);
                }
                else
                {
                    // 左右都刚好 minKeys → 先合并再递归删除
                    mergeChildren(node, idx);
                    removeHelper(node->children[idx], key);
                }
            }
            return;
        }

        // ---- 情况 B：key 不在当前节点，需要去子节点中找 ----
        if (node->isLeaf)
        {
            // key 不存在
            return;
        }

        // 确保往下走时子节点有足够 key（≥ minKeys+1），以免底层下溢
        bool atLastChild = (idx == (int)node->children.size() - 1);
        BTreeNode<T>* child = node->children[idx];

        if ((int)child->keys.size() <= minKeys)
        {
            // 先填满子节点（借或合并），再递归
            fixUnderflow(node, idx);
            // fixUnderflow 可能改变了子节点结构，idx 可能需要调整
            if (atLastChild && idx > (int)node->children.size() - 1)
            {
                idx = (int)node->children.size() - 1;
            }
        }

        removeHelper(node->children[idx], key);
    }

    // ----------------------------------------------------------
    // 中序遍历（递归）
    // B 树的中序遍历 = 全局有序输出
    //
    // 对于每个内部节点，模式是：
    //   遍历 child[0] → 输出 key[0] → 遍历 child[1] → 输出 key[1] → ...
    // ----------------------------------------------------------
    void inorderHelper(BTreeNode<T>* node) const
    {
        if (node == nullptr) return;

        int i;
        for (i = 0; i < (int)node->keys.size(); i++)
        {
            // 先遍历左子树
            if (!node->isLeaf)
            {
                inorderHelper(node->children[i]);
            }
            cout << node->keys[i] << " ";
        }
        // 最后遍历最右边的子树
        if (!node->isLeaf)
        {
            inorderHelper(node->children[i]);
        }
    }

    // ----------------------------------------------------------
    // 递归销毁
    // ----------------------------------------------------------
    void destroyTree(BTreeNode<T>* node)
    {
        if (node == nullptr) return;
        if (!node->isLeaf)
        {
            for (auto child : node->children)
            {
                destroyTree(child);
            }
        }
        delete node;
    }

    // ----------------------------------------------------------
    // 打印树结构（递归缩进）
    // ----------------------------------------------------------
    void printHelper(BTreeNode<T>* node, const string& prefix, bool isLast) const
    {
        if (node == nullptr) return;

        cout << prefix;
        cout << (isLast ? "└── " : "├── ");
        cout << "[";
        for (int i = 0; i < (int)node->keys.size(); i++)
        {
            cout << node->keys[i];
            if (i < (int)node->keys.size() - 1) cout << ", ";
        }
        cout << "]" << endl;

        if (!node->isLeaf)
        {
            string newPrefix = prefix + (isLast ? "    " : "│   ");
            for (int i = 0; i < (int)node->children.size(); i++)
            {
                printHelper(node->children[i], newPrefix,
                            i == (int)node->children.size() - 1);
            }
        }
    }

public:
    // ==================== 构造与析构 ====================

    BTree(int m)
    {
        order = m;
        minKeys = (m + 1) / 2 - 1;  // ⌈m/2⌉ - 1
        // 例如 m=4: minKeys = 1（2-3-4 树中每个节点至少 1 个 key）
        //    m=5: minKeys = 2（每个节点至少 2 个 key）
        root = nullptr;
    }

    ~BTree()
    {
        destroyTree(root);
    }

    // ==================== 公有接口 ====================

    void insert(const T& key)
    {
        if (root == nullptr)
        {
            // 空树：创建根节点（叶节点）
            root = new BTreeNode<T>(true);
            root->keys.push_back(key);
            return;
        }

        // 如果根满了，需要创建新根（树长高的唯一方式！）
        if ((int)root->keys.size() == order - 1)
        {
            BTreeNode<T>* newRoot = new BTreeNode<T>(false);
            newRoot->children.push_back(root);
            splitChild(newRoot, 0);
            root = newRoot;
        }

        insertHelper(root, key);
    }

    void remove(const T& key)
    {
        if (root == nullptr) return;

        removeHelper(root, key);

        // 如果根节点变成空（key 被删光），降低树的高度
        if (root->keys.empty())
        {
            BTreeNode<T>* oldRoot = root;
            if (root->isLeaf)
            {
                root = nullptr;   // 树变成空
            }
            else
            {
                root = root->children[0];  // 唯一的子节点成为新根
            }
            delete oldRoot;
        }
    }

    bool search(const T& key) const
    {
        return searchHelper(root, key) != nullptr;
    }

    void print() const
    {
        cout << "\n========== B 树结构（阶 = " << order << "）==========" << endl;
        if (root == nullptr)
        {
            cout << "(空树)" << endl;
        }
        else
        {
            printHelper(root, "", true);
        }
        cout << "================================" << endl;
    }

    void inorder() const
    {
        cout << "中序遍历：";
        inorderHelper(root);
        cout << endl;
    }

    void BFS() const
    {
        if (root == nullptr) return;

        cout << "\n========== BFS 层序遍历 ==========" << endl;
        queue<BTreeNode<T>*> q;
        q.push(root);
        int level = 0;

        while (!q.empty())
        {
            int size = q.size();
            cout << "第 " << level << " 层：";
            for (int i = 0; i < size; i++)
            {
                BTreeNode<T>* node = q.front(); q.pop();
                cout << "[";
                for (int j = 0; j < (int)node->keys.size(); j++)
                {
                    cout << node->keys[j];
                    if (j < (int)node->keys.size() - 1) cout << ",";
                }
                cout << "]  ";

                if (!node->isLeaf)
                {
                    for (auto child : node->children)
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

    int getOrder() const { return order; }
};

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════════════╗" << endl;
    cout << "║    多路搜索树 / B 树（Multi-way Tree）测试 ║" << endl;
    cout << "╚══════════════════════════════════════════╝" << endl;

    // ---------- 测试 3 阶 B 树（2-3 树）----------
    // 3 路树 = 2-3 树：每个节点最多 2 个 key，最多 3 个子节点
    // 分裂时机：当节点有 2 个 key 时再插入第 3 个 → 中间的 key 提升到父节点
    cout << "\n>>> 测试 0：3 阶 B 树（2-3 树）—— 3路树插入演示" << endl;
    cout << "插入序列：50, 30, 70, 20, 40（连续分裂演示）" << endl;
    {
        BTree<int> t23(3);  // 3 阶 = 2-3 树（每个节点最多 2 个 key）
        for (int v : {50, 30, 70, 20, 40})
        {
            cout << "插入 " << v << " 后：" << endl;
            t23.insert(v);
            t23.print();
        }
        cout << "中序遍历（应为 20 30 40 50 70）：";
        t23.inorder();

        cout << "\n查找 30：" << (t23.search(30) ? "找到" : "未找到") << endl;
        cout << "查找 99：" << (t23.search(99) ? "找到" : "未找到") << endl;

        cout << "\n删除 30..." << endl;
        t23.remove(30);
        t23.print();
        t23.inorder();
    }

    // ---------- 测试 4 阶 B 树（2-3-4 树）----------
    cout << "\n>>> 测试 1：4 阶 B 树（2-3-4 树）插入操作" << endl;
    cout << "插入序列：10, 20, 30, 40, 50, 25, 35, 15, 5, 45, 55, 60" << endl;

    BTree<int> btree(4);  // 4 阶 = 2-3-4 树（每个节点最多 3 个 key）
    int data[] = {10, 20, 30, 40, 50, 25, 35, 15, 5, 45, 55, 60};
    for (int val : data)
    {
        btree.insert(val);
        cout << "插入 " << val << " 后的树：" << endl;
        btree.print();
    }

    btree.BFS();
    btree.inorder();

    // ---------- 测试查找 ----------
    cout << "\n>>> 测试 2：查找操作" << endl;
    cout << "查找 25：" << (btree.search(25) ? "找到" : "未找到") << endl;
    cout << "查找 100：" << (btree.search(100) ? "找到" : "未找到") << endl;

    // ---------- 测试删除 ----------
    cout << "\n>>> 测试 3：删除操作" << endl;
    cout << "删除 5..." << endl;
    btree.remove(5);
    btree.print();

    cout << "\n删除 30..." << endl;
    btree.remove(30);
    btree.print();

    cout << "\n删除 40..." << endl;
    btree.remove(40);
    btree.print();

    btree.inorder();

    // ---------- 测试不同阶的 B 树 ----------
    cout << "\n>>> 测试 4：5 阶 B 树（每个节点最多 4 个 key）" << endl;
    BTree<int> btree5(5);
    for (int i = 1; i <= 20; i++)
    {
        btree5.insert(i);
    }
    btree5.print();
    btree5.inorder();

    // ---------- 连续递增插入测试（考验分裂机制）----------
    cout << "\n>>> 测试 5：连续递增插入 1~15（4 阶 B 树）" << endl;
    BTree<int> btree3(4);
    for (int i = 1; i <= 15; i++)
    {
        btree3.insert(i);
    }
    btree3.print();
    btree3.BFS();
    cout << "中序遍历（应为 1~15）：";
    btree3.inorder();

    // ---------- 全部删除 ----------
    cout << "\n>>> 测试 6：逐个删除所有 key" << endl;
    for (int i = 15; i >= 1; i--)
    {
        cout << "删除 " << i << " ..." << endl;
        btree3.remove(i);
    }
    cout << "删除全部后：";
    btree3.print();
    btree3.inorder();

    cout << "\nB 树所有测试完成！" << endl;
    return 0;
}
