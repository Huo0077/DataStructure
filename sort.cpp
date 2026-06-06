// 插入排序：从前往后，将每个元素插入到前面已排序部分的正确位置
// 时间复杂度：O(n²)，稳定排序
#include<iostream>
#include<vector>
#include<queue>//使用优先队列实现最小堆
using namespace std;

void insertion_sort(vector<int>& arr)
{
    int n = arr.size();
    // 从第二个元素开始，依次往前插入
    for(int i=1; i<n; i++)
    {
        int key = arr[i];     // 当前待插入的元素
        int j = i-1;
        // 将比 key 大的元素逐个后移
        while(j>=0 && arr[j]>key)
        {
            arr[j+1]=arr[j];
            j--;
        }
        arr[j+1]=key;         // 插入到正确位置
    }
}

