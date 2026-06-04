// 顺序队列：基于循环数组实现，模板类
// 牺牲一个数组位置来区分队空（front==rear）和队满（(rear+1)%capacity==front）
#include<iostream>
using namespace std;

template<typename T>
class Queue
{
    private:
    T* arr;
    int capacity;   // 实际数组大小 = 用户指定容量 + 1（预留一位）
    int front;      // 队头索引
    int rear;       // 队尾索引（指向下一个可插入位置）

    public:
    Queue(int size):capacity(size+1),arr(new T[capacity]),front(0),rear(0){}
    ~Queue(){delete[] arr;}

    bool Isempty()
    {
        return rear == front;
    }

    bool Isfull()
    {
        return (rear+1)%capacity == front;
    }

    // 入队：放入 rear 位置，rear 循环后移
    void push(T val)
    {
        if(Isfull())
        {
            cout<<"queue is full"<<endl;
            return;
        }
        arr[rear]=val;
        rear = (rear+1)%capacity;
    }

    // 查看队头元素
    T front_val()
    {
        if(Isempty())
        {
            cout<<"queue is empty"<<endl;
            return T();
        }
        return arr[front];
    }

    // 出队：直接后移 front，不返回值（需要值时先调用 front_val()）
    void pop()
    {
        if(Isempty())
        {
            cout<<"queue is empty"<<endl;
            return;
        }
        front = (front+1)%capacity;
    }
};