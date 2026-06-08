// ============================================================
//              最小堆（Min Heap）—— 基于动态数组
// ============================================================
//
// 【核心概念】
//   堆是一棵完全二叉树，满足"堆序性质"：
//     最小堆：每个节点的值 ≤ 其子节点的值（根最小）
//     最大堆：每个节点的值 ≥ 其子节点的值（根最大）
//
//   本实现是最小堆，即堆顶（根）始终是整棵树的最小值。
//
// 【数组存储完全二叉树】
//   完全二叉树可以用数组紧凑存储，不需要指针：
//
//     索引 i 的节点（从 0 开始）：
//       父节点索引  = (i - 1) / 2
//       左子节点索引 = 2*i + 1
//       右子节点索引 = 2*i + 2
//
//   图示（数组索引）：
//            [0]
//          /     \
//        [1]     [2]
//       /  \     /  \
//     [3]  [4] [5]  [6]
//
//   parent(3) = (3-1)/2 = 1  ✓
//   leftChild(1) = 2*1+1 = 3  ✓
//   rightChild(1) = 2*1+2 = 4 ✓
//
// 【两个核心操作】
//
//   1. push（上浮 / "上滤"）:
//      将新元素放到数组末尾，然后与父节点比较：
//        如果比父节点小 → 交换 → 继续上浮
//        直到满足堆序 或 到达根
//
//      push(5) 的过程（已有 [3,7,8,10]）:
//        [3,7,8,10,5]    # 放到末尾，索引 4
//        5 < parent(4)=7  → 交换
//        [3,5,8,10,7]    # 5 上浮到索引 1
//        5 > parent(1)=3  → 停止
//
//   2. pop（下沉 / "下滤"）:
//      取出堆顶（最小值），将数组最后一个元素移到堆顶，
//      然后与较小的子节点比较：
//        如果比子节点大 → 交换 → 继续下沉
//        直到满足堆序 或 到达叶节点
//
//      pop() 的过程（堆 [3,5,8,10,7]）:
//        取出 3，把末尾 7 放到堆顶 → [7,5,8,10]
//        7 > 较小的子节点 5 → 交换
//        [5,7,8,10]        # 7 下沉到索引 1
//        7 < 唯一的子节点 10 → 停止
//
// 【时间复杂度】
//   push: O(log n) —— 最多上浮到根（树高）
//   pop:  O(log n) —— 最多下沉到叶
//   top:  O(1)     —— 直接返回 heap[0]
//
// 【堆 vs 排序】
//   堆顶只保证最小/最大，不保证整体有序。
//   用堆可以实现 O(n log n) 的堆排序。
//
// ============================================================
//
//                     Top-K 最大 —— 用最小堆
//
// 【问题】从 n 个元素的数组中找出前 k 大的数，降序排列。
//
// 【思路】维护一个大小为 k 的"最小堆"作为门槛过滤器：
//
//   1. 遍历数组，维护大小为 k 的最小堆
//   2. 堆未满 k 个 → 直接入堆
//   3. 堆已满 k 个：
//        当前元素 > 堆顶（门槛）？
//          是 → 堆顶太小了，踢掉它，新元素入堆
//          否 → 当前元素不够大，跳过
//   4. 遍历完成后，堆中就是"前 k 大"的数
//   5. 弹出并反转得到降序结果
//
//   堆顶始终是"当前 top-K 中最小的那个"（门槛值）。
//   任何比门槛小的元素都进不了堆。
//
// 【为什么用最小堆而不是最大堆？】
//   最大堆的堆顶是最大值，我们无法判断"哪个元素该淘汰"。
//   最小堆的堆顶是最小值（门槛），比门槛小的直接跳过即可。
//
// 【时间复杂度】
//   遍历 n 个元素，每次最大 O(log k) 的堆操作：
//     O(n log k)，当 k << n 时远优于 O(n log n) 的完全排序。
//
// 【示例】arr = [3,1,5,7,2,8], k = 3
//   遍历 3: 堆 [3]
//   遍历 1: 堆 [1,3]
//   遍历 5: 堆 [1,3,5]  （满）
//   遍历 7: 7>堆顶1 → pop(1), push(7) → 堆 [3,5,7]
//   遍历 2: 2<堆顶3 → 跳过
//   遍历 8: 8>堆顶3 → pop(3), push(8) → 堆 [5,7,8]
//   弹出: 5,7,8 → 反转 → [8,7,5] ✓
//
// ============================================================

#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;

class MinHeap
{
private:
    vector<int> heap;

    int parent(int i)      { return (i - 1) / 2; }
    int leftChild(int i)   { return 2 * i + 1; }
    int rightChild(int i)  { return 2 * i + 2; }

    // 上浮：新插入元素从底部向上调整到正确位置
    void UP(int index)
    {
        while (index > 0 && heap[index] < heap[parent(index)])
        {
            int Parentid = parent(index);
            swap(heap[index], heap[Parentid]);
            index = Parentid;
        }
    }

    // 下沉：堆顶元素从顶部向下调整到正确位置
    // 每次与较小的子节点比较，如果比子节点大则交换下沉
    void DOWN(int index)
    {
        while (leftChild(index) < size())
        {
            int smallerChildIndex = leftChild(index);
            int rightChildIndex = rightChild(index);
            // 如果右子存在且更小，选右子
            if (rightChildIndex < size() && heap[rightChildIndex] < heap[smallerChildIndex])
            {
                smallerChildIndex = rightChildIndex;
            }
            // 如果当前节点 ≤ 最小的子节点，已满足堆序
            if (heap[index] <= heap[smallerChildIndex])
            {
                break;
            }
            swap(heap[index], heap[smallerChildIndex]);
            index = smallerChildIndex;
        }
    }

public:
    MinHeap() {}

    // 查看堆顶（最小值）
    int top()
    {
        if (heap.empty())
        {
            cout << "heap is empty" << endl;
            return 0;
        }
        return heap[0];
    }

    bool Isempty()
    {
        return heap.empty();
    }

    int size()
    {
        return heap.size();
    }

    // 插入：放到末尾，然后上浮
    void push(int val)
    {
        heap.push_back(val);
        int current = heap.size() - 1;
        UP(current);
    }

    // 删除堆顶：用最后一个元素取代堆顶，然后下沉
    int pop()
    {
        if (Isempty())
        {
            cout << "heap is empty" << endl;
            return 0;
        }
        int temp = heap[0];         // 保存堆顶
        heap[0] = heap.back();     // 末尾元素移到堆顶
        heap.pop_back();           // 删除末尾
        if (!Isempty())
        {
            DOWN(0);               // 下沉调整
        }
        return temp;
    }
};

vector<int> topKLargest(vector<int>& arr, int k)
{
    MinHeap heap;
    for (int i = 0; i < arr.size(); i++)
    {
        if (heap.size() < k)
        {
            heap.push(arr[i]);  // 堆未满，直接入
        }
        else if (arr[i] > heap.top())
        {
            heap.pop();         // 踢掉门槛（最小值）
            heap.push(arr[i]);  // 加入更大的元素
        }
    }
    // 弹出堆中元素（升序），反转得降序
    vector<int> result;
    while (!heap.Isempty())
    {
        result.push_back(heap.pop());
    }
    reverse(result.begin(), result.end());
    return result;
}
