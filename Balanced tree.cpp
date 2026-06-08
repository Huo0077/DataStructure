#include<iostream>
#include<algorithm>
using namespace std;

// ============================================================
//                    AVL 树（自平衡二叉搜索树）
// ============================================================
//
// 【什么是 AVL 树？】
//   AVL 树是最早发明的自平衡二叉搜索树（1962年，Adelson-Velsky & Landis）。
//   它在每次插入/删除后检查每个节点的 平衡因子，一旦发现不平衡
//   就通过 旋转 恢复平衡，保证树高始终为 O(log n)。
//
// 【平衡因子】
//   节点平衡因子 = 左子树高度 - 右子树高度
//   AVL 树要求：每个节点的平衡因子 ∈ {-1, 0, 1}
//   如果某个节点平衡因子为 -2 或 2，就需要旋转修正。
//
// 【为什么需要 AVL 树？】
//   普通 BST 在最坏情况下（如依次插入 1,2,3,4,5）会退化成链表，
//   查找变成 O(n)。AVL 树通过强制平衡保证查找永远是 O(log n)。
//
// 【四种失衡与旋转】
//
//   情况1: LL（左子树的左边插入）→ 右旋一次
//         y(平衡=2)           x
//        / \                 / \
//       x   T3     →       T1   y
//      / \                     / \
//     T1 T2                   T2 T3
//     ↑插入
//
//   情况2: RR（右子树的右边插入）→ 左旋一次
//         y(平衡=-2)          x
//        / \                 / \
//       T1  x       →       y  T3
//          / \             / \
//         T2 T3           T1 T2
//            ↑插入
//
//   情况3: LR（左子树的右边插入）→ 先左旋左子，再右旋根
//         y(平衡=2)         y(平衡=2)           z
//        / \               / \                 / \
//       x   T4    →       z   T4      →      x   y
//      / \               / \                 / \ / \
//     T1  z             x  T3               T1 T2 T3 T4
//        / \           / \
//       T2 T3         T1 T2
//       ↑插入
//
//   情况4: RL（右子树的左边插入）→ 先右旋右子，再左旋根
//         y(平衡=-2)        y(平衡=-2)           z
//        / \               / \                 / \
//       T1  x      →      T1  z       →       y   x
//          / \               / \             / \ / \
//         z  T4             T2  x           T1 T2 T3 T4
//        / \                   / \
//       T2 T3                 T3 T4
//       ↑插入
//
// 【记忆技巧】
//   看"插入发生在哪一侧"命名：
//     LL = 左子树的 左 边 → 右旋（Right Rotation）
//     RR = 右子树的 右 边 → 左旋（Left Rotation）
//     LR = 左子树的 右 边 → 左旋 + 右旋（先左后右）
//     RL = 右子树的 左 边 → 右旋 + 左旋（先右后左）
//
//   口诀：LL 右旋，RR 左旋，LR 左右旋，RL 右左旋
//
// ============================================================

class Node
{
public:
    int data;
    int height;    // 以该节点为根的子树高度，叶节点高度 = 1
    Node* left;
    Node* right;

    Node(int val) : data(val), height(1), left(nullptr), right(nullptr) {}
    //                         ↑ 新节点一定是叶节点，高度为 1
};

class AVLtree
{
private:
    Node* root;

    // ----------------------------------------------------------
    // 获取节点高度（空节点高度为 0，与叶节点高度 1 区分）
    // ----------------------------------------------------------
    int getHeight(Node* node)
    {
        if (node == nullptr)
        {
            return 0;
        }
        return node->height;
    }

    // ----------------------------------------------------------
    // 计算节点的平衡因子
    //   = 左子树高度 - 右子树高度
    //   正值 → 左倾，负值 → 右倾
    //   合法范围：-1, 0, 1
    // ----------------------------------------------------------
    int getBalance(Node* node)
    {
        if (node == nullptr)
        {
            return 0;
        }
        return getHeight(node->left) - getHeight(node->right);
    }

    // ----------------------------------------------------------
    // 右旋转（处理 LL 失衡）
    //
    //    y                x
    //   / \              / \
    //  x  T3    →      T1   y
    // / \                  / \
    //T1 T2                T2 T3
    //
    // 步骤：
    //   1. x = y->left（保存新根）
    //   2. y->left = x->right（T2 过继给 y）
    //   3. x->right = y（y 成为 x 的右子）
    //   4. 更新 y 的高度，再更新 x 的高度（顺序不能反，因为 x 依赖 y）
    //   5. 返回新根 x
    // ----------------------------------------------------------
    Node* Rrotate(Node* y)
    {
        Node* x = y->left;
        Node* T2 = x->right;

        // 执行旋转
        x->right = y;
        y->left = T2;

        // 更新高度：必须先更新 y（下层），再更新 x（上层）
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;  // 返回新根
    }

    // ----------------------------------------------------------
    // 左旋转（处理 RR 失衡）
    //
    //    y                x
    //   / \              / \
    //  T1  x     →      y  T3
    //     / \          / \
    //    T2 T3        T1 T2
    //
    // 与右旋转镜像对称
    // ----------------------------------------------------------
    Node* Lrotate(Node* y)
    {
        Node* x = y->right;
        Node* T2 = x->left;

        // 执行旋转
        x->left = y;
        y->right = T2;

        // 更新高度：必须先更新 y（下层），再更新 x（上层）
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

        return x;  // 返回新根
    }

    // ----------------------------------------------------------
    // 递归插入（私有辅助函数）
    //
    // 流程：
    //   1. 按 BST 规则递归插入到左/右子树
    //   2. 回溯时更新当前节点高度
    //   3. 计算平衡因子，判断是否失衡
    //   4. 根据四种情况选择旋转策略
    //
    // 注意：树中不能有重复值，重复值直接忽略
    // ----------------------------------------------------------
    Node* insertHelper(Node* node, int val)
    {
        // ---------- 第一步：标准 BST 插入 ----------
        if (node == nullptr)
        {
            return new Node(val);
        }

        if (val < node->data)
        {
            node->left = insertHelper(node->left, val);
        }
        else if (val > node->data)
        {
            node->right = insertHelper(node->right, val);
        }
        else
        {
            // 重复值，不做任何操作
            return node;
        }

        // ---------- 第二步：更新当前节点高度 ----------
        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

        // ---------- 第三步：检查平衡因子 ----------
        int balance = getBalance(node);

        // ---------- 第四步：四种失衡情况处理 ----------

        // LL 情况：左子树高，且新节点在 左子树的左边
        //   balance > 1 说明左倾超过 1
        //   val < node->left->data 说明插入发生在左子树的左边
        if (balance > 1 && val < node->left->data)
        {
            return Rrotate(node);  // 右旋一次
        }

        // RR 情况：右子树高，且新节点在 右子树的右边
        //   balance < -1 说明右倾超过 1
        //   val > node->right->data 说明插入发生在右子树的右边
        if (balance < -1 && val > node->right->data)
        {
            return Lrotate(node);  // 左旋一次
        }

        // LR 情况：左子树高，但新节点在 左子树的右边
        //   先对左子左旋 → 变成 LL 形态 → 再对当前节点右旋
        if (balance > 1 && val > node->left->data)
        {
            node->left = Lrotate(node->left);  // 左子左旋
            return Rrotate(node);               // 当前节点右旋
        }

        // RL 情况：右子树高，但新节点在 右子树的左边
        //   先对右子右旋 → 变成 RR 形态 → 再对当前节点左旋
        if (balance < -1 && val < node->right->data)
        {
            node->right = Rrotate(node->right);  // 右子右旋
            return Lrotate(node);                 // 当前节点左旋
        }

        return node;
    }

    // ----------------------------------------------------------
    // 找子树中值最小的节点（一直向左走到底）
    // 用于删除操作：当被删节点有两个子节点时，用后继节点替换
    // ----------------------------------------------------------
    Node* findMin(Node* node)
    {
        if (node->left == nullptr)
        {
            return node;
        }
        return findMin(node->left);
    }

    // ----------------------------------------------------------
    // 递归删除（私有辅助函数）
    //
    // 流程：
    //   1. 按 BST 规则找到并删除节点
    //   2. 回溯时更新高度
    //   3. 检查平衡并旋转修复
    //
    // 与插入不同：删除可能导致更高层的祖先也失衡，
    // 所以每一层回溯都要检查和修复（不像插入只影响路径上第一个失衡点）。
    // ----------------------------------------------------------
    Node* removeHelper(Node* node, int val)
    {
        // ---------- 第一步：标准 BST 删除 ----------
        if (node == nullptr)
        {
            return nullptr;
        }

        if (val < node->data)
        {
            node->left = removeHelper(node->left, val);
        }
        else if (val > node->data)
        {
            node->right = removeHelper(node->right, val);
        }
        else
        {
            // 找到了要删除的节点

            // 情况 A：只有一个子节点或没有子节点
            if (node->left == nullptr)
            {
                Node* temp = node->right;
                delete node;
                return temp;
            }
            else if (node->right == nullptr)
            {
                Node* temp = node->left;
                delete node;
                return temp;
            }
            // 情况 B：有两个子节点
            // 用右子树的最小节点（后继）替换当前节点，
            // 然后递归删除后继节点
            else
            {
                Node* successor = findMin(node->right);
                node->data = successor->data;
                node->right = removeHelper(node->right, successor->data);
            }
        }

        // 如果删除的是叶节点，直接返回
        if (node == nullptr)
        {
            return nullptr;
        }

        // ---------- 第二步：更新高度 ----------
        node->height = max(getHeight(node->left), getHeight(node->right)) + 1;

        // ---------- 第三步：检查并修复平衡 ----------
        int balance = getBalance(node);

        // LL 情况
        if (balance > 1 && getBalance(node->left) >= 0)
        {
            return Rrotate(node);
        }

        // LR 情况
        if (balance > 1 && getBalance(node->left) < 0)
        {
            node->left = Lrotate(node->left);
            return Rrotate(node);
        }

        // RR 情况
        if (balance < -1 && getBalance(node->right) <= 0)
        {
            return Lrotate(node);
        }

        // RL 情况
        if (balance < -1 && getBalance(node->right) > 0)
        {
            node->right = Rrotate(node->right);
            return Lrotate(node);
        }

        return node;
    }

    // ----------------------------------------------------------
    // 递归释放所有节点（析构函数调用）
    // ----------------------------------------------------------
    void clear(Node* node)
    {
        if (node == nullptr)
        {
            return;
        }
        clear(node->left);
        clear(node->right);
        delete node;
    }

    // ----------------------------------------------------------
    // 遍历辅助函数
    // ----------------------------------------------------------

    // 前序遍历：根 → 左 → 右
    void preorderHelper(Node* node)
    {
        if (node == nullptr)
        {
            return;
        }
        cout << node->data << " ";
        preorderHelper(node->left);
        preorderHelper(node->right);
    }

    // 中序遍历：左 → 根 → 右（BST 中序遍历 = 升序输出！）
    void inorderHelper(Node* node)
    {
        if (node == nullptr)
        {
            return;
        }
        inorderHelper(node->left);
        cout << node->data << " ";
        inorderHelper(node->right);
    }

    // 后序遍历：左 → 右 → 根
    void postorderHelper(Node* node)
    {
        if (node == nullptr)
        {
            return;
        }
        postorderHelper(node->left);
        postorderHelper(node->right);
        cout << node->data << " ";
    }

    // ----------------------------------------------------------
    // 验证是否为合法 AVL 树
    // 条件：① BST 性质（有序） ② 平衡因子 ∈ {-1, 0, 1}
    // 返回树高，若非法则返回 -1
    // ----------------------------------------------------------
    int verifyHelper(Node* node)
    {
        if (node == nullptr)
        {
            return 0;
        }

        int leftHeight = verifyHelper(node->left);
        if (leftHeight == -1)
        {
            return -1;  // 左子树非法
        }

        int rightHeight = verifyHelper(node->right);
        if (rightHeight == -1)
        {
            return -1;  // 右子树非法
        }

        // 检查 BST 性质：左子树所有值 < 当前值 < 右子树所有值
        if (node->left != nullptr && node->data <= node->left->data)
        {
            return -1;
        }
        if (node->right != nullptr && node->data >= node->right->data)
        {
            return -1;
        }

        // 检查平衡因子
        int balance = leftHeight - rightHeight;
        if (balance < -1 || balance > 1)
        {
            return -1;
        }

        return max(leftHeight, rightHeight) + 1;
    }

public:
    // ----------------------------------------------------------
    // 构造函数 & 析构函数
    // ----------------------------------------------------------
    AVLtree() : root(nullptr) {}

    ~AVLtree()
    {
        clear(root);
    }

    // ----------------------------------------------------------
    // 公有接口：插入
    // ----------------------------------------------------------
    void insert(int val)
    {
        root = insertHelper(root, val);
    }

    // ----------------------------------------------------------
    // 公有接口：删除
    // ----------------------------------------------------------
    void remove(int val)
    {
        root = removeHelper(root, val);
    }

    // ----------------------------------------------------------
    // 公有接口：查找
    // ----------------------------------------------------------
    bool search(int val)
    {
        Node* cur = root;
        while (cur != nullptr)
        {
            if (val < cur->data)
            {
                cur = cur->left;
            }
            else if (val > cur->data)
            {
                cur = cur->right;
            }
            else
            {
                return true;
            }
        }
        return false;
    }

    // ----------------------------------------------------------
    // 获取整棵树的高度
    // ----------------------------------------------------------
    int getHeight()
    {
        return getHeight(root);
    }

    // ----------------------------------------------------------
    // 验证是否为合法的 AVL 树
    // ----------------------------------------------------------
    bool isValidAVL()
    {
        return verifyHelper(root) != -1;
    }

    // ----------------------------------------------------------
    // 三种遍历
    // ----------------------------------------------------------

    // 前序遍历：根 → 左 → 右
    void preorder()
    {
        preorderHelper(root);
        cout << endl;
    }

    // 中序遍历：左 → 根 → 右（输出为升序，验证 BST 性质）
    void inorder()
    {
        inorderHelper(root);
        cout << endl;
    }

    // 后序遍历：左 → 右 → 根
    void postorder()
    {
        postorderHelper(root);
        cout << endl;
    }
};

// ============================================================
// 主函数：演示 AVL 树的插入、删除、遍历和自平衡特性
// ============================================================
int main()
{
    AVLtree tree;

    // ---------- 测试 1：依次插入导致普通 BST 退化的序列 ----------
    // 普通 BST 插入 1,2,3,4,5 会退化成链表（高度=5）
    // AVL 树会自动调整，保持高度 ≤ 1.44 * log₂(n)
    cout << "========== 插入 1 ~ 10 ==========" << endl;
    for (int i = 1; i <= 10; i++)
    {
        tree.insert(i);
    }

    cout << "树高: " << tree.getHeight() << endl;
    cout << "是否为合法 AVL 树: " << (tree.isValidAVL() ? "是 ✓" : "否 ✗") << endl;
    cout << endl;

    cout << "前序遍历（根左右）: ";
    tree.preorder();
    cout << "中序遍历（左根右）: ";
    tree.inorder();
    cout << "后序遍历（左右根）: ";
    tree.postorder();
    cout << endl;

    // ---------- 测试 2：查找 ----------
    cout << "========== 查找测试 ==========" << endl;
    for (int val : {3, 7, 11, 1, 10})
    {
        cout << "  search(" << val << "): " << (tree.search(val) ? "存在" : "不存在") << endl;
    }
    cout << endl;

    // ---------- 测试 3：删除 ----------
    cout << "========== 删除 5 ==========" << endl;
    tree.remove(5);
    cout << "中序遍历: ";
    tree.inorder();
    cout << "树高: " << tree.getHeight() << endl;
    cout << "是否为合法 AVL 树: " << (tree.isValidAVL() ? "是 ✓" : "否 ✗") << endl;
    cout << endl;

    // ---------- 测试 4：删除根节点 ----------
    cout << "========== 删除根节点 " << endl;
    cout << "中序遍历: ";
    tree.inorder();
    cout << "树高: " << tree.getHeight() << endl;
    cout << "是否为合法 AVL 树: " << (tree.isValidAVL() ? "是 ✓" : "否 ✗") << endl;
    cout << endl;

    // ---------- 测试 5：删除不存在的值 ----------
    cout << "========== 删除不存在的值 100 ==========" << endl;
    tree.remove(100);  // 应该静默处理，不崩溃
    cout << "中序遍历: ";
    tree.inorder();
    cout << "是否为合法 AVL 树: " << (tree.isValidAVL() ? "是 ✓" : "否 ✗") << endl;

    return 0;
}
