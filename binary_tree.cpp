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
//          /  \
//         5   15
//            /  \
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

    // 树高：空树 -1，叶节点 0
    int height(TreeNode* node)
    {
        if (!node) return -1;
        return max(height(node->Left), height(node->Right)) + 1;
    }

    // 递归插入：重复值跳过
    TreeNode* BSTinsert(TreeNode* node, int val)
    {
        if (!node) return new TreeNode(val);
        if (val < node->data)
            node->Left = BSTinsert(node->Left, val);
        else if (val > node->data)
            node->Right = BSTinsert(node->Right, val);
        return node;
    }

    // 找子树最小节点：一直向左走
    TreeNode* FindMin(TreeNode* node)
    {
        if (!node->Left) return node;
        return FindMin(node->Left);
    }

    // 递归删除（三种情况）
    //
    // 关键细节：每次递归调用必须把返回值赋给对应的子指针！
    //   node->Left = BSTdelete(node->Left, val);
    //
    // 如果写成 BSTdelete(node->Left, val)（不接收返回值），
    // 当删除的是 node->Left 本身时，node->Left 仍指向已释放的内存
    // → 野指针/悬垂指针，后续访问会崩溃。
    TreeNode* BSTdelete(TreeNode* node, int val)
    {
        if (!node) { cout << val << " is not found" << endl; return nullptr; }

        if (val < node->data)
            node->Left = BSTdelete(node->Left, val);   // 在左子树中找 + 更新左子指针
        else if (val > node->data)
            node->Right = BSTdelete(node->Right, val);  // 在右子树中找 + 更新右子指针
        else
        {
            // 找到了！node 就是要删除的节点

            // 情况 1 & 2：0 或 1 个子节点
            // 返回非空子节点（或 nullptr）来替代自己
            if (!node->Left)
            {
                TreeNode* temp = node->Right;  // 可能是子节点也可能是 nullptr
                delete node;
                return temp;  // 父节点会收到这个返回值并更新对应的子指针
            }
            else if (!node->Right)
            {
                TreeNode* temp = node->Left;
                delete node;
                return temp;
            }
            // 情况 3：有两个子节点 → 后继替换策略
            // 为什么用后继（右子树最小节点）？
            //   后继是"比 node 大的最小节点"。
            //   把它放到 node 的位置上：
            //     - 右子树的节点都比后继大 ✓
            //     - 左子树的节点都比 node 小，而 node < 后继 → 左子树也比后继小 ✓
            TreeNode* Min = FindMin(node->Right);
            node->data = Min->data;                       // 用后继的值覆盖
            node->Right = BSTdelete(node->Right, Min->data);  // 删除原后继节点
        }
        return node;
    }

    // 验证 BST：利用中序遍历严格升序的性质
    // prev 是 int* 的引用（int*&），所有递归层级共享同一个指针变量。
    // 初始调用时传 nullptr，表示还没有前驱。
    bool isBSTHelper(TreeNode* node, int*& prev)
    {
        if (!node) return true;
        if (!isBSTHelper(node->Left, prev)) return false;
        if (prev && node->data <= *prev) return false;
        prev = &node->data;
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
    // 【前序遍历 NLR】根→左→右  适合复制树（先创建根再递归子节点）
    // 【中序遍历 LNR】左→根→右  BST 中序 = 升序输出
    // 【后序遍历 LRN】左→右→根  适合释放内存（先删子节点再删父节点）
    // 【层序遍历 BFS】逐层从左到右，用队列实现
    //
    //   示例树：
    //         4
    //        / \
    //       2   6
    //      / \ / \
    //     1  3 5 7
    //
    //   前序: 4 2 1 3 6 5 7
    //   中序: 1 2 3 4 5 6 7  （BST：升序！）
    //   后序: 1 3 2 5 7 6 4
    //   层序: Level0: 4 / Level1: 2 6 / Level2: 1 3 5 7
    // ============================================================

private:
    void PreorderHelper(TreeNode* node)
    {
        if (!node) return;
        cout << node->data << " ";
        PreorderHelper(node->Left);
        PreorderHelper(node->Right);
    }

    void InorderHelper(TreeNode* node)
    {
        if (!node) return;
        InorderHelper(node->Left);
        cout << node->data << " ";
        InorderHelper(node->Right);
    }

    void PostorderHelper(TreeNode* node)
    {
        if (!node) return;
        PostorderHelper(node->Left);
        PostorderHelper(node->Right);
        cout << node->data << " ";
    }

    // 层序遍历（BFS）—— 使用队列逐层处理
    //
    // 关键技巧：levelSize = q.size() 在每层开始时记录当前层节点数。
    // 因为处理过程中新子节点会入队，但 levelSize 不变，
    // 所以 for 循环恰好处理完当前层的所有节点。
    //
    // 没有 levelSize 会怎样？所有节点混在一行，无法区分层级。
    void BFSHelper(TreeNode* node)
    {
        if (!node) return;
        queue<TreeNode*> q;
        q.push(node);
        int level = 0;
        while (!q.empty())
        {
            int levelSize = q.size();  // 当前层的节点数（处理前拍快照）
            cout << "Level " << level << ": ";
            for (int i = 0; i < levelSize; i++)
            {
                TreeNode* t = q.front(); q.pop();
                cout << t->data << " ";
                if (t->Left)  q.push(t->Left);   // 子节点进入下一层
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
//        /        \
//       3(rank=2) 12(rank=2)
//      / \        /  \
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
    //       /        \
    //   3(rank=2)  12(rank=2)
    //    / \        /  \
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
