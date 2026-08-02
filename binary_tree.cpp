// ============================================================
//     二叉搜索树（BST）+ 二叉树遍历 + 增强树（查第 k 小）
// ============================================================
//
// 本文件涵盖：
//   Part 1: 二叉搜索树（BST）—— 插入/删除/验证
//   Part 2: 二叉树四种遍历 —— 前序/中序/后序/层序
//   Part 3: 增强二叉树 —— O(log n) 查找第 k 小元素
//
// ============================================================

#include<iostream>
#include<algorithm>
#include<queue>
using namespace std;

// 【阅读地图】
//   阅读顺序：BST 基本操作 -> 四种遍历 -> 增强树的第 k 小查询。
//   BST 不变量不是“左孩子小、右孩子大”，而是任意节点的整棵左子树都小、整棵右子树都大。
//   删除和递归插入后都要接住辅助函数的返回值，否则父子链接可能丢失。

// ============================================================
//   Part 1: 二叉搜索树（BST）
// ============================================================
//
// 【BST 性质】
//   对于任意节点，左子树所有值 < 节点值 < 右子树所有值。
//   中序遍历 BST = 严格升序输出。
//
// 【BST 删除的三种情况】
//   情况 1：无子节点（叶）→ 直接 delete，返回 nullptr
//   情况 2：有一个子节点  → 用子节点替代，delete 后返回子节点
//   情况 3：有两个子节点  → 找右子树最小节点（后继），用其值覆盖当前节点，
//                          然后递归删除后继节点
//
//   为什么用后继？后继 = "比当前节点大的最小节点"，用它替换可保持 BST 性质。
//
//   示例：删除 10（两个子节点）
//           10
//          /  ╲
//         5   15
//            /  ╲
//           12  20
//   后继 = FindMin(10->right) = 12
//   10->data = 12，然后删除右子树中的 12
//
// 【中序遍历验证 BST（isBST）】
//   prev 指针跟踪上一个访问的节点值，利用引用传指针保证递归层级间共享。
//   如果当前值 ≤ prev → 不是严格升序 → 不是合法 BST。
//
// 【时间复杂度】
//   insert/delete/search: O(h), h = 树高
//   平衡时 h = O(log n)，退化成链表时 h = O(n)
// ============================================================

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
    // height(node)：计算以 node 为根的子树高度
    //
    // 目的：提供树的高度的查询，用于分析树是否平衡。
    // 空树 = -1（没有节点），叶节点 = 0（只有一个节点，无左右子）。
    // 递归取左右子树高度的较大值 + 1。
    //
    // 为什么空树是 -1 而不是 0？
    //   -1 使叶节点高度 = max(-1, -1) + 1 = 0，公式统一，
    //   不需要特殊处理叶节点的情况。
    // ----------------------------------------------------------
    int height(TreeNode* node)
    {
        if (!node) return -1;
        return max(height(node->Left), height(node->Right)) + 1;
    }

    // ----------------------------------------------------------
    // BSTinsert(node, val)：递归插入一个值到 BST 中
    //
    // 目的：将 val 插入到 BST 的正确位置，保持 BST 性质不变。
    //
    // 操作思路：
    //   从根开始，如果 val < 当前节点值 → 去左子树插入
    //   如果 val > 当前节点值 → 去右子树插入
    //   如果 val == 当前节点值 → 已存在，跳过（不允许重复值）
    //   到达 nullptr → 在此创建新节点
    //
    // 必须接收递归返回值并赋给子指针：
    //   node->Left = BSTinsert(node->Left, val);
    // 这样才能在创建新节点时正确地建立父子链接。
    // ----------------------------------------------------------
    TreeNode* BSTinsert(TreeNode* node, int val)
    {
        if (!node) return new TreeNode(val);
        if (val < node->data)
            node->Left = BSTinsert(node->Left, val);
        else if (val > node->data)
            node->Right = BSTinsert(node->Right, val);
        return node;
    }

    // ----------------------------------------------------------
    // FindMin(node)：找子树中最小的节点
    //
    // 目的：BST 的核心辅助函数，用于删除时的"后继查找"。
    // BST 性质保证：一直向左走，最左侧的节点就是最小值。
    // O(h)，h = 树高。
    // ----------------------------------------------------------
    TreeNode* FindMin(TreeNode* node)
    {
        if (!node->Left) return node;
        return FindMin(node->Left);
    }

    // ----------------------------------------------------------
    // BSTdelete(node, val)：递归从 BST 中删除指定值
    //
    // 目的：删除值为 val 的节点，同时保持 BST 性质和树结构完整。
    //
    // 三种情况处理：
    //   情况 1（叶节点）：直接 delete，返回 nullptr 让父节点切断链接
    //   情况 2（一个子节点）：用唯一的子节点替代自己，返回子节点
    //   情况 3（两个子节点）：找右子树的最小节点（后继），用它覆盖当前值，
    //                         然后递归删除后继节点（后继最多一个右子，回退到情况1/2）
    //
    // 为什么用后继（右子树最小节点）替换？
    //   后继 = "比当前节点大的最小节点"
    //   - 它大于所有左子树节点 ✓
    //   - 它小于所有右子树其他节点（因为它是最小的那个） ✓
    //   - 替换后 BST 性质不变
    //
    // 【关键细节】递归调用的返回值必须赋给子指针！
    //   node->Left = BSTdelete(node->Left, val);
    // 如果写成 BSTdelete(node->Left, val) 不接收返回值，
    // 删除 node->Left 本身时，node->Left 仍指向已释放内存 → 野指针崩掉。
    //
    // 完整示例（删除根节点 10）：
    //       10              12
    //      /  \            /  ╲
    //     5   15    →      5   15
    //        /  \              /
    //       12  20            20
    //  FindMin(10->Right) = 12, 用 12 覆盖 10, 递归删除 12
    //  12 是叶节点 → delete 后 15->Left = nullptr ✓
    // ----------------------------------------------------------
    TreeNode* BSTdelete(TreeNode* node, int val)
    {
        if (!node) { cout << val << " is not found" << endl; return nullptr; }

        if (val < node->data)
            node->Left = BSTdelete(node->Left, val);
        else if (val > node->data)
            node->Right = BSTdelete(node->Right, val);
        else
        {
            // 找到了要删除的节点

            // 情况 1 & 2：0 或 1 个子节点
            if (!node->Left)
            {
                TreeNode* temp = node->Right;  // 可能是子节点或 nullptr
                delete node;
                return temp;
            }
            else if (!node->Right)
            {
                TreeNode* temp = node->Left;
                delete node;
                return temp;
            }
            // 情况 3：有两个子节点 → 后继替换
            TreeNode* Min = FindMin(node->Right);
            node->data = Min->data;                            // 覆盖值
            node->Right = BSTdelete(node->Right, Min->data);   // 删除原后继
        }
        return node;
    }

    // ----------------------------------------------------------
    // isBSTHelper(node, prev)：中序遍历验证 BST 合法性
    //
    // 目的：判断这棵树是否是合法的 BST。
    // 原理：BST 的中序遍历必须是严格升序。
    // prev 是 int*&（引用传指针），所有递归层级共享同一个变量，
    // 始终指向中序遍历中上一个访问的节点值。
    //
    // 检查：如果当前节点值 ≤ prev → 不是严格升序 → 不是合法 BST。
    //
    // 为什么用 int*& 而不是 int&？
    //   因为 nullptr 携带"尚无前驱"的信息——遍历第一个节点时
    //   prev 还是 nullptr，需要特殊处理（跳过比较）。
    // ----------------------------------------------------------
    bool isBSTHelper(TreeNode* node, int*& prev)
    {
        if (!node) return true;
        if (!isBSTHelper(node->Left, prev)) return false;  // 左子树不合法
        if (prev && node->data <= *prev) return false;     // 违反升序
        prev = &node->data;                                 // 更新前驱
        return isBSTHelper(node->Right, prev);
    }

public:
    Tree() : root(nullptr) {}

    void BSTInsert(int val) { root = BSTinsert(root, val); }
    void BSTDelete(int val) { root = BSTdelete(root, val); }

    bool isBST()
    {
        int* prev = nullptr;
        return isBSTHelper(root, prev);
    }

    int height() { return height(root); }

    // ============================================================
    //   Part 2: 二叉树四种遍历
    // ============================================================
    //
    // 三种递归遍历的区别只在"处理当前节点"的时机：
    //
    // 【前序遍历 NLR】根→左→右
    //   访问的顺序就是"先遇到谁就访问谁"。
    //   典型应用：复制（序列化）树——先创建根节点，再递归创建左右子树。
    //
    // 【中序遍历 LNR】左→根→右
    //   对 BST 而言输出严格升序。
    //   典型应用：BST 验证、有序输出。
    //
    // 【后序遍历 LRN】左→右→根
    //   先处理完子节点再处理父节点。
    //   典型应用：释放内存（必须先释放子节点再释放父节点，否则子节点丢失）。
    //
    // 【层序遍历 BFS】逐层从左到右
    //   用队列实现：每处理完一层，子节点全部入队（成为下一层）。
    //   典型应用：按层打印、最短路径（无权图）。
    //
    //   示例：
    //         4
    //        / ╲
    //       2   6
    //      / \ / ╲
    //     1  3 5 7
    //   前序: 4 2 1 3 6 5 7
    //   中序: 1 2 3 4 5 6 7  (BST:升序)
    //   后序: 1 3 2 5 7 6 4
    //   层序: L0:4 / L1:2 6 / L2:1 3 5 7
    // ============================================================

private:
    // 前序遍历：根→左→右。适合复制树，先创建根再递归子节点。
    void PreorderHelper(TreeNode* node)
    {
        if (!node) return;
        cout << node->data << " ";
        PreorderHelper(node->Left);
        PreorderHelper(node->Right);
    }

    // 中序遍历：左→根→右。BST 中序 = 严格升序输出。
    void InorderHelper(TreeNode* node)
    {
        if (!node) return;
        InorderHelper(node->Left);
        cout << node->data << " ";
        InorderHelper(node->Right);
    }

    // 后序遍历：左→右→根。适合释放内存——必须先删除子节点再删父节点。
    void PostorderHelper(TreeNode* node)
    {
        if (!node) return;
        PostorderHelper(node->Left);
        PostorderHelper(node->Right);
        cout << node->data << " ";
    }

    // ----------------------------------------------------------
    // BFSHelper(node)：按层输出整棵树
    //
    // 目的：逐层打印树的节点，清晰展示树的结构。
    //
    // 操作思路：
    //   用队列存储待访问的节点。每一层开始前，先记录当前队列大小
    //   （= 该层的节点数），然后 for 循环恰好处理这一层的所有节点，
    //   处理过程中将下一层的节点入队（但不会影响当前层的计数）。
    //
    // 关键技巧：levelSize = q.size() 在每层开始时拍照
    //   如果不拍照，处理过程中新节点不断入队，无法确定层边界，
    //   最终所有节点挤在一行。
    // ----------------------------------------------------------
    void BFSHelper(TreeNode* node)
    {
        if (!node) return;
        queue<TreeNode*> q;
        q.push(node);
        int level = 0;
        while (!q.empty())
        {
            int levelSize = q.size();  // 当前层节点数（拍快照）
            cout << "Level " << level << ": ";
            for (int i = 0; i < levelSize; i++)
            {
                TreeNode* t = q.front(); q.pop();
                cout << t->data << " ";
                if (t->Left)  q.push(t->Left);
                if (t->Right) q.push(t->Right);
            }
            cout << endl;
            level++;
        }
    }

public:
    void Preorder()  { PreorderHelper(root);  cout << endl; }
    void Inorder()   { InorderHelper(root);   cout << endl; }
    void Postorder() { PostorderHelper(root); cout << endl; }
    void BFS()       { BFSHelper(root); }
};

// ============================================================
//   Part 3: 增强二叉树 —— O(log n) 查找第 k 小元素
// ============================================================
//
// 【核心思想】
//   节点额外存储 leftSize = 左子树节点总数。
//   currentRank = leftSize + 1 = 当前节点在子树中的排名。
//
//   查找第 k 小逻辑：
//     k == currentRank → 命中，返回当前节点
//     k <  currentRank → 目标在左子树中，k 不变
//     k >  currentRank → 目标在右子树中，k -= currentRank
//
//   示例（根 8，leftSize=3）：
//         8(rank=4)
//        /        ╲
//       3(rank=2) 12(rank=2)
//      / \        /  ╲
//     1   6     10   14
//
//   找 k=2：根 rank=4, 2<4 → 左子 rank=2, 2=2 → 命中 3
//   找 k=5：根 rank=4, 5>4 → 右子, k=5-4=1, rank=2, 1<2 → 左子10, rank=1 → 命中 10
//
//   O(log n)：每次根据排名判断走左或右，树高决定步数。
//   这个技巧用于实现"有序集合"，如 __gnu_pbds::tree。
// ============================================================

class AugmentedNode
{
public:
    int info;
    int leftSize;
    AugmentedNode* left;
    AugmentedNode* right;
    AugmentedNode(int val, int lSize = 0)
        : info(val), leftSize(lSize), left(nullptr), right(nullptr) {}
};

// ----------------------------------------------------------
// findKth(root, k)：在增强 BST 中查找第 k 小的元素，O(log n)
//
// 目的：不遍历整棵树，通过节点的 leftSize（左子树的节点数）
//       直接算出当前节点在子树中的排名，据此决定走左还是走右。
//
// 操作思路（类比二分查找）：
//   currentRank = leftSize + 1（当前节点在子树中排第几）
//   - k == currentRank → 命中！当前节点就是第 k 小
//   - k <  currentRank → 目标在左子树中，递归左子（k 不变）
//   - k >  currentRank → 目标在右子树中，递归右子（k -= currentRank，
//                         因为在右子树中排名要"减掉"左侧所有比自己小的节点）
//
// 为什么 O(log n)？
//   每次根据排名判断方向，只走一条路径，树高就是最坏步数。
//
// 示例（根 8，leftSize=3）：
//         8(rank=4)
//        /        ╲
//    3(rank=2)  12(rank=2)
//     / \        /  ╲
//    1   6     10   14
//   找 k=2：根 rank=4 > 2 → 走左。3 的 rank=2 → 命中！
//   找 k=5：根 rank=4 < 5 → 走右，k=5-4=1。12 的 rank=2 > 1 → 走左。
//           10 的 rank=1 → 命中！
//
// 应用：这种增强 BST 实际上是"有序集合"（Ordered Set）的底层结构。
//       C++ 的 __gnu_pbds::tree 就使用类似技术。
// ----------------------------------------------------------
AugmentedNode* findKth(AugmentedNode* root, int k)
{
    if (!root) return nullptr;
    int currentRank = root->leftSize + 1;
    if (k == currentRank)
        return root;
    else if (k < currentRank)
        return findKth(root->left, k);
    else
        return findKth(root->right, k - currentRank);
}

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║   BST + 遍历 + 第K小 综合测试     ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    // ---- Part 1 & 2: BST + 遍历 ----
    cout << "\n┌─ Part 1: BST 插入与验证 ────────┐" << endl;
    Tree t;
    t.BSTInsert(4); t.BSTInsert(2); t.BSTInsert(6);
    t.BSTInsert(1); t.BSTInsert(3); t.BSTInsert(5); t.BSTInsert(7);
    cout << "树高=" << t.height() << " (expect 2)" << endl;
    cout << "isBST=" << t.isBST() << " (expect 1)" << endl;
    cout << "中序: "; t.Inorder();

    cout << "\n┌─ Part 2: 四种遍历 ──────────────┐" << endl;
    cout << "前序: "; t.Preorder();
    cout << "后序: "; t.Postorder();
    cout << "层序:" << endl; t.BFS();

    cout << "\n┌─ BST 删除测试 ─────────────────┐" << endl;
    t.BSTDelete(2);  // 有一个子节点
    cout << "删除2后中序: "; t.Inorder();
    cout << "isBST=" << t.isBST() << " (expect 1)" << endl;

    // ---- Part 3: 第K小 ----
    cout << "\n┌─ Part 3: 增强树查第K小 ──────────┐" << endl;
    // 手动构建增强 BST（演示 leftSize 机制）
    //        8(rank=4)
    //       /        ╲
    //   3(rank=2)  12(rank=2)
    //    / \        /  ╲
    //   1   6     10   14
    AugmentedNode* root = new AugmentedNode(8, 3);
    root->left  = new AugmentedNode(3, 1);
    root->right = new AugmentedNode(12, 1);
    root->left->left  = new AugmentedNode(1, 0);
    root->left->right = new AugmentedNode(6, 0);
    root->right->left  = new AugmentedNode(10, 0);
    root->right->right = new AugmentedNode(14, 0);

    for (int k = 1; k <= 7; k++)
    {
        AugmentedNode* res = findKth(root, k);
        cout << "第" << k << "小 = " << (res ? res->info : -1) << endl;
    }

    cout << "\n所有测试完成！" << endl;
    return 0;
}
