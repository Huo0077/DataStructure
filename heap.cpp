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