// 双端队列：基于双向链表实现，模板类
// 支持在队头和队尾进行插入和删除，共四种操作
#include<iostream>
using namespace std;

template <typename T>
class Node
{
    public:
    T data;
    Node* prev;
    Node* next;
    Node(T val):data(val),prev(nullptr),next(nullptr){}
};

template <typename T>
class Deque
{
    private:
    Node<T>* front;  // 队头指针
    Node<T>* rear;   // 队尾指针
    public:
    Deque():front(nullptr),rear(nullptr){}
    ~Deque()
    {
        while (!Isempty())
            pop_front();
    }

    bool Isempty()
    {
        return front == nullptr;
    }

    // 队头插入：新节点挂在 front 前面，更新 front
    void push_front(T val)
    {
        Node<T>* newnode = new Node<T>(val);
        if(Isempty())
        {
            front = newnode;
            rear = newnode;
        }
        else
        {
            newnode->next = front;
            front->prev = newnode;
            front = newnode;
        }
    }

    // 队尾插入：新节点挂在 rear 后面，更新 rear
    void push_back(T val)
    {
        Node<T>* newnode = new Node<T>(val);
        if(Isempty())
        {
            front = newnode;
            rear = newnode;
        }
        else
        {
            newnode->prev = rear;
            rear->next = newnode;
            rear = newnode;
        }
    }

    // 查看队头元素
    T front_val()
    {
        if(Isempty())
        {
            cout<<"deque is empty"<<endl;
            return T();
        }
        return front->data;
    }

    // 查看队尾元素
    T back_val()
    {
        if(Isempty())
        {
            cout<<"deque is empty"<<endl;
            return T();
        }
        return rear->data;
    }

    // 队头删除：front 后移，释放原头节点
    void pop_front()
    {
        if(Isempty())
        {
            cout<<"deque is empty"<<endl;
            return;
        }
        Node<T>* temp = front;
        front = front->next;
        if(front == nullptr)
            rear = nullptr;       // 队列已空
        else
            front->prev = nullptr;
        delete temp;
    }

    // 队尾删除：rear 前移，释放原尾节点
    void pop_back()
    {
        if(Isempty())
        {
            cout<<"deque is empty"<<endl;
            return;
        }
        Node<T>* temp = rear;
        rear = rear->prev;
        if(rear == nullptr)
            front = nullptr;      // 队列已空
        else
            rear->next = nullptr;
        delete temp;
    }
};