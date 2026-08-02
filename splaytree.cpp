// ============================================================
//      伸展树（Splay Tree）—— 自适应二叉搜索树
// ============================================================
//
// 【什么是伸展树？】
//   伸展树（Splay Tree）由 Sleator 和 Tarjan 于 1985 年发明。
//   它是一种"自适应"（self-adjusting）的二叉搜索树——
//   每次查找、插入或删除一个节点后，通过一系列旋转将该节点
//   "伸展"到根的位置。
//
//   核心思想：最近访问的节点应当最容易再次被访问。
//   （类似 LRU 缓存的思路，但用树的旋转来实现）
//
// 【为什么需要伸展树？】
//   1. 简单：不需要存储平衡因子（AVL）或颜色（红黑树），
//      只需要标准的旋转操作。
//   2. 自适应：如果访问模式有局部性（某些节点被频繁访问），
//      它们会自然"浮"到靠近根的位置，平均访问更快。
//   3. 均摊 O(log n)：虽然单次操作可能 O(n)，但 m 次操作
//      的均摊复杂度为 O(m log n)。
//
// 【伸展树 vs AVL / 红黑树】
//
//   ┌──────────┬────────────┬──────────────┬──────────────┐
//   │          │  伸展树     │  AVL 树       │  红黑树       │
//   ├──────────┼────────────┼──────────────┼──────────────┤
//   │ 平衡维护  │ 伸展操作    │ 平衡因子+旋转  │ 颜色+旋转     │
//   │ 额外存储  │ 无          │ 高度(int)     │ 颜色(bool)    │
//   │ 单次最坏  │ O(n)       │ O(log n)      │ O(log n)     │
//   │ 均摊性能  │ O(log n)   │ O(log n)      │ O(log n)     │
//   │ 访问局部性│ 自动优化    │ 无优化        │ 无优化        │
//   │ 实现难度  │ 中等        │ 中等          │ 较难          │
//   └──────────┴────────────┴──────────────┴──────────────┘
//
// 【伸展操作（Splay）—— 核心】
//
//   伸展操作通过一系列旋转，将目标节点 X 移动到根。
//   每次旋转涉及 X、X 的父节点 P、X 的祖父节点 G。
//   根据 X、P、G 的相对位置，有三种情况：
//
//   情况 1 — Zig（X 的父节点是根）：
//     对 P 做一次单旋转（左旋或右旋），X 成为根。
//
//        P                X
//       / \              / ╲
//      X   C    →       A   P
//     / \                  / ╲
//    A   B                B   C
//
//   情况 2 — Zig-Zig（X 和 P 在同一侧：都是左子 or 都是右子）：
//     先旋转 P（绕 G），再旋转 X（绕 P 的新位置）。
//     ★ 两次旋转同方向——这是"先父后子"（与 AVL 的先子后父不同！）
//
//        G                P                X
//       / \              / \              / ╲
//      P   D    →       X   G     →      A   P
//     / \              / \ / \              / ╲
//    X   C            A  B C  D            B   G
//   / \                                      / ╲
//  A   B                                    C   D
//
//   关键：Zig-Zig 是"先转父，后转子"！！！
//   如果像 AVL 那样"先转子，后转父"，就不是 Zig-Zig 了。
//   Zig-Zig 先转父的原因：将整条链"拉平"得更彻底。
//
//   情况 3 — Zig-Zag（X 和 P 在不同侧：X 是左子但 P 是右子，或反之）：
//     先旋转 X（绕 P），再旋转 X（绕 G）。
//     ★ 两次旋转不同方向——这和 AVL 的 LR/RL 一样（先子后父）。
//
//        G                G                X
//       / \              / \              / ╲
//      P   D    →       X   D     →     P   G
//     / \              / \              / \ / ╲
//    A   X            P   C            A  B C  D
//       / \          / ╲
//      B   C        A   B
//
// 【伸展的三种触发场景】
//
//   1. Search(k)  → 找到 k 所在节点，伸展该节点到根
//   2. Insert(k)  → 先按 BST 插入，然后伸展新节点到根
//   3. Delete(k)  → 先伸展 k 到根，然后删除根，将左子树的最大
//                   节点伸展到根（或右子树的最小节点）
//
// 【伸展树的应用】
//   - 实现缓存（最近使用的最接近根）
//   - 网络流中的动态树（Link-Cut Tree 基于 Splay）
//   - 实现序列/区间操作（Splay Tree 可以像 Treap 一样维护序列）
//   - 编译器优化中的符号表管理
//
// 【手动演算示例 —— 查找 1 的过程（每次查找后伸展）】
//
//   初始树（BST，可能很不平衡）：
//          5
//         / ╲
//        3   7
//       / ╲
//      2   4
//     /
//    1
//
//   查找 1 的过程：
//     Step 1: 从根向下找到 1（路径：5→3→2→1）
//
//     Step 2: 从 1 开始自底向上伸展：
//
//       当前：X=1, P=2, G=3
//       X 是 P 的左子，P 是 G 的左子 → Zig-Zig！
//       先转 P(2) 绕 G(3)：
//            5               5
//           / \             / ╲
//          3   7   →       2   7
//         / \             / ╲
//        2   4           1   3
//       /                     ╲
//      1                       4
//       再转 X(1) 绕 P(2)：
//            5
//           / ╲
//          1   7
//           ╲
//            2
//             ╲
//              3
//               ╲
//                4
//       X 还不是根！继续。
//
//       当前：X=1, P=5（X 是 P 的左子，P 无祖父）→ Zig！
//       对 P(5) 右旋：
//            1
//             ╲
//              5
//             / ╲
//            2   7
//             ╲
//              3
//               ╲
//                4
//       伸展完成！1 成为新的根。
//
//   观察：虽然这棵树现在仍然很不平衡（退化成了一条链！），
//   但最近访问的元素 1 在最顶上。如果接下来反复访问 1，
//   每次都是 O(1) —— 这就是自适应的威力。
//   如果后续访问其他节点，它们也会被伸展到顶部，
//   树会在使用过程中逐渐变得"相对平衡"。
//
// ============================================================

#include<iostream>
#include<queue>
using namespace std;

// 【图示约定与阅读地图】
//   图中 X 是本次伸展目标，P 是父节点，G 是祖父节点；A..D 代表保持 BST 键范围的子树。
//   公开 Search / Insert / Delete 完成后，目标节点或最后访问节点会被伸展到根。
//   Zig-Zig 必须先旋 P 再旋 X；Zig-Zag 则连续旋 X。不要把两种旋转顺序混为一谈。

// ==================== 伸展树节点 ====================

struct SplayNode
{
    int data;
    SplayNode* left;
    SplayNode* right;
    SplayNode* parent;    // 伸展操作需要父指针

    SplayNode(int val)
        : data(val), left(nullptr), right(nullptr), parent(nullptr) {}
};

// ==================== 伸展树类 ====================

class SplayTree
{
private:
    SplayNode* root;

    // ----------------------------------------------------------
    // 右旋转（处理左子→根的情况）
    //
    //     y                x
    //    / \              / ╲
    //   x   C    →       A   y
    //  / \                  / ╲
    // A   B                B   C
    //
    // 旋转涉及 3 个节点（x, y, B）和 4 个父子关系的更新：
    //   ① y->left = B（B 过继给 y）
    //   ② x->parent = y->parent（x 接管 y 在树中的位置）
    //   ③ y->parent = x
    //   ④ x->right = y
    //
    // 顺序不能随意调换：必须先用 ① 暂存 B，再用 ② 保存 y 的父指针。
    // 如果先做 ④，y 的原始 parent 信息会丢失。
    //
    // 与 AVL 旋转的关键区别：需要维护 parent 指针。
    // AVL 的旋转只改 left/right，这里的旋转还要改每个节点的 parent，
    // 因为 splay 需要在旋转后沿 parent 继续向上伸展。
    // ----------------------------------------------------------
    void rightRotate(SplayNode* y)
    {
        SplayNode* x = y->left;
        SplayNode* B = x->right;

        // ① B 过继给 y：x < B < y，B 挂到 y 的左子
        y->left = B;
        if (B != nullptr) B->parent = y;

        // ② x 替代 y 在树中的位置（继承 y 的父指针）
        x->parent = y->parent;
        if (y->parent == nullptr)
        {
            root = x;                  // y 原来是根 → x 成为新根
        }
        else if (y == y->parent->left)
        {
            y->parent->left = x;       // y 是父的左子 → x 接管左子位置
        }
        else
        {
            y->parent->right = x;      // y 是父的右子 → x 接管右子位置
        }

        // ③④ 建立 x→y 的父子关系
        x->right = y;
        y->parent = x;
    }

    // ----------------------------------------------------------
    // 左旋转（处理右子→根的情况）—— 右旋的镜像对称
    //
    //     y                x
    //    / \              / ╲
    //   A   x     →      y   C
    //      / \          / ╲
    //     B   C        A   B
    //
    // 同上，4 步操作（与右旋对称）：
    //   ① y->right = B（B 过继给 y）
    //   ② x->parent = y->parent（x 接管 y 的位置）
    //   ③④ x->left = y; y->parent = x（建立新父子关系）
    // ----------------------------------------------------------
    void leftRotate(SplayNode* y)
    {
        SplayNode* x = y->right;
        SplayNode* B = x->left;

        // ① B 过继给 y：y < B < x，B 挂到 y 的右子
        y->right = B;
        if (B != nullptr) B->parent = y;

        // ② x 替代 y 在树中的位置
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

        // ③④ 建立 x→y 的父子关系
        x->left = y;
        y->parent = x;
    }

    // ----------------------------------------------------------
    // 伸展操作（Splay）—— 将节点 x 旋转到根的位置
    //
    // 三种情况循环执行，直到 x 成为根：
    //
    //   情况 1（Zig）：x 的父节点是根
    //     → 对根做一次单旋转
    //
    //   情况 2（Zig-Zig）：x 和父节点 P 在同一侧
    //     → x 是 P 的左子且 P 是 G 的左子
    //     → 或 x 是 P 的右子且 P 是 G 的右子
    //     → 先旋转 P，再旋转 x（"先父后子"）
    //
    //   情况 3（Zig-Zag）：x 和父节点 P 在不同侧
    //     → x 是 P 的左子但 P 是 G 的右子
    //     → 或 x 是 P 的右子但 P 是 G 的左子
    //     → 先旋转 x，再旋转 x（"先子后父"，和 AVL LR/RL 一样）
    //
    //   关键区别：Zig-Zig 是"先父后子"，Zig-Zag 是"先子后父"
    //   这个顺序差异是保证均摊 O(log n) 的关键！
    // ----------------------------------------------------------
    void splay(SplayNode* x)
    {
        while (x->parent != nullptr)  // 只要 x 不是根就继续转
        {
            SplayNode* P = x->parent;
            SplayNode* G = P->parent;

            if (G == nullptr)
            {
                // ---- 情况 1：Zig（父节点是根）----
                if (x == P->left)
                {
                    rightRotate(P);  // 右旋父节点
                }
                else
                {
                    leftRotate(P);   // 左旋父节点
                }
            }
            else
            {
                if (P == G->left && x == P->left)
                {
                    // ---- 情况 2a：Zig-Zig（左-左）----
                    // 先转父 P 绕 G，再转 x 绕 P
                    rightRotate(G);   // 先转父（P 变成 G 的位置）
                    rightRotate(P);   // 再转 x（x 变成 P 的位置）
                }
                else if (P == G->right && x == P->right)
                {
                    // ---- 情况 2b：Zig-Zig（右-右）----
                    leftRotate(G);
                    leftRotate(P);
                }
                else if (P == G->left && x == P->right)
                {
                    // ---- 情况 3a：Zig-Zag（左-右）----
                    // 先转 x 绕 P，再转 x 绕 G
                    leftRotate(P);    // 先转 x（x 变成 P 的位置）
                    rightRotate(G);   // 再转 x（x 变成 G 的位置）
                }
                else  // P == G->right && x == P->left
                {
                    // ---- 情况 3b：Zig-Zag（右-左）----
                    rightRotate(P);
                    leftRotate(G);
                }
            }
        }
    }

    // ----------------------------------------------------------
    // BST 查找（返回找到的节点，如果不存在返回搜索路径上最后的节点）
    // ----------------------------------------------------------
    SplayNode* bstSearch(SplayNode* node, int key, SplayNode*& lastVisited)
    {
        lastVisited = nullptr;
        while (node != nullptr)
        {
            lastVisited = node;
            if (key < node->data)
            {
                node = node->left;
            }
            else if (key > node->data)
            {
                node = node->right;
            }
            else
            {
                return node;  // 找到了
            }
        }
        return nullptr;  // 没找到，lastVisited 是搜索路径最后节点
    }

    // ----------------------------------------------------------
    // 找子树中值最大的节点
    // ----------------------------------------------------------
    SplayNode* findMax(SplayNode* node)
    {
        if (node == nullptr) return nullptr;
        while (node->right != nullptr)
        {
            node = node->right;
        }
        return node;
    }

    // ----------------------------------------------------------
    // 找子树中值最小的节点
    // ----------------------------------------------------------
    SplayNode* findMin(SplayNode* node)
    {
        if (node == nullptr) return nullptr;
        while (node->left != nullptr)
        {
            node = node->left;
        }
        return node;
    }

    // ----------------------------------------------------------
    // 递归销毁
    // ----------------------------------------------------------
    void destroy(SplayNode* node)
    {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    // ----------------------------------------------------------
    // 递归打印
    // ----------------------------------------------------------
    void printHelper(SplayNode* node, const string& prefix, bool isLeft) const
    {
        if (node == nullptr) return;
        cout << prefix;
        cout << (isLeft ? "├── " : "└── ");
        cout << node->data << endl;
        string newPrefix = prefix + (isLeft ? "│   " : "    ");
        printHelper(node->left, newPrefix, true);
        printHelper(node->right, newPrefix, false);
    }

    // 中序
    void inorderHelper(SplayNode* node) const
    {
        if (node == nullptr) return;
        inorderHelper(node->left);
        cout << node->data << " ";
        inorderHelper(node->right);
    }

public:
    SplayTree() : root(nullptr) {}
    ~SplayTree() { destroy(root); }

    // ==================== 查找（含伸展）====================

    bool search(int key)
    {
        SplayNode* lastVisited = nullptr;
        SplayNode* found = bstSearch(root, key, lastVisited);

        if (found != nullptr)
        {
            // 找到了：将该节点伸展到根
            splay(found);
            root = found;
            return true;
        }
        else if (lastVisited != nullptr)
        {
            // 没找到：将搜索路径上的最后一个节点伸展到根
            splay(lastVisited);
            root = lastVisited;
        }
        return false;
    }

    // ==================== 插入（含伸展）====================

    void insert(int key)
    {
        if (root == nullptr)
        {
            root = new SplayNode(key);
            return;
        }

        // 第 1 步：按 BST 规则找到插入位置的父节点
        SplayNode* curr = root;
        SplayNode* parent = nullptr;
        while (curr != nullptr)
        {
            parent = curr;
            if (key < curr->data)
            {
                curr = curr->left;
            }
            else if (key > curr->data)
            {
                curr = curr->right;
            }
            else
            {
                // 重复值：将已存在的节点伸展到根
                splay(curr);
                root = curr;
                return;
            }
        }

        // 第 2 步：插入新节点
        SplayNode* newNode = new SplayNode(key);
        newNode->parent = parent;
        if (key < parent->data)
        {
            parent->left = newNode;
        }
        else
        {
            parent->right = newNode;
        }

        // 第 3 步：将新插入的节点伸展到根
        splay(newNode);
        root = newNode;
    }

    // ==================== 删除（含伸展）====================

    void remove(int key)
    {
        if (root == nullptr) return;

        // 第 1 步：将要删除的节点伸展到根
        if (!search(key))
        {
            return;  // 不存在，search 已经将最近的节点伸展了
        }

        // 第 2 步：现在要删除的节点在根上
        SplayNode* target = root;
        if (root->left == nullptr)
        {
            // 没有左子树：右子成为新根
            root = root->right;
            if (root != nullptr) root->parent = nullptr;
        }
        else if (root->right == nullptr)
        {
            // 没有右子树：左子成为新根
            root = root->left;
            if (root != nullptr) root->parent = nullptr;
        }
        else
        {
            // 左右子树都有：
            // 将左子树的最大节点伸展到左子树的根，
            // 然后把右子树挂到新根下面
            SplayNode* leftSubtree = root->left;
            leftSubtree->parent = nullptr;  // 暂时断开

            SplayNode* maxLeft = findMax(leftSubtree);
            splay(maxLeft);                 // 最大节点变成左子树的根
            // 现在 maxLeft 没有右子（因为它是最大的）

            maxLeft->right = root->right;   // 挂上右子树
            root->right->parent = maxLeft;

            root = maxLeft;
            root->parent = nullptr;
        }
        delete target;
    }

    // ==================== 遍历与打印 ====================

    void print() const
    {
        cout << "\n========== 伸展树结构 ==========" << endl;
        if (root == nullptr)
        {
            cout << "(空树)" << endl;
        }
        else
        {
            printHelper(root, "", false);
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
        queue<SplayNode*> q;
        q.push(root);
        int level = 0;
        while (!q.empty())
        {
            int sz = q.size();
            cout << "第 " << level << " 层：";
            for (int i = 0; i < sz; i++)
            {
                SplayNode* node = q.front(); q.pop();
                cout << node->data << "  ";
                if (node->left) q.push(node->left);
                if (node->right) q.push(node->right);
            }
            cout << endl;
            level++;
        }
        cout << "==================================" << endl;
    }

    int getRoot() const { return root ? root->data : -1; }
};

// ============================================================
//                   手动演示伸展过程
// ============================================================

void demoZigZig()
{
    cout << "\n┌── 手动演示：Zig-Zig（左-左）伸展过程 ────────┐" << endl;

    // 下图仅用于标记 G、P、X 三个旋转角色。实际演示始终通过 SplayTree
    // 的公开接口构造节点，避免创建游离节点和手动维护其生命周期。

    cout << "构造一棵示例树：" << endl;
    cout << "        3 (G)" << endl;
    cout << "       / \\" << endl;
    cout << " (P) 2   4 (D)" << endl;
    cout << "     /" << endl;
    cout << "(X)1" << endl;

    SplayTree st;
    // 用公有接口插入这些值来演示
    cout << "\n使用 SplayTree 插入 3, 2, 4, 1（每次插入后伸展到根）：" << endl;
    st.insert(3);  st.print();
    st.insert(2);  st.print();
    st.insert(4);  st.print();
    st.insert(1);  st.print();
    cout << "观察：插入 1 后，1 被伸展到根的位置！" << endl;
    st.BFS();
}

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║  伸展树（Splay Tree）测试          ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    // ==================== 测试 1：构建与插入 ====================
    cout << "\n>>> 测试 1：插入操作（每次插入后伸展到根）" << endl;
    SplayTree tree;
    int data[] = {10, 20, 30, 15, 25, 5, 35};

    for (int val : data)
    {
        cout << "\n--- 插入 " << val << " ---" << endl;
        tree.insert(val);
        tree.print();
        cout << "根节点 = " << tree.getRoot() << "（新插入的 " << val
             << " 被伸展到根）" << endl;
    }
    tree.inorder();

    // ==================== 测试 2：查找（含伸展）====================
    cout << "\n>>> 测试 2：查找操作（含伸展）" << endl;
    cout << "查找 5 ..." << endl;
    bool found5 = tree.search(5);
    cout << "结果：" << (found5 ? "找到" : "未找到")
         << "，根变为 " << tree.getRoot() << endl;
    tree.print();

    cout << "查找 15 ..." << endl;
    bool found15 = tree.search(15);
    cout << "结果：" << (found15 ? "找到" : "未找到")
         << "，根变为 " << tree.getRoot() << endl;
    tree.print();

    cout << "查找 100（不存在）..." << endl;
    bool found100 = tree.search(100);
    cout << "结果：" << (found100 ? "找到" : "未找到")
         << "，根变为 " << tree.getRoot() << "（最近搜索路径上的节点被伸展）" << endl;
    tree.print();

    // ==================== 测试 3：删除（含伸展）====================
    cout << "\n>>> 测试 3：删除操作（含伸展）" << endl;
    cout << "删除 20 ..." << endl;
    tree.remove(20);
    tree.print();
    tree.inorder();

    cout << "删除 10（当前根）..." << endl;
    tree.remove(10);
    tree.print();
    tree.inorder();

    // ==================== 测试 4：自适应特性演示 ====================
    cout << "\n>>> 测试 4：自适应特性演示" << endl;
    SplayTree tree2;
    for (int i = 1; i <= 7; i++)
    {
        tree2.insert(i);
    }
    cout << "顺序插入 1~7 后的树（倾向于退化为链）：" << endl;
    tree2.print();
    tree2.BFS();

    cout << "\n连续 3 次查找 1：" << endl;
    for (int i = 0; i < 3; i++)
    {
        tree2.search(1);
        tree2.print();
    }
    cout << "→ 1 始终在根附近！这就是自适应的效果。" << endl;

    // ==================== 测试 5：Zig-Zig/Zig-Zag 演示 ====================
    cout << "\n>>> 测试 5：手动构造并观察伸展" << endl;
    SplayTree tree3;

    // 插入一串数据，每次插入后都会伸展
    cout << "\n插入: 8, 3, 10, 1, 6, 14, 4, 7, 13" << endl;
    int data2[] = {8, 3, 10, 1, 6, 14, 4, 7, 13};
    for (int val : data2)
    {
        tree3.insert(val);
    }
    tree3.print();
    tree3.BFS();
    tree3.inorder();

    cout << "\n查找 7（触发可能的 Zig-Zig 或 Zig-Zag）:" << endl;
    tree3.search(7);
    tree3.print();

    cout << "\n查找 13:" << endl;
    tree3.search(13);
    tree3.print();

    cout << "\n伸展树所有测试完成！" << endl;
    return 0;
}
