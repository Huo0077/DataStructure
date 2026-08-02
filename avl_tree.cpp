// ============================================================
//           AVL 树（自平衡二叉搜索树）+ 旋转详解
// ============================================================
//
// 【什么是 AVL 树？】
//   AVL 树是最早发明的自平衡二叉搜索树（1962, Adelson-Velsky & Landis）。
//   每次插入/删除后检查 平衡因子，一旦不平衡就通过 旋转 恢复平衡，
//   保证树高始终为 O(log n)。
//
// 【平衡因子】
//   平衡因子 = 左子树高 - 右子树高，合法范围 {-1, 0, 1}。
//   若为 -2 或 2，需要旋转修正。
//
// 【为什么需要 AVL 树？】
//   普通 BST 最坏（如依次插入 1,2,3,4,5）退化成链表，查找 O(n)。
//   AVL 树强制平衡保证查找永远是 O(log n)。
//   代价：每次插入/删除需要额外旋转开销。
//
// 【四种失衡与旋转】
//
//   LL（左子树的左边插入）→ 右旋一次
//         y(bf=2)           x
//        / \               / ╲
//       x   T3    →      T1   y
//      / \                   / ╲
//     T1 T2                 T2 T3
//     ↑插入
//
//   RR（右子树的右边插入）→ 左旋一次（镜像）
//
//   LR（左子树的右边插入）→ 先左旋左子，再右旋根
//         y(bf=2)         y(bf=2)            z
//        / \              / \               /  ╲
//       x   T4   →       z   T4    →      x    y
//      / \              / \              / \   / ╲
//     T1  z            x  T3            T1 T2 T3 T4
//        / \          / ╲
//       T2 T3        T1 T2
//       ↑插入
//
//   RL（右子树的左边插入）→ 先右旋右子，再左旋根（镜像）
//
//   口诀：LL 右旋，RR 左旋，LR 左右旋，RL 右左旋
//
// 【拷贝构造函数 & 深拷贝】
//   编译器默认的拷贝构造是浅拷贝（只复制指针，不复制指向的内容）。
//   两个对象共享同一棵树 → 修改互相影响 + 析构时 double free。
//   必须深拷贝：递归复制整棵树，创建完全独立的节点。
//
//   三法则（Rule of Three）：
//   需要自定义 析构/拷贝构造/赋值运算符 中任意一个 → 通常三者都需要。
//
// ============================================================

#include<iostream>
#include<algorithm>
#include<queue>
using namespace std;

// 【图示约定与阅读地图】
//   图中的 T1..T4 代表可能为空、但键范围正确的整棵子树；箭头右侧是旋转完成后的局部结构。
//   递归回溯时固定执行：更新子树高度 -> 计算平衡因子 -> 选择旋转。
//   旋转后必须先更新较低节点 y，再更新新根 x；否则 x 会读取到过期的子树高度。

class Node
{
public:
    int data;
    int height;    // 子树高度，叶节点 = 1
    Node* left;
    Node* right;

    Node(int val) : data(val), height(1), left(nullptr), right(nullptr) {}
};

class AVLtree
{
private:
    Node* root;

    int getHeight(Node* node) const
    {
        return node ? node->height : 0;
    }

    int getBalance(Node* node)
    {
        if (!node) return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    // ----------------------------------------------------------
    // 右旋转（处理 LL 失衡）
    //
    //    y(bf=2)            x(bf=0~1)
    //   / \                / ╲
    //  x  T3    →        T1   y
    // / \                    / ╲
    //T1 T2                  T2 T3
    //
    // 三个步骤：
    //   1. x = y->left        — 新根
    //   2. y->left = x->right — T2 过继给 y（T2 介于 x 和 y 之间）
    //   3. x->right = y       — y 成为 x 的右子
    //
    // 为什么必须先更新 y 再更新 x？
    //   x 的高度依赖其新右子 y 的高度。
    //   如果先更新 x，它用的还是 y 的旧高度 → 错误。
    // ----------------------------------------------------------
    Node* Rrotate(Node* y)
    {
        Node* x = y->left;
        Node* T2 = x->right;       // T2 是 x 和 y 之间的值：x < T2 < y

        x->right = y;              // y 成为 x 的右子
        y->left = T2;              // T2 过继给 y 作为左子

        // 先更新 y（下层），再更新 x（上层）
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;  // 新根
    }

    // 左旋转（处理 RR 失衡）—— 右旋的镜像对称
    //
    //    y(bf=-2)           x
    //   / \                / ╲
    //  T1  x     →        y  T3
    //     / \            / ╲
    //    T2 T3          T1 T2
    Node* Lrotate(Node* y)
    {
        Node* x = y->right;
        Node* T2 = x->left;       // T2 介于 y 和 x 之间：y < T2 < x

        x->left = y;
        y->right = T2;

        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;
    }

    // 递归插入：BST 插入 → 更新高度 → 检查平衡 → 四种旋转修复
    //
    // 每一种失衡的判断包含两部分：
    //   ① balance 的符号和大小 → 判断失衡方向（左倾 LL/LR 还是右倾 RR/RL）
    //   ② val 与子节点的大小关系 → 判断插入发生在子树的哪一侧
    //
    // LL 例：balance > 1（左倾）且 val < node->left->data → 插在左子左边
    // LR 例：balance > 1（左倾）但 val > node->left->data → 插在左子右边
    Node* insertHelper(Node* node, int val)
    {
        // --- 标准 BST 插入 ---
        if (!node) return new Node(val);

        if (val < node->data)
            node->left = insertHelper(node->left, val);
        else if (val > node->data)
            node->right = insertHelper(node->right, val);
        else
            return node;  // 重复值，跳过

        // --- 回溯：更新高度 ---
        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

        // --- 检查平衡并旋转修复 ---
        int balance = getBalance(node);

        // LL：左子高 + 新节点在左子左边 → 右旋一次
        if (balance > 1 && val < node->left->data)
            return Rrotate(node);

        // RR：右子高 + 新节点在右子右边 → 左旋一次
        if (balance < -1 && val > node->right->data)
            return Lrotate(node);

        // LR：左子高 + 新节点在左子右边 → 左旋左子 → 变 LL → 右旋
        if (balance > 1 && val > node->left->data)
        {
            node->left = Lrotate(node->left);   // 左子左旋 → 变成 LL 形态
            return Rrotate(node);                // 当前节点右旋
        }

        // RL：右子高 + 新节点在右子左边 → 右旋右子 → 变 RR → 左旋
        if (balance < -1 && val < node->right->data)
        {
            node->right = Rrotate(node->right);  // 右子右旋 → 变成 RR 形态
            return Lrotate(node);                 // 当前节点左旋
        }

        return node;
    }

    Node* findMin(Node* node)
    {
        if (!node->left) return node;
        return findMin(node->left);
    }

    // 递归删除：BST 删除 → 更新高度 → 检查平衡 → 旋转
    // 注意：删除可能导致多层祖先失衡，每层都要检查修复
    Node* removeHelper(Node* node, int val)
    {
        if (!node) return nullptr;

        if (val < node->data)
            node->left = removeHelper(node->left, val);
        else if (val > node->data)
            node->right = removeHelper(node->right, val);
        else
        {
            // 0 或 1 个子节点
            if (!node->left)
            {
                Node* temp = node->right;
                delete node;
                return temp;
            }
            else if (!node->right)
            {
                Node* temp = node->left;
                delete node;
                return temp;
            }
            // 2 个子节点：后继替换
            Node* successor = findMin(node->right);
            node->data = successor->data;
            node->right = removeHelper(node->right, successor->data);
        }

        if (!node) return nullptr;

        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;
        int balance = getBalance(node);

        // LL
        if (balance > 1 && getBalance(node->left) >= 0)
            return Rrotate(node);
        // LR
        if (balance > 1 && getBalance(node->left) < 0)
        {
            node->left = Lrotate(node->left);
            return Rrotate(node);
        }
        // RR
        if (balance < -1 && getBalance(node->right) <= 0)
            return Lrotate(node);
        // RL
        if (balance < -1 && getBalance(node->right) > 0)
        {
            node->right = Rrotate(node->right);
            return Lrotate(node);
        }

        return node;
    }

    void clear(Node* node)
    {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    void preorderHelper(Node* node)
    {
        if (!node) return;
        cout << node->data << " ";
        preorderHelper(node->left);
        preorderHelper(node->right);
    }

    void inorderHelper(Node* node)
    {
        if (!node) return;
        inorderHelper(node->left);
        cout << node->data << " ";
        inorderHelper(node->right);
    }

    void postorderHelper(Node* node)
    {
        if (!node) return;
        postorderHelper(node->left);
        postorderHelper(node->right);
        cout << node->data << " ";
    }

    // 深拷贝：后序遍历逻辑——先复制左右子树，再创建当前节点
    Node* copyHelper(Node* node)
    {
        if (!node) return nullptr;
        Node* newNode = new Node(node->data);
        newNode->height = node->height;
        newNode->left = copyHelper(node->left);
        newNode->right = copyHelper(node->right);
        return newNode;
    }

    int countNodesHelper(Node* node) const
    {
        if (!node) return 0;
        return 1 + countNodesHelper(node->left) + countNodesHelper(node->right);
    }

    void BFSHelper(Node* node) const
    {
        if (!node) { cout << "树为空" << endl; return; }
        queue<Node*> q;
        q.push(node);
        int level = 0;
        while (!q.empty())
        {
            int levelSize = q.size();
            cout << "第" << level << "层: ";
            for (int i = 0; i < levelSize; i++)
            {
                Node* cur = q.front(); q.pop();
                cout << cur->data << " ";
                if (cur->left)  q.push(cur->left);
                if (cur->right) q.push(cur->right);
            }
            cout << endl;
            level++;
        }
    }

    // 验证 AVL：① BST 性质 ② 平衡因子 {-1,0,1}
    int verifyHelper(Node* node) const
    {
        if (!node) return 0;
        int lh = verifyHelper(node->left);
        if (lh == -1) return -1;
        int rh = verifyHelper(node->right);
        if (rh == -1) return -1;

        if (node->left && node->data <= node->left->data) return -1;
        if (node->right && node->data >= node->right->data) return -1;

        int balance = lh - rh;
        if (balance < -1 || balance > 1) return -1;

        return max(lh, rh) + 1;
    }

public:
    AVLtree() : root(nullptr) {}
    ~AVLtree() { clear(root); }

    // 深拷贝构造
    AVLtree(const AVLtree& other)
    {
        root = copyHelper(other.root);
    }

    void insert(int val) { root = insertHelper(root, val); }
    void remove(int val) { root = removeHelper(root, val); }

    // const 成员函数：不修改对象的只读操作
    bool search(int val) const
    {
        Node* cur = root;
        while (cur)
        {
            if (val < cur->data)      cur = cur->left;
            else if (val > cur->data) cur = cur->right;
            else                      return true;
        }
        return false;
    }

    bool isEmpty() const      { return root == nullptr; }
    int getHeight() const     { return getHeight(root); }
    int countNodes() const    { return countNodesHelper(root); }
    bool isValidAVL() const   { return verifyHelper(root) != -1; }
    void BFS() const          { BFSHelper(root); }

    void preorder()  { preorderHelper(root); cout << endl; }
    void inorder()   { inorderHelper(root); cout << endl; }
    void postorder() { postorderHelper(root); cout << endl; }
};

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║     AVL 树 综 合 测 试           ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    AVLtree tree;

    // 测试1：插入会导致普通BST退化的序列 1~10
    // 普通 BST：高度=10（链表）
    // AVL 树：高度 ≤ 1.44*log₂(10) ≈ 5
    cout << "\n┌─ 插入 1~10 ────────────────────┐" << endl;
    for (int i = 1; i <= 10; i++) tree.insert(i);
    cout << "树高: " << tree.getHeight() << " (普通BST退化=10)" << endl;
    cout << "合法AVL: " << (tree.isValidAVL() ? "是" : "否") << endl;
    cout << "中序: "; tree.inorder();

    // 测试2：查找
    cout << "\n┌─ 查找测试 ────────────────────┐" << endl;
    for (int v : {3, 7, 11, 1, 10})
        cout << "search(" << v << ")=" << tree.search(v) << endl;

    // 测试3：删除
    cout << "\n┌─ 删除5 ───────────────────────┐" << endl;
    tree.remove(5);
    cout << "中序: "; tree.inorder();
    cout << "树高: " << tree.getHeight() << ", 合法AVL: " << (tree.isValidAVL() ? "是" : "否") << endl;

    // 测试4：删除不存在值
    cout << "\n┌─ 删除不存在值100 ──────────────┐" << endl;
    tree.remove(100);
    cout << "中序: "; tree.inorder();

    // 测试5：深拷贝
    cout << "\n┌─ 深拷贝测试 ──────────────────┐" << endl;
    AVLtree tree2(tree);  // 拷贝构造
    tree2.insert(100);
    cout << "原树中序: "; tree.inorder();
    cout << "副本中序(含100): "; tree2.inorder();

    // 测试6：层序遍历
    cout << "\n┌─ 层序遍历 ────────────────────┐" << endl;
    tree.BFS();

    cout << "\n所有测试完成！" << endl;
    return 0;
}
