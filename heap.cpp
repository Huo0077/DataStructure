// ============================================================
//     堆合集：最小堆（数组堆）+ 左式堆（可合并堆）
// ============================================================
//
// 本文件涵盖：
//   Part 1: 最小堆（Min Heap）—— 数组实现 + Top-K 最大应用
//   Part 2: 左式堆（Leftist Heap）—— 可合并的优先队列
//
// ============================================================

#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

// ============================================================
//   Part 1: 最小堆（Min Heap）—— 基于动态数组
// ============================================================
//
// 【核心概念】
//   堆是一棵完全二叉树，满足堆序性质：
//     最小堆：每个节点值 ≤ 子节点值（根最小）
//     最大堆：每个节点值 ≥ 子节点值（根最大）
//
// 【数组存储完全二叉树】
//   节点 i（从0始）：父=(i-1)/2, 左子=2i+1, 右子=2i+2
//
//   push O(log n)：放到末尾 → 上浮（与父节点比较，小于则交换）
//   pop  O(log n)：取出堆顶，末尾移到堆顶 → 下沉（与较小子节点比较）
//   top  O(1)：直接返回 heap[0]
//
// 【Top-K 最大 —— 用最小堆做门槛过滤器】
//   维护大小为 k 的最小堆：
//     堆未满 → 直接入堆
//     堆满了，当前元素 > 堆顶（门槛）→ 踢掉堆顶，新元素入堆
//     否则跳过
//   堆顶始终是"当前 Top-K 中最小的那个"（门槛）。
//
//   为什么用最小堆而不是最大堆？
//     最大堆顶是最大值，无法判断该淘汰谁。
//     最小堆顶是最小值（门槛），比门槛小的直接跳过。
//
//   O(n log k) vs 完全排序 O(n log n)
//
//   示例 arr=[3,1,5,7,2,8], k=3:
//     遍历3: [3] → 1: [1,3] → 5: [1,3,5](满)
//     遍历7: 7>1 → [3,5,7]
//     遍历2: 2<3 → 跳过
//     遍历8: 8>3 → [5,7,8]
//     弹出反转 → [8,7,5]
// ============================================================

class MinHeap
{
private:
    vector<int> heap;

    int parent(int i)      { return (i - 1) / 2; }
    int leftChild(int i)   { return 2 * i + 1; }
    int rightChild(int i)  { return 2 * i + 2; }

    // ----------------------------------------------------------
    // UP(index)：上浮操作 —— 把新插入的元素浮到正确位置
    //
    // 目的：恢复堆序性质。当新元素被 push 到末尾后，可能与父节点违反堆序，
    //       需要不断上浮（与父节点比较并交换），直到满足堆序或到根。
    //
    // 操作思路：
    //   while 当前节点不是根 且 当前值 < 父节点值：
    //     交换当前节点与父节点
    //     继续从父节点的位置向上检查
    //
    // 每次 push 后执行一次 UP，时间复杂度 O(log n)（树高）。
    //
    // 示例 push(2) 到已有堆 [1, 3, 5, 8, 7]：
    //   2 放入末尾 → [1, 3, 5, 8, 7, 2]
    //   2 < 父节点 5 → 交换 → [1, 3, 2, 8, 7, 5]
    //   2 < 父节点 3 → 交换 → [1, 2, 3, 8, 7, 5]
    //   2 > 父节点 1 → 停止 ✓
    // ----------------------------------------------------------
    void UP(int index)
    {
        while (index > 0 && heap[index] < heap[parent(index)])
        {
            swap(heap[index], heap[parent(index)]);
            index = parent(index);
        }
    }

    // ----------------------------------------------------------
    // DOWN(index)：下沉操作 —— 把堆顶的"错误"元素沉到正确位置
    //
    // 目的：pop 后堆顶被末尾元素替换，该元素可能比子节点大，违反堆序。
    //       需要不断下沉（与较小子节点比较并交换），直到满足堆序或到叶节点。
    //
    // 操作思路：
    //   while 当前节点有左子节点：
    //     smaller = 左子
    //     如果右子存在且右子更小 → smaller = 右子
    //     如果当前值 ≤ smaller → 堆序满足，停止
    //     否则交换当前节点与 smaller，继续向下
    //
    // 为什么选"较小子节点"交换？
    //   换上去的值必须 ≤ 两个子节点。选较小的那个换上来，
    //   保证换上去的值 ≤ 另一个没动的子节点。
    //
    // 每次 pop 后执行一次 DOWN，时间复杂度 O(log n)。
    //
    // 示例 pop() 用 7 替换堆顶 [1, 3, 5, 8, 7] → [7, 3, 5, 8]：
    //   7 > 左子 3（较小）→ 交换 → [3, 7, 5, 8]
    //   7 > 左子 8？没右子 → 停止 ✓
    // ----------------------------------------------------------
    void DOWN(int index)
    {
        while (leftChild(index) < size())
        {
            int smaller = leftChild(index);
            int r = rightChild(index);
            if (r < size() && heap[r] < heap[smaller])
                smaller = r;                              // 选两子中较小的
            if (heap[index] <= heap[smaller])
                break;                                    // 堆序已满足
            swap(heap[index], heap[smaller]);
            index = smaller;
        }
    }

public:
    // 获取堆顶（最小值），O(1)
    int top()
    {
        if (heap.empty()) { cout << "heap is empty" << endl; return 0; }
        return heap[0];
    }
    bool Isempty() { return heap.empty(); }
    int size()     { return heap.size(); }

    // push：入堆。放到末尾，然后上浮到正确位置。
    void push(int val)
    {
        heap.push_back(val);
        UP(heap.size() - 1);
    }

    // pop：弹出堆顶（最小值）。
    // 用末尾元素覆盖堆顶，弹出末尾，然后下沉恢复堆序。
    int pop()
    {
        if (Isempty()) { cout << "heap is empty" << endl; return 0; }
        int tmp = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if (!Isempty()) DOWN(0);
        return tmp;
    }
};

// ----------------------------------------------------------
// topKLargest(arr, k)：返回 arr 中前 k 大的元素，降序排列
//
// 目的：从海量数据中找出最大的 k 个元素，O(n log k)，
//      远优于全排序的 O(n log n)，特别适合 k << n 的场景。
//
// 操作思路（最小堆做"门槛过滤器"）：
//   维护一个大小为 k 的最小堆，堆中始终保存"当前已看到的 Top K"。
//   对于每个输入元素：
//     - 堆未满（size < k）→ 直接入堆
//     - 堆已满，当前元素 > 堆顶（堆顶 = 当前 Top-K 中最小的那个）
//       → 踢掉最小的（pop），新元素入堆（push）
//     - 当前元素 ≤ 堆顶 → 不是 Top-K，跳过
//
//   最后堆中就是前 k 大的元素（无序），弹出并反转得到降序。
//
// 为什么用最小堆存"最大"的元素？
//   最小堆的堆顶是堆中最小元素，恰好是 Top-K 的"入门门槛"。
//   比门槛小的元素不可能进入 Top-K，直接跳过。
//   如果选最大堆，堆顶是最大元素，无法判断何时淘汰旧元素。
//
// 示例 arr=[3,1,5,7,2,8], k=3:
//   3→[3], 1→[1,3], 5→[1,3,5](满)
//   7>1→pop 1, push 7→[3,5,7]
//   2<3→跳过
//   8>3→pop 3, push 8→[5,7,8]
//   输出反转 → [8,7,5]
// ----------------------------------------------------------
vector<int> topKLargest(vector<int>& arr, int k)
{
    MinHeap heap;
    for (int val : arr)
    {
        if (heap.size() < k)
            heap.push(val);
        else if (val > heap.top())
        {
            heap.pop();
            heap.push(val);
        }
    }
    vector<int> result;
    while (!heap.Isempty())
        result.push_back(heap.pop());
    reverse(result.begin(), result.end());  // 堆是小→大，反转得到大→小
    return result;
}

// ============================================================
//   Part 2: 左式堆（Leftist Heap）—— 可合并的优先队列
// ============================================================
//
// 【核心概念】
//   普通二叉堆无法高效合并两个堆（需 O(n)）。
//   左式堆是"可合并堆"，支持 O(log n) 合并。
//
// 【NPL（零路径长）】
//   npl(x) = 从 x 到最近"没有两个子节点的节点"的最短路径长度
//   nullptr → npl = -1
//   叶节点 → npl = 0
//   公式：npl(x) = npl(x->right) + 1（由左倾性质保证）
//
// 【左倾性质】
//   对于任意节点：npl(left) ≥ npl(right)
//   即左子树零路径长 ≥ 右子树，"右侧更短"。
//   合并时沿右子树递归，保证高效 O(log n)。
//
// 【核心操作：merge】
//   1. 确保 node1 根 ≤ node2 根（否则交换）
//   2. 递归合并 node1->Right 和 node2
//   3. 回溯时若 npl(left) < npl(right)，交换左右子树
//   4. 更新 npl = npl(right) + 1
//
//   push(val) = merge(root, newNode)         —— O(log n)
//   pop()    = merge(root->left, root->right) —— O(log n)
// ============================================================

class LeftNode
{
public:
    int data;
    int npl;
    LeftNode* left;
    LeftNode* right;
    LeftNode(int val) : data(val), npl(0), left(nullptr), right(nullptr) {}
};

class LeftistHeap
{
private:
    LeftNode* root;

    void clear(LeftNode* node)
    {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    // ----------------------------------------------------------
    // merge(n1, n2)：合并两棵左式堆，返回合并后的根
    //
    // 目的：将两个堆的所有元素合并成一个新堆。左式堆的核心操作。
    //       insert 和 deleteMin 都可以用 merge 实现。
    //
    // 自顶向下 + 自底向上的递归：
    //   1. 空堆处理（一边为空直接返回另一边）
    //   2. 确保 n1 的根 ≤ n2 的根（否则交换），n1 成为新根
    //   3. 递归合并 n1->right 和 n2（沿右路径向下）
    //   4. 回溯时检查左倾性质：如果 npl(left) < npl(right)，交换左右子树
    //   5. 更新 npl = npl(right) + 1
    //
    // 为什么总是沿右子树递归？
    //   左倾性质保证右路径是"最短路径"，合并沿最短路径进行 → O(log n)
    //
    // 为什么回溯时需要交换左右？
    //   递归让右子树不断变深。如果不交换，可能违反左倾性质
    //   （左边的 npl 反而比右边小）。交换可以恢复。
    //
    // 具体示例合并 [3] 和 [4]：
    //   n1=3, n2=4: 3≤4, n1 为根
    //   merge(n1->right=null, n2=4) → 返回 4 作为 n1 的右子
    //   回溯：n1 左=null(npl=-1), 右=4(npl=0), -1<0 → 交换左右
    //   结果：3 的左=4, 3 的右=null, npl(3)=0 ✓
    // ----------------------------------------------------------
    LeftNode* merge(LeftNode* n1, LeftNode* n2)
    {
        // 空堆处理
        if (!n1) return n2;
        if (!n2) return n1;

        // 确保 n1 根 ≤ n2 根，n1 作为合并后的根（最小堆性质）
        if (n1->data > n2->data) swap(n1, n2);

        // 递归沿右路径合并（右路径短，保证 log n 深度）
        n1->right = merge(n1->right, n2);

        // 回溯：检查并维护左倾性质
        // 如果左子树的 npl < 右子树的 npl → 交换左右
        int lnpl = n1->left ? n1->left->npl : -1;
        int rnpl = n1->right ? n1->right->npl : -1;
        if (lnpl < rnpl) swap(n1->left, n1->right);

        // 更新 npl：总是由较短的右子树决定
        n1->npl = (n1->right ? n1->right->npl : -1) + 1;
        return n1;
    }

public:
    LeftistHeap() : root(nullptr) {}
    ~LeftistHeap() { clear(root); }

    bool Isempty() { return root == nullptr; }

    // 获取堆顶（最小值），O(1)
    int top()
    {
        if (!root) { cout << "heap is empty" << endl; return 0; }
        return root->data;
    }

    // push(val)：插入新元素。创建单节点堆，与现有堆合并。
    void push(int val)
    {
        root = merge(root, new LeftNode(val));
    }

    // pop()：删除并返回堆顶（最小值）。
    // 删除根节点，将左右子树合并为新堆。
    int pop()
    {
        if (!root) { cout << "heap is empty" << endl; return 0; }
        int Min = root->data;
        LeftNode* old = root;
        root = merge(old->left, old->right);  // 合并左右子树
        delete old;
        return Min;
    }

    // mergeWith(other)：将另一个左式堆全部合并到当前堆
    // 注意：这是破坏性操作，other 的 root 被置空（防止 double free）
    void mergeWith(LeftistHeap& other)
    {
        root = merge(root, other.root);
        other.root = nullptr;
    }
};

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║      堆 综 合 测 试              ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    // ---- Part 1: 最小堆 ----
    cout << "\n┌─ Part 1: 最小堆（数组实现）─────┐" << endl;
    MinHeap mh;
    mh.push(5); mh.push(2); mh.push(8); mh.push(1); mh.push(3);
    cout << "堆顶=" << mh.top() << " (expect 1)" << endl;
    cout << "依次弹出: ";
    while (!mh.Isempty()) cout << mh.pop() << " ";
    cout << "(expect 升序: 1 2 3 5 8)" << endl;

    // Top-K
    cout << "\n┌─ Top-K 最大 ────────────────────┐" << endl;
    vector<int> arr = {3, 1, 5, 7, 2, 8, 10, 4, 6, 9};
    vector<int> top3 = topKLargest(arr, 3);
    cout << "Top-3 最大: ";
    for (int v : top3) cout << v << " ";
    cout << "(expect: 10 9 8)" << endl;

    // ---- Part 2: 左式堆 ----
    cout << "\n┌─ Part 2: 左式堆（可合并堆）─────┐" << endl;
    LeftistHeap lh1, lh2;
    lh1.push(10); lh1.push(30); lh1.push(20);
    lh2.push(5);  lh2.push(15); lh2.push(25);
    cout << "lh1堆顶=" << lh1.top() << " (expect 10)" << endl;
    cout << "lh2堆顶=" << lh2.top() << " (expect 5)" << endl;

    lh1.mergeWith(lh2);
    cout << "合并后堆顶=" << lh1.top() << " (expect 5)" << endl;
    cout << "lh2为空? " << lh2.Isempty() << " (expect 1)" << endl;
    cout << "依次弹出: ";
    while (!lh1.Isempty()) cout << lh1.pop() << " ";
    cout << "(expect 升序: 5 10 15 20 25 30)" << endl;

    cout << "\n所有测试完成！" << endl;
    return 0;
}
