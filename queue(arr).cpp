// ============================================================
//          顺序队列（Array Queue）—— 基于循环数组
// ============================================================
//
// 【核心概念】
//   队列 = FIFO（First In First Out，先进先出）。
//   像排队买票：先来的先被服务。
//
//   循环数组：用数组模拟环形结构，避免出队后空间浪费。
//
// 【循环数组原理】
//   数组逻辑上是一个"环"：
//     rear  → 入队位置（下一个插入位置）
//     front → 出队位置（队头）
//
//   入队 arr[rear]=val, rear = (rear+1) % capacity
//   出队 front = (front+1) % capacity
//   两个指针都顺时针移动，不会浪费出队后空出的位置。
//
//   图示（capacity=6，已入队 10,20,30，已出队 10）：
//     索引:   [0]  [1]  [2]  [3]  [4]  [5]
//     内容:    ?   20   30    ?    ?    ?
//                 ↑front   ↑rear
//     front=1, rear=3，元素 20 和 30 在队列中
//
// 【牺牲一个位置区分空/满】
//   如果所有 capacity 个位置都用来存数据，就会出现一个问题：
//     front == rear  到底是空还是满？（无法区分）
//
//   解决方案：牺牲一个位置。
//     空: front == rear
//     满: (rear + 1) % capacity == front
//   实际可用容量 = capacity - 1 = 用户传入的 size
//
//   这就是为什么构造函数中 capacity = size + 1。
//
// 【时间复杂度】
//   所有操作 O(1)。无数据搬移，纯指针移动。
//
// ============================================================

#include<iostream>
using namespace std;

template <typename T>
class Queue
{
private:
    T* arr;
    int capacity;   // 实际数组容量 = 用户指定 + 1（牺牲一个位置区分空/满）
    int front;      // 队头索引（下次出队位置）
    int rear;       // 队尾索引（下次入队位置）

public:
    Queue(int size) : capacity(size + 1), arr(new T[capacity]), front(0), rear(0) {}
    ~Queue() { delete[] arr; }

    // 空：front 和 rear 重合
    bool Isempty()
    {
        return rear == front;
    }

    // 满：rear 再前进一格就追上 front（两者之间恰好差一个空位）
    bool Isfull()
    {
        return (rear + 1) % capacity == front;
    }

    // 入队：放入 rear 位置，rear 顺时针前进
    void push(T val)
    {
        if (Isfull())
        {
            cout << "queue is full" << endl;
            return;
        }
        arr[rear] = val;
        rear = (rear + 1) % capacity;  // 循环前进，到末尾时回到 0
    }

    // 查看队头元素（不出队）
    T front_val()
    {
        if (Isempty())
        {
            cout << "queue is empty" << endl;
            return T();
        }
        return arr[front];
    }

    // 出队：front 顺时针前进，相当于丢弃当前队头
    void pop()
    {
        if (Isempty())
        {
            cout << "queue is empty" << endl;
            return;
        }
        front = (front + 1) % capacity;
    }
};
