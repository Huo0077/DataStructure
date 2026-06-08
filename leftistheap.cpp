// ============================================================
//              左式堆（Leftist Heap）—— 可合并的优先队列
// ============================================================
//
// 【核心概念】
//   普通二叉堆（heap.cpp）无法高效合并两个堆——合并需要 O(n) 时间。
//   左式堆是一种"可合并堆"（Mergeable Heap），支持 O(log n) 合并。
//
//   左式堆也是二叉树，满足堆序性质（根最小），但结构上不是完全二叉树，
//   而是通过 "npl" 维护一种左倾性质。
//
// 【NPL（Null Path Length，零路径长）】
//   npl(x) = 从节点 x 到最近的"没有两个子节点的节点"的最短路径长度
//
//   - nullptr 的 npl 视为 -1
//   - 叶节点（无子节点）的 npl = 0
//   - 只有一个子节点的节点 npl = 0（因为有一个方向是 nullptr）
//   - npl = min(npl(left), npl(right)) + 1
//
//   简化公式：npl(x) = npl(x->right) + 1（见下文"左倾性质"）
//
// 【左倾性质（Leftist Property）】
//   对于任意节点 x：npl(x->left) ≥ npl(x->right)
//
//   翻译：左子树的零路径长总是 ≥ 右子树的。
//   这意味着"右子树更短"——合并时优先沿右子树递归，保证高效。
//
//   因为 npl(left) ≥ npl(right)，所以：
//     npl(node) = npl(node->right) + 1
//   （npl 由较短的右子树决定）
//
// 【核心操作：merge（合并两棵左式堆）】
//   1. 确保 node1 的根 ≤ node2 的根（否则交换），node1 作为新根
//   2. 递归合并 node1->Right 和 node2
//   3. 如果左子树的 npl < 右子树的 npl → 交换左右子树（维护左倾性质）
//   4. 更新 node1 的 npl
//
//   这是一个自顶向下 + 自底向上的过程：
//     向下：沿右子树递归，找到合并点
//     向上：回溯时检查左倾性质，必要时交换左右子树
//
//   push(val) = merge(root, new Node(val))      —— O(log n)
//   pop()    = merge(root->left, root->right)   —— O(log n)
//
// 【时间复杂度】
//   merge/push/pop: O(log n) —— merge 沿右子树递归，右子树高度 O(log n)
//
// 【为什么叫"左式"堆？】
//   左倾性质使得右路径始终保持较短，合并沿右路径进行，
//   因此效率为 O(log n)。
//
// ============================================================

#include<iostream>
#include<algorithm>
using namespace std;

class Node
{
public:
    int data;
    int npl;       // 零路径长：到最近"非满节点"的最短距离
    Node* Left;
    Node* Right;
    Node(int val) : data(val), npl(0), Left(nullptr), Right(nullptr) {}
    // 新节点总是叶节点，npl = 0
};

class LeftistHeap
{
private:
    Node* root;

    // 递归释放所有节点（后序遍历）
    void clear(Node* node)
    {
        if (node == nullptr)
        {
            return;
        }
        clear(node->Left);
        clear(node->Right);
        delete node;
    }

    // ============================================================
    // 合并两棵左式堆，返回合并后的根
    //
    // 步骤：
    //   1. 空堆处理
    //   2. 确保 node1->data ≤ node2->data（node1 作为新根）
    //   3. 递归合并 node1->Right 和 node2
    //   4. 检查左倾性质：如果 Left->npl < Right->npl，交换左右子
    //   5. 更新 npl = Right->npl + 1
    //
    // 示例（合并两个左式堆）：
    //   node1: [3, npl=1]         node2: [4, npl=0]
    //          /    \                    /
    //      [10]    [8]               [12]
    //
    //   3 ≤ 4，node1 为根
    //   merge(node1->Right=[8], node2=[4, 12]) → 4 为根
    //     merge([8], [12]) → 8 为根, [12] 为右子
    //     Left->npl=0 ≥ Right->npl=0，不交换
    //   node1 检查：Left->npl=0 < Right->npl=1 → 交换左右！
    //   （现在 Left 变短了，需要换到右边去...不对，需要确保左边更长）
    //
    //   实际 left->npl < right->npl 时交换，使得左边 npl ≥ 右边 npl。✓
    // ============================================================
    Node* merge(Node* node1, Node* node2)
    {
        // 空堆处理
        if (node1 == nullptr) { return node2; }
        if (node2 == nullptr) { return node1; }

        // 确保 node1 的根更小（最小堆性质）
        if (node1->data > node2->data)
        {
            swap(node1, node2);
        }

        // 递归合并 node1 的右子树和 node2
        node1->Right = merge(node1->Right, node2);

        // 检查并维护左倾性质：左子树的 npl 必须 ≥ 右子树的 npl
        if (node1->Left == nullptr || node1->Left->npl < node1->Right->npl)
        {
            swap(node1->Left, node1->Right);  // 交换左右子树
        }

        // 更新 npl：如果右子为空 npl=0，否则 npl = Right->npl + 1
        if (node1->Right == nullptr)
        {
            node1->npl = 0;
        }
        else
        {
            node1->npl = node1->Right->npl + 1;
        }
        return node1;
    }

public:
    LeftistHeap() : root(nullptr) {}

    ~LeftistHeap()
    {
        clear(root);
    }

    bool Isempty()
    {
        return root == nullptr;
    }

    // 查看堆顶（最小值），O(1)
    int top()
    {
        if (root == nullptr)
        {
            cout << "heap is empty" << endl;
            return 0;
        }
        return root->data;
    }

    // 插入 = 将新节点（单节点堆）与当前堆合并
    void push(int val)
    {
        Node* newnode = new Node(val);
        root = merge(root, newnode);
    }

    // 删除堆顶 = 将左右子树合并（绕开根节点）
    int pop()
    {
        if (root == nullptr)
        {
            cout << "heap is empty" << endl;
            return 0;
        }
        int Min = root->data;
        Node* oldroot = root;
        root = merge(oldroot->Left, oldroot->Right);  // 合并左右子树
        delete oldroot;
        return Min;
    }
};
