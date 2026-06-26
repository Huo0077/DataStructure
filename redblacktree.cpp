// ============================================================
//          红黑树（Red-Black Tree）—— 自平衡二叉搜索树
// ============================================================
//
// 【什么是红黑树？】
//   红黑树是一种自平衡二叉搜索树，由 Rudolf Bayer 于 1972 年提出。
//   它通过给每个节点赋予颜色（红/黑），并维护五条性质来保证近似平衡，
//   确保树高 ≤ 2*log₂(n+1)，从而保证查找、插入、删除均为 O(log n)。
//
//   相比 AVL 树：红黑树平衡条件更宽松，插入/删除时旋转次数更少，
//   因此在写密集型场景（如 STL 的 map/set、Linux 内核的 CFS 调度器、
//   Java 的 TreeMap/TreeSet、epoll 的事件管理）中更受青睐。
//
// 【红黑树的五条性质】
//   1. 每个节点要么是红色，要么是黑色
//   2. 根节点永远是黑色
//   3. 每个叶子节点（NIL / nullptr）被认为是黑色
//   4. 红色节点的两个子节点必须都是黑色（即不允许连续两个红色）
//      → 从根到叶子的路径上，红色节点不能相邻
//   5. 从任意节点出发，到其所有后代叶子节点的每条路径上，
//      包含相同数量的黑色节点（这条被称为"黑高相同"）
//
//   性质 4 和性质 5 共同保证了树不会太"偏"：
//     最长路径（红黑交替）≤ 2 × 最短路径（全黑）
//     因此树高是 O(log n) 级别。
//
//   ┌─────────────────────────────────────────────┐
//   │  性质 4 (不能连续红) + 性质 5 (黑高相同)     │
//   │  → 最长路径 ≤ 2 × 最短路径                  │
//   │  → 高度始终保持在 O(log n)                  │
//   └─────────────────────────────────────────────┘
//
// 【黑高（Black-Height）】
//   节点 x 的黑高 bh(x) = 从 x 到其任意后代叶子的路径上
//   （不包括 x 自身）黑色节点的个数。
//
//   根据性质 5，无论走哪条路径，黑色节点的数量相同，
//   所以 bh(x) 是唯一定义的。
//
//   示例：
//         ● (黑)
//        / \
//       ○   ○ (两个红子)
//
//   从根到叶：路径上只有根是黑的 → bh(根) = 1
//
// 【红黑树 vs AVL 树】
//
//   ┌──────────┬──────────────┬──────────────┐
//   │          │  AVL 树       │  红黑树       │
//   ├──────────┼──────────────┼──────────────┤
//   │ 平衡条件 │ 严格 (BF∈-1,0,1)│ 宽松 (五条性质) │
//   │ 树高     │ ≤ 1.44 log n │ ≤ 2 log n    │
//   │ 查找     │ 略快（树矮）   │ 略慢（树稍高） │
//   │ 插入删除 │ 旋转较多       │ 旋转 ≤ 3 次  │
//   │ 适用场景 │ 查多写少       │ 写多或均衡    │
//   └──────────┴──────────────┴──────────────┘
//
// 【插入操作概述】
//   1. 按照 BST 规则找到位置，插入一个红色节点
//      （为什么是红色？——因为插入红色不会破坏性质 5（黑高），
//       只可能破坏性质 2（根为红）或性质 4（连续红色））
//   2. 通过 recolor（变色）和 rotation（旋转）修复被破坏的性质
//   3. 最多需要 2 次旋转（AVL 插入最多也是 2 次，但红黑树可能
//      需要 O(log n) 次变色）
//
//   插入修正的三种情况（新节点为红，父节点为红时才需要修正）：
//
//   约定：N = 新插入节点，P = 父节点，U = 叔叔节点，G = 祖父节点
//
//   【情况 1】叔叔 U 是红色
//       ┌───┐
//       │ G │ (黑)          ● G (红)
//       └─┬─┘              / \
//        /   \            /   \
//    ● P   ● U  →    ● P     ● U
//    /                       /
//   ● N                     ● N
//
//   处理：P 和 U 变黑，G 变红，然后把 G 当作新插入节点继续向上修正
//
//   【情况 2】叔叔 U 是黑色，且 N 与 P 在不同侧（N 是 P 的右子，P 是 G 的左子）
//       ┌───┐
//       │ G │ (黑)         先对 P 左旋 → 变成情况 3
//       └─┬─┘
//        /   \
//    ● P     ■ U
//       \
//        ● N
//
//   【情况 3】叔叔 U 是黑色，且 N 与 P 在同一侧（N 是 P 的左子，P 是 G 的左子）
//       ┌───┐
//       │ G │ (黑)         对 G 右旋，P 变黑，G 变红
//       └─┬─┘
//        /   \
//    ● P     ■ U
//     /
//    ● N
//
//   口诀：叔红变色往上走，叔黑同侧一旋定，叔黑异侧先转同。
//
// 【删除操作概述】
//   红黑树的删除是最复杂的部分。核心思想：
//   1. 先按 BST 规则删除节点
//   2. 如果被删除（或被移动）的节点是黑色，则"借用"或"重新分布"黑色
//   3. 通过复杂的变色和旋转修复所有性质
//
//   由于删除极其复杂，详细分析见代码中的 deleteFixup 部分。
//
// ============================================================

#include<iostream>
#include<queue>
#include<sstream>
using namespace std;

// ==================== 红黑树节点 ====================

enum class Color { RED, BLACK };

struct RBNode
{
    int data;
    Color color;
    RBNode* left;
    RBNode* right;
    RBNode* parent;   // 红黑树需要父指针，因为修正时需向上回溯

    RBNode(int val)
        : data(val), color(Color::RED), left(nullptr), right(nullptr), parent(nullptr) {}
    // 新节点默认为红色：插入红色不破坏黑高（性质 5），只可能破坏性质 2 和 4
};

// ==================== 红黑树类 ====================

class RedBlackTree
{
private:
    RBNode* root;
    RBNode* NIL;  // 哨兵节点：代表所有空叶子，颜色为黑，简化边界判断

    // ----------------------------------------------------------
    // 初始化哨兵节点
    // 把所有 nullptr 替换为指向 NIL 的指针，这样就不需要每次判断 null
    // ----------------------------------------------------------
    void initNIL()
    {
        NIL = new RBNode(0);
        NIL->color = Color::BLACK;
        NIL->left = NIL;
        NIL->right = NIL;
        NIL->parent = nullptr;
    }

    // ----------------------------------------------------------
    // 左旋转（与 AVL 左旋原理相同，但多了 parent 指针的维护）
    //
    //     y                    x
    //    / \                  / \
    //   T1  x       →        y  T3
    //      / \              / \
    //     T2 T3            T1 T2
    //
    // 旋转会改变三个父子关系：y↔x、x↔T2、y↔parent(x 替代 y 的位置)
    // ----------------------------------------------------------
    void leftRotate(RBNode* y)
    {
        RBNode* x = y->right;
        RBNode* T2 = x->left;

        // 步骤 1：把 T2 过继给 y 的右子
        y->right = T2;
        if (T2 != NIL)
        {
            T2->parent = y;
        }

        // 步骤 2：把 y 挂到 x 的左边
        x->parent = y->parent;
        if (y->parent == nullptr)
        {
            root = x;
        }
        else if (y == y->parent->left)
        {
            y->parent->left = x;
        }
        else
        {
            y->parent->right = x;
        }
        x->left = y;
        y->parent = x;
    }

    // ----------------------------------------------------------
    // 右旋转（左旋的镜像）
    // ----------------------------------------------------------
    void rightRotate(RBNode* y)
    {
        RBNode* x = y->left;
        RBNode* T2 = x->right;

        y->left = T2;
        if (T2 != NIL)
        {
            T2->parent = y;
        }

        x->parent = y->parent;
        if (y->parent == nullptr)
        {
            root = x;
        }
        else if (y == y->parent->right)
        {
            y->parent->right = x;
        }
        else
        {
            y->parent->left = x;
        }
        x->right = y;
        y->parent = x;
    }

    // ----------------------------------------------------------
    // 插入修正（insertFixup）
    //
    // 调用时机：每次 BST 插入完成后，新节点 N 是红色。
    // 需要修正的条件：N 的父节点 P 是红色（违反性质 4）。
    //
    // 三种情况（对称共 6 种，此处以 P 是 G 的左子为例）：
    //
    //   约定：N = 当前关注的红色节点
    //         P = N->parent（红色）
    //         G = P->parent（必然是黑色，因为 P 是红）
    //         U = G 的另一子节点（叔叔）
    //
    //   【情况 1】叔叔 U 是红色 → 变色
    //       将 P 和 U 染黑，G 染红。
    //       为何有效？——给两条路径各增加了 1 个黑色（保留了黑高），
    //       但 G 变红后可能和 G 的父节点产生新的冲突，所以 N 跳到 G 继续。
    //
    //   【情况 2】叔叔 U 是黑色，且 N 是 P 的右子（异侧）→ 旋转 P
    //       对 P 左旋，N 和 P 交换角色 → 变为情况 3
    //
    //   【情况 3】叔叔 U 是黑色，且 N 是 P 的左子（同侧）→ 旋转 G
    //       对 G 右旋，P 染黑，G 染红，结束！
    // ----------------------------------------------------------
    void insertFixup(RBNode* N)
    {
        // 只要父节点是红色就需要处理（违反性质 4：不能连续红）
        while (N->parent != nullptr && N->parent->color == Color::RED)
        {
            RBNode* P = N->parent;
            RBNode* G = P->parent;  // 一定非空（因为 P 是红色，根必须是黑色）

            if (P == G->left)
            {
                // ---- P 是 G 的左子，U 是 G 的右子 ----
                RBNode* U = G->right;

                if (U->color == Color::RED)
                {
                    // 情况 1：叔叔是红色 → 变色，N 跳到 G
                    P->color = Color::BLACK;
                    U->color = Color::BLACK;
                    G->color = Color::RED;
                    N = G;  // 向上传递
                }
                else
                {
                    // 叔叔是黑色
                    if (N == P->right)
                    {
                        // 情况 2：N 是 P 的右子（异侧）→ 先左旋 P
                        N = P;
                        leftRotate(N);
                        // 旋转后 N 和 P 角色互换，变成情况 3
                        P = N->parent;  // 此时 P 是原来的 N
                        // G 不变（还是原来的祖父）
                    }
                    // 情况 3：N 是 P 的左子（同侧）→ 右旋 G
                    P->color = Color::BLACK;
                    G->color = Color::RED;
                    rightRotate(G);
                    // 修正完成，P 已变为黑，循环条件自然结束
                }
            }
            else
            {
                // ---- P 是 G 的右子（镜像对称）----
                RBNode* U = G->left;

                if (U->color == Color::RED)
                {
                    // 情况 1：叔叔是红色
                    P->color = Color::BLACK;
                    U->color = Color::BLACK;
                    G->color = Color::RED;
                    N = G;
                }
                else
                {
                    if (N == P->left)
                    {
                        // 情况 2：N 是 P 的左子（异侧）→ 先右旋 P
                        N = P;
                        rightRotate(N);
                        P = N->parent;
                    }
                    // 情况 3：N 是 P 的右子（同侧）→ 左旋 G
                    P->color = Color::BLACK;
                    G->color = Color::RED;
                    leftRotate(G);
                }
            }
        }
        // 性质 2：根必须是黑色
        root->color = Color::BLACK;
    }

    // ----------------------------------------------------------
    // BST 插入（递归），插入后调用 insertFixup 修正
    // ----------------------------------------------------------
    RBNode* insertBST(RBNode* node, int val, RBNode*& outNewNode)
    {
        if (node == NIL)
        {
            outNewNode = new RBNode(val);
            outNewNode->left = NIL;
            outNewNode->right = NIL;
            return outNewNode;
        }

        if (val < node->data)
        {
            node->left = insertBST(node->left, val, outNewNode);
            node->left->parent = node;
        }
        else if (val > node->data)
        {
            node->right = insertBST(node->right, val, outNewNode);
            node->right->parent = node;
        }
        else
        {
            outNewNode = node;  // 重复值，返回已存在节点
        }
        return node;
    }

    // ----------------------------------------------------------
    // 寻找子树中的最小节点
    // ----------------------------------------------------------
    RBNode* findMin(RBNode* node) const
    {
        while (node->left != NIL)
        {
            node = node->left;
        }
        return node;
    }

    // ----------------------------------------------------------
    // 用 v 替换 u 在树中的位置（移植操作，Transplant）
    // 只改变 v 和 u 的父节点之间的关系，不处理 v 的子节点
    // ----------------------------------------------------------
    void transplant(RBNode* u, RBNode* v)
    {
        if (u->parent == nullptr)
        {
            root = v;
        }
        else if (u == u->parent->left)
        {
            u->parent->left = v;
        }
        else
        {
            u->parent->right = v;
        }
        v->parent = u->parent;
    }

    // ----------------------------------------------------------
    // 删除修正（deleteFixup）
    //
    // 调用时机：当被删除（或移走）的节点是黑色时。
    // 因为删除黑色节点会导致经过该节点的路径减少一个黑色节点，
    // 违反了性质 5（黑高相同）。
    //
    // 核心思想：把 x 看作携带了"额外一层黑色"（双重黑色），
    // 修正过程就是逐步消除这个额外黑色。
    //
    // 约定：x 是当前带有"双重黑色"的节点
    //       S = x 的兄弟节点（sibling）
    //
    // 四种情况（以 x 是左子为例）：
    //
    //   【情况 1】兄弟 S 是红色
    //      → P 染红，S 染黑，左旋 P，更新 S
    //      → 转化为情况 2/3/4
    //
    //   【情况 2】兄弟 S 是黑色，且 S 的两个子节点都是黑色
    //      → S 染红，x 跳到 P
    //      → 如果 P 是红色，下轮 P 变黑即可结束
    //
    //   【情况 3】兄弟 S 是黑色，且 S 的 近侧 子节点是红色，远侧 是黑色
    //      → S 的近侧红子染黑，S 染红，对 S 旋转
    //      → 转化为情况 4
    //
    //   【情况 4】兄弟 S 是黑色，且 S 的 远侧 子节点是红色
    //      → S 染成 P 的颜色，P 染黑，S 的远侧红子染黑
    //      → 左旋 P → 结束！
    // ----------------------------------------------------------
    void deleteFixup(RBNode* x)
    {
        // x 携带双重黑色，只要 x 不是红+黑（即单纯的红色或到达根）
        while (x != root && x->color == Color::BLACK)
        {
            if (x == x->parent->left)
            {
                // ---- x 是左子 ----
                RBNode* S = x->parent->right;  // 兄弟

                // 情况 1：兄弟是红色
                if (S->color == Color::RED)
                {
                    S->color = Color::BLACK;
                    x->parent->color = Color::RED;
                    leftRotate(x->parent);
                    S = x->parent->right;  // 新的兄弟（现在是黑色）
                }
                // 现在 S 一定是黑色

                // 情况 2：兄弟的两个子节点都是黑色
                if (S->left->color == Color::BLACK && S->right->color == Color::BLACK)
                {
                    S->color = Color::RED;
                    x = x->parent;  // 双重黑色上移至父节点
                }
                else
                {
                    // 情况 3：兄弟的 左子（近侧）红，右子（远侧）黑
                    if (S->right->color == Color::BLACK)
                    {
                        S->left->color = Color::BLACK;
                        S->color = Color::RED;
                        rightRotate(S);
                        S = x->parent->right;  // 新兄弟
                    }
                    // 情况 4：兄弟的 右子（远侧）是红色
                    S->color = x->parent->color;
                    x->parent->color = Color::BLACK;
                    S->right->color = Color::BLACK;
                    leftRotate(x->parent);
                    x = root;  // 修正完毕，退出循环
                }
            }
            else
            {
                // ---- x 是右子（镜像对称）----
                RBNode* S = x->parent->left;

                if (S->color == Color::RED)
                {
                    S->color = Color::BLACK;
                    x->parent->color = Color::RED;
                    rightRotate(x->parent);
                    S = x->parent->left;
                }

                if (S->left->color == Color::BLACK && S->right->color == Color::BLACK)
                {
                    S->color = Color::RED;
                    x = x->parent;
                }
                else
                {
                    if (S->left->color == Color::BLACK)
                    {
                        S->right->color = Color::BLACK;
                        S->color = Color::RED;
                        leftRotate(S);
                        S = x->parent->left;
                    }

                    S->color = x->parent->color;
                    x->parent->color = Color::BLACK;
                    S->left->color = Color::BLACK;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }
        x->color = Color::BLACK;  // 确保根为黑或消除红+黑
    }

    // ----------------------------------------------------------
    // BST 删除 + 修正
    //
    // 删除的三种 BST 情况：
    //   ① 无子节点 → 直接删除
    //   ② 一个子节点 → 用该子节点替代
    //   ③ 两个子节点 → 用后继节点替代（后继的值复制到当前节点，实际删除后继）
    //
    // 红黑树删除的关注点：
    //   - 如果实际被删除的节点是黑色 → 需要 deleteFixup
    //   - 如果被删除的是红色 → 不影响黑高，无需修正
    // ----------------------------------------------------------
    void removeNode(RBNode* z)
    {
        RBNode* y = z;          // y 指向实际上被删除（或移走）的节点
        RBNode* x;              // x 指向替代 y 位置的节点
        Color yOriginalColor = y->color;

        if (z->left == NIL)
        {
            // 情况 ①+②：左子为空（右子可能为 NIL 或有效节点）
            x = z->right;
            transplant(z, z->right);
        }
        else if (z->right == NIL)
        {
            // 情况 ②：右子为空
            x = z->left;
            transplant(z, z->left);
        }
        else
        {
            // 情况 ③：两个子节点都存在
            y = findMin(z->right);         // 找后继（右子树最小）
            yOriginalColor = y->color;
            x = y->right;

            if (y->parent == z)
            {
                // 后继恰好是 z 的直接右子
                x->parent = y;  // x 可能是 NIL，也需要设置 parent
            }
            else
            {
                // 后继不是 z 的直接右子：先把后继从原位置摘除
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            transplant(z, y);             // 用后继替换 z
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;          // 继承 z 的颜色
        }

        delete z;

        // 只有被移除的节点是黑色时才需要修正
        // 因为移走黑色节点会破坏性质 5（黑高不同）
        if (yOriginalColor == Color::BLACK)
        {
            deleteFixup(x);
        }
    }

    // ----------------------------------------------------------
    // 递归查找
    // ----------------------------------------------------------
    RBNode* searchHelper(RBNode* node, int val) const
    {
        if (node == NIL || node->data == val)
        {
            return (node == NIL) ? nullptr : node;
        }
        if (val < node->data)
        {
            return searchHelper(node->left, val);
        }
        return searchHelper(node->right, val);
    }

    // ----------------------------------------------------------
    // 递归销毁所有节点，释放内存
    // ----------------------------------------------------------
    void destroyTree(RBNode* node)
    {
        if (node == NIL || node == nullptr)
        {
            return;
        }
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    // ----------------------------------------------------------
    // 打印树结构（用于观察红黑树的形态和颜色）
    // ----------------------------------------------------------
    void printHelper(RBNode* node, const string& prefix, bool isLeft) const
    {
        if (node == NIL) return;

        cout << prefix;
        cout << (isLeft ? "├──" : "└──");
        cout << node->data;
        cout << (node->color == Color::RED ? "(R)" : "(B)") << endl;

        string childPrefix = prefix + (isLeft ? "│  " : "   ");
        if (node->left != NIL || node->right != NIL)
        {
            printHelper(node->left, childPrefix, true);
            printHelper(node->right, childPrefix, false);
        }
    }

    // ----------------------------------------------------------
    // 中序遍历（用于验证有序性）
    // ----------------------------------------------------------
    void inorderHelper(RBNode* node) const
    {
        if (node == NIL) return;
        inorderHelper(node->left);
        cout << node->data << " ";
        inorderHelper(node->right);
    }

    // ----------------------------------------------------------
    // 检查红黑树性质（调试用）
    // ----------------------------------------------------------
    int checkBlackHeight(RBNode* node) const
    {
        if (node == NIL) return 1;  // NIL 是黑色，贡献 1 个黑高

        // 性质 4：红色节点不能有红色子节点
        if (node->color == Color::RED)
        {
            if (node->left->color == Color::RED || node->right->color == Color::RED)
            {
                cout << "[错误] 性质 4 违反：节点 " << node->data
                     << " 是红色但存在红色子节点！" << endl;
            }
        }

        int leftBH = checkBlackHeight(node->left);
        int rightBH = checkBlackHeight(node->right);

        // 性质 5：左右子树黑高必须相等
        if (leftBH != rightBH)
        {
            cout << "[错误] 性质 5 违反：节点 " << node->data
                 << " 左右子树黑高不等（左=" << leftBH << ", 右=" << rightBH << "）！" << endl;
        }

        // 当前节点是黑色就 +1，红色 +0
        int currentBH = leftBH + (node->color == Color::BLACK ? 1 : 0);
        return currentBH;
    }

public:
    // ==================== 构造函数与析构函数 ====================

    RedBlackTree()
    {
        initNIL();
        root = NIL;
    }

    ~RedBlackTree()
    {
        destroyTree(root);
        delete NIL;
    }

    // ==================== 公有接口 ====================

    void insert(int val)
    {
        RBNode* newNode = nullptr;
        root = insertBST(root, val, newNode);

        // 只有真正插入了新节点（而非重复值）时才需要修正
        if (newNode != nullptr && newNode != NIL && newNode->color == Color::RED)
        {
            insertFixup(newNode);
        }
    }

    void remove(int val)
    {
        RBNode* z = searchHelper(root, val);
        if (z == nullptr)
        {
            cout << val << " 不在树中" << endl;
            return;
        }
        removeNode(z);
    }

    bool search(int val) const
    {
        return searchHelper(root, val) != nullptr;
    }

    void print() const
    {
        cout << "\n========== 红黑树结构 ==========" << endl;
        printHelper(root, "", false);
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
        if (root == NIL) return;
        cout << "\n========== 层序遍历（BFS）==========" << endl;
        queue<RBNode*> q;
        q.push(root);
        int level = 0;
        while (!q.empty())
        {
            int size = q.size();
            cout << "第 " << level << " 层：";
            for (int i = 0; i < size; i++)
            {
                RBNode* cur = q.front(); q.pop();
                cout << cur->data << (cur->color == Color::RED ? "(R)" : "(B)") << "  ";
                if (cur->left != NIL)  q.push(cur->left);
                if (cur->right != NIL) q.push(cur->right);
            }
            cout << endl;
            level++;
        }
        cout << "================================" << endl;
    }

    bool validate() const
    {
        if (root == NIL) return true;

        // 性质 2：根必须是黑色
        if (root->color != Color::BLACK)
        {
            cout << "[错误] 性质 2 违反：根节点不是黑色！" << endl;
            return false;
        }

        checkBlackHeight(root);
        return true;
    }
};

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    RedBlackTree rbt;

    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║     红黑树（Red-Black Tree）测试  ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    // ---------- 测试插入 ----------
    cout << "\n>>> 测试 1：插入操作" << endl;
    cout << "插入序列：10, 20, 30, 15, 25, 5, 35, 40, 12, 18" << endl;

    int testData[] = {10, 20, 30, 15, 25, 5, 35, 40, 12, 18};
    for (int val : testData)
    {
        cout << "插入 " << val << " ..." << endl;
        rbt.insert(val);
    }

    rbt.print();
    rbt.BFS();
    rbt.inorder();

    // 验证红黑树性质
    cout << "\n>>> 性质验证：" << endl;
    rbt.validate();

    // ---------- 测试查找 ----------
    cout << "\n>>> 测试 2：查找操作" << endl;
    cout << "查找 15：" << (rbt.search(15) ? "找到" : "未找到") << endl;
    cout << "查找 100：" << (rbt.search(100) ? "找到" : "未找到") << endl;

    // ---------- 测试删除 ----------
    cout << "\n>>> 测试 3：删除操作" << endl;
    cout << "删除 5（叶节点）..." << endl;
    rbt.remove(5);
    rbt.print();

    cout << "\n删除 30（有一个子节点）..." << endl;
    rbt.remove(30);
    rbt.print();

    cout << "\n删除 15（有两个子节点）..." << endl;
    rbt.remove(15);
    rbt.print();

    cout << "\n删除 100（不存在）..." << endl;
    rbt.remove(100);

    // 最终状态
    cout << "\n>>> 最终红黑树：" << endl;
    rbt.print();
    rbt.BFS();
    rbt.inorder();
    rbt.validate();

    // ---------- 边界测试 ----------
    cout << "\n>>> 测试 4：连续递增插入（测试平衡性）" << endl;
    RedBlackTree rbt2;
    for (int i = 1; i <= 10; i++)
    {
        rbt2.insert(i);
    }
    rbt2.print();
    rbt2.BFS();
    cout << "中序遍历（应为 1~10 有序）：" << endl;
    rbt2.inorder();
    rbt2.validate();

    cout << "\n红黑树所有测试完成！" << endl;
    return 0;
}
