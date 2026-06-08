// ============================================================
//     二叉树合集：二叉搜索树（BST）+ 增强树（查第 k 小）+ 遍历
// ============================================================
//
// 【第一部分：二叉搜索树（BST）】
//
//   BST 性质：对于任意节点，左子树所有值 < 节点值 < 右子树所有值。
//
//   支持的操作：
//     BSTInsert(val)  — 递归插入，自动找到正确位置
//     BSTDelete(val)  — 递归删除，分三种情况处理
//     isBST()         — 中序遍历验证是否为合法 BST（严格升序）
//     height()        — 递归计算树高（空树高度 = -1）
//     Preorder()      — 前序遍历：根 → 左 → 右
//     Inorder()       — 中序遍历：左 → 根 → 右（BST 中序 = 升序）
//     Postorder()     — 后序遍历：左 → 右 → 根
//     BFS()           — 广度优先/层序遍历，按层打印
//
// 【BST 删除的三种情况】
//
//   情况 1：被删节点无子节点（叶节点）
//     直接 delete，返回 nullptr
//
//   情况 2：被删节点有一个子节点
//     用子节点替代被删节点，delete 后返回子节点
//
//   情况 3：被删节点有两个子节点
//     找到右子树的最小节点（后继），用其后继的值覆盖当前节点，
//     然后递归删除后继节点。
//     为什么用后继？——后继是"比当前节点大的最小节点"，
//     用它替换可以保持 BST 性质。
//
//     示例：删除值为 10 的节点（有两个子节点）
//           10
//          /  \
//         5   15
//            /  \
//           12  20
//     后继 = FindMin(10->right) = 12
//     10->data = 12，然后删除右子树中的 12
//
// 【中序遍历验证 BST（isBST）】
//   BST 的中序遍历结果一定严格升序。
//   用 prev 指针跟踪上一个访问的节点值，如果当前值 ≤ prev，
//   说明不是严格升序 → 不是合法 BST。
//
//   int*& prev：引用传指针，保证递归调用之间共享同一个 prev。
//   初始传递 nullptr 表示还没有前驱。
//
// 【广度优先遍历 BFS（层序遍历）】
//   使用队列逐层处理：
//   1. 根入队
//   2. 循环：取出队头 → 打印 → 其子节点入队
//   3. levelSize 记录每层节点数，实现按层换行
//
//   层序遍历可以直观看到树的层级结构。
//
//
// 【第二部分：增强二叉树——查找第 k 小元素】
//
//   AugmentedNode 除了左右子节点，额外维护 leftSize：
//     leftSize = 左子树中的节点总数
//
//   currentRank = leftSize + 1 = 当前节点在子树中的排名
//
//   查找第 k 小元素的逻辑：
//     k == currentRank → 当前节点就是第 k 小（命中！）
//     k <  currentRank → 目标在左子树中（排名不变）
//     k >  currentRank → 目标在右子树中（排名 = k - currentRank）
//
//   示例（根节点值为 8，leftSize = 3）：
//         8(rank=4)
//        /        \
//       3         12
//      / \       /  \
//     1   6    10   14
//
//   找 k=2（第 2 小）：
//     根 8 的 rank=4, k=2<4 → 去左子树，k 不变
//     左子 3 的 rank=2, k=2=2 → 命中！返回节点 3
//
//   找 k=5（第 5 小）：
//     根 8 的 rank=4, k=5>4 → 去右子树，k=5-4=1
//     右子 12 的 rank=2, k=1<2 → 去左子树
//     10 的 rank=1, k=1=1 → 命中！返回节点 10
//
//   时间复杂度：O(log n)——每次根据排名判断走左还是右，树高决定步数。
//
//   这个技巧常用于实现"有序集合"数据结构，如 C++ 的
//   __gnu_pbds::tree 底层就用类似机制。
//
// ============================================================

#include<iostream>
#include<algorithm>
#include<queue>
using namespace std;

// ==================== 第一部分：二叉搜索树 ====================

class TreeNode
{
public:
    int data;
    TreeNode* Left;
    TreeNode* Right;
    TreeNode(int val) : data(val), Left(nullptr), Right(nullptr) {}
};

class Tree
{
private:
    TreeNode* root;

    // ----------------------------------------------------------
    // 计算树高：空树返回 -1，叶节点返回 0
    // 树高 = max(左子树高, 右子树高) + 1
    // ----------------------------------------------------------
    int height(TreeNode* node)
    {
        if (node == nullptr)
        {
            return -1;   // 空树高度为 -1（与教材一致）
        }
        int left_height = height(node->Left);
        int right_height = height(node->Right);
        return max(left_height, right_height) + 1;
    }

    // ----------------------------------------------------------
    // 前序遍历（NLR）：Node → Left → Right
    // 适合复制一棵树（先创建根，再复制左右子树）
    // ----------------------------------------------------------
    void PreorderHelper(TreeNode* node)
    {
        if (node == nullptr)
        {
            return;
        }
        cout << node->data << " ";
        PreorderHelper(node->Left);
        PreorderHelper(node->Right);
    }

    // ----------------------------------------------------------
    // 中序遍历（LNR）：Left → Node → Right
    // BST 的中序遍历 = 升序输出。用于验证 BST 性质。
    // ----------------------------------------------------------
    void InorderHelper(TreeNode* node)
    {
        if (node == nullptr)
        {
            return;
        }
        InorderHelper(node->Left);
        cout << node->data << " ";
        InorderHelper(node->Right);
    }

    // ----------------------------------------------------------
    // 后序遍历（LRN）：Left → Right → Node
    // 适合释放内存（先删子节点，再删父节点）
    // ----------------------------------------------------------
    void PostorderHelper(TreeNode* node)
    {
        if (node == nullptr)
        {
            return;
        }
        PostorderHelper(node->Left);
        PostorderHelper(node->Right);
        cout << node->data << " ";
    }

    // ----------------------------------------------------------
    // 递归插入：沿着树向下找位置，到 null 时创建新节点
    // 重复值不插入（BST 默认不含重复键）
    // ----------------------------------------------------------
    TreeNode* BSTinsert(TreeNode* node, int val)
    {
        if (node == nullptr)
        {
            return new TreeNode(val);  // 找到插入位置
        }
        if (val < node->data)
        {
            node->Left = BSTinsert(node->Left, val);   // 插到左边
        }
        else if (val > node->data)
        {
            node->Right = BSTinsert(node->Right, val); // 插到右边
        }
        // val == node->data 时什么也不做（跳过重复）
        return node;
    }

    // ----------------------------------------------------------
    // 找子树的最小节点：一直向左走到底
    // 用于 BST 删除的"后继"替换策略
    // ----------------------------------------------------------
    TreeNode* FindMin(TreeNode* node)
    {
        if (node->Left == nullptr)
        {
            return node;  // 不能再往左走了，当前即为最小
        }
        return FindMin(node->Left);
    }

    // ----------------------------------------------------------
    // 递归删除三种情况：
    //   1. 叶节点     → 直接 delete，返回 nullptr
    //   2. 有一个子节点 → 用子节点替代，delete 后返回子节点
    //   3. 有两个子节点 → 用后继节点的值覆盖，然后递归删除后继
    //
    // 注意：递归调用必须把返回值赋回给对应的子指针。
    //
    // 示例：node->Left = BSTdelete(node->Left, val)
    //   如果不赋值，删除的是 node->Left 的子节点时，
    //   node->Left 还指向已释放的内存（野指针/悬垂指针）。
    // ----------------------------------------------------------
    TreeNode* BSTdelete(TreeNode* node, int val)
    {
        if (node == nullptr)
        {
            cout << val << " is not found" << endl;
            return nullptr;
        }

        // 在左子树或右子树中查找
        if (node->data > val)
        {
            node->Left = BSTdelete(node->Left, val);    // 左边去找（并更新左子指针）
        }
        else if (node->data < val)
        {
            node->Right = BSTdelete(node->Right, val);  // 右边去找（并更新右子指针）
        }
        else
        {
            // 找到了！当前 node 就是要删除的节点

            // 情况 1 & 2：0 或 1 个子节点
            if (node->Left == nullptr)
            {
                TreeNode* temp = node->Right;
                delete node;
                return temp;
            }
            else if (node->Right == nullptr)
            {
                TreeNode* temp = node->Left;
                delete node;
                return temp;
            }
            // 情况 3：有两个子节点
            else
            {
                TreeNode* Min = FindMin(node->Right);        // 找后继
                node->data = Min->data;                      // 用后继的值覆盖
                node->Right = BSTdelete(node->Right, Min->data);  // 删除后继节点
            }
        }
        return node;
    }

    // ----------------------------------------------------------
    // 验证 BST：利用中序遍历严格升序的性质
    // prev 是 int* 引用，确保递归层级间共享同一个指针
    // ----------------------------------------------------------
    bool isBSTHelper(TreeNode* node, int*& prev)
    {
        if (node == nullptr)
        {
            return true;
        }
        // 左子树
        if (!isBSTHelper(node->Left, prev))
        {
            return false;
        }
        // 当前节点：必须严格大于前驱
        if (prev != nullptr && node->data <= *prev)
        {
            return false;  // 不满足严格升序
        }
        prev = &node->data;  // 更新前驱为当前节点
        // 右子树
        return isBSTHelper(node->Right, prev);
    }

    // ----------------------------------------------------------
    // 广度优先遍历（层序遍历，BFS）
    // 使用队列逐层处理，levelSize 记录每层节点数实现分层打印
    //
    // 图解过程：
    //         4
    //        / \
    //       2   6
    //      / \ / \
    //     1  3 5 7
    //
    //   队列: [4]      → levelSize=1: 打印 4, 子节点 2,6 入队
    //   队列: [2,6]    → levelSize=2: 打印 2,6, 子节点 1,3,5,7 入队
    //   队列: [1,3,5,7]→ levelSize=4: 打印 1,3,5,7, 无子节点
    //   队列空 → 结束
    // ----------------------------------------------------------
    void BFSHelper(TreeNode* node)
    {
        if (node == nullptr)
        {
            return;
        }
        queue<TreeNode*> q;
        q.push(node);
        int level = 0;
        while (!q.empty())
        {
            int levelSize = q.size();  // 当前层的节点数
            cout << "Level " << level << ": ";
            for (int i = 0; i < levelSize; i++)
            {
                TreeNode* temp = q.front();
                q.pop();
                cout << temp->data << " ";
                if (temp->Left != nullptr)
                {
                    q.push(temp->Left);
                }
                if (temp->Right != nullptr)
                {
                    q.push(temp->Right);
                }
            }
            cout << endl;
            level++;
        }
    }

public:
    Tree() : root(nullptr) {}

    // ---------- BST 公有接口 ----------

    void BSTInsert(int val)
    {
        root = BSTinsert(root, val);
    }

    void BSTDelete(int val)
    {
        root = BSTdelete(root, val);
    }

    bool isBST()
    {
        int* prev = nullptr;
        return isBSTHelper(root, prev);
    }

    void BFS()
    {
        BFSHelper(root);
    }

    void Preorder()
    {
        PreorderHelper(root);
        cout << endl;
    }

    void Inorder()
    {
        InorderHelper(root);
        cout << endl;
    }

    void Postorder()
    {
        PostorderHelper(root);
        cout << endl;
    }

    int height()
    {
        return height(root);
    }
};

// ==================== 第二部分：增强二叉树（查第 k 小） ====================

// 增强节点：额外存储左子树的节点数 leftSize
class AugmentedNode
{
public:
    int info;
    int leftSize;          // 左子树中的节点总数
    AugmentedNode* left;
    AugmentedNode* right;
    AugmentedNode(int val, int lSize = 0)
        : info(val), leftSize(lSize), left(nullptr), right(nullptr) {}
};

// 在增强 BST 中查找第 k 小的元素
//
// 算法：
//   currentRank = leftSize + 1  （当前节点在子树中排第几）
//   比较 k 和 currentRank：
//     k == currentRank → 命中，返回当前节点
//     k <  currentRank → 第 k 小在左子树中，递归左子树
//     k >  currentRank → 第 k 小在右子树中，k 减去 currentRank 后递归右子树
//
//   减去 currentRank 的原因：
//     当前节点及整个左子树都比右子树的所有节点小，
//     所以右子树的"第 (k - currentRank) 小"就是整棵树的"第 k 小"。
AugmentedNode* findKth(AugmentedNode* root, int k)
{
    if (root == nullptr)
    {
        return nullptr;
    }
    int currentRank = root->leftSize + 1;
    if (k == currentRank)
    {
        return root;                           // 命中！
    }
    else if (k < currentRank)
    {
        return findKth(root->left, k);         // 在左子树中，排名不变
    }
    else
    {
        return findKth(root->right, k - currentRank);  // 在右子树中，更新排名
    }
}
