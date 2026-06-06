#include<iostream>
#include<vector>
#include<algorithm>
using namespace std;
class MinHeap
{
    private:
    vector<int> heap;

    int parent(int i)
    {
        return (i - 1) / 2;
    }

    int leftChild(int i)
    {
        return 2 * i + 1;
    }

    int rightChild(int i)
    {
        return 2 * i + 2;
    }

    void UP(int index)
    {
        while(index > 0 && heap[index]<heap[parent(index)])
        {
            int Parentid = parent(index);
            swap(heap[index],heap[Parentid]);
            index = Parentid;
        }
    }

    void DOWN(int index)
    {
        while(leftChild(index) < size())
        {
            int smallerChildIndex = leftChild(index);
            int rightChildIndex = rightChild(index);
            if(rightChildIndex < size() && heap[rightChildIndex] < heap[smallerChildIndex])
            {
                smallerChildIndex = rightChildIndex;
            }
            if(heap[index] <= heap[smallerChildIndex])
            {
                break;
            }
            swap(heap[index],heap[smallerChildIndex]);
            index = smallerChildIndex;
        }
    }

    public:
    MinHeap(){};
    int top()
    {
        if(heap.empty())
        {
            cout<<"heap is empty"<<endl;
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

    
    void push(int val)
    {
        heap.push_back(val);
        int current = heap.size()-1;
        UP(current);
    }

    int pop()
    {
        if(Isempty())
        {
            cout<<"heap is empty"<<endl;
            return 0;
        }
        int temp = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        if(!Isempty())
        DOWN(0);
        return temp;
    }
};

// 用最小堆获取前 k 大的元素，结果降序排列
// 1. 维护一个大小为 k 的最小堆，堆顶是"门槛"（前 k 大中最小的那个）
// 2. 遍历数组：堆未满直接入；已满时若当前元素 > 门槛，踢掉门槛加入新元素
// 3. 遍历完后堆中就是前 k 大的数，弹出再反转得降序
// 时间复杂度: O(n log k)
vector<int> topKLargest(vector<int>& arr, int k)
{
    MinHeap heap;
    for (int i = 0; i < arr.size(); i++)
    {
        if (heap.size() < k)
        {
            heap.push(arr[i]);
        }
        else if (arr[i] > heap.top())
        {
            heap.pop();
            heap.push(arr[i]);
        }
    }
    // 依次弹出（升序），反转得到降序
    vector<int> result;
    while (!heap.Isempty())
    {
        result.push_back(heap.pop());
    }
    reverse(result.begin(), result.end());
    return result;
}