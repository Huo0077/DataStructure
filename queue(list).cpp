// 链式队列：基于单链表实现，模板类
// front 指向队头（出队端），rear 指向队尾（入队端），FIFO
#include<iostream>
using namespace std;

template <typename T>
class Node
{
public:
    T data;
    Node* next;
    Node(T val): data(val), next(nullptr) {}
};

template <typename T>
class Listqueue
{
private:
    Node<T>* front;  // 队头指针（出队从这里删）
    Node<T>* rear;   // 队尾指针（入队往这里加）
public:
    Listqueue(): front(nullptr), rear(nullptr) {}
    ~Listqueue()
    {
        while (!Isempty())
            pop();
    }

    bool Isempty()
    {
        return front == nullptr;
    }

    // 入队：在 rear 后面插入，更新 rear
    void push(T val)
    {
        Node<T>* newNode = new Node<T>(val);
        if (Isempty())
        {
            front = newNode;
            rear = newNode;
        }
        else
        {
            rear->next = newNode;
            rear = newNode;
        }
    }

    // 查看队头元素
    T front_val()
    {
        if (Isempty())
        {
            cout << "queue is empty" << endl;
            return T();
        }
        return front->data;
    }

    // 出队：从 front 删除，如果删空了要把 rear 也置空
    void pop()
    {
        if (Isempty())
        {
            cout << "queue is empty" << endl;
            return;
        }
        Node<T>* temp = front;
        front = front->next;
        if (front == nullptr)
            rear = nullptr;  // 队列已空，rear 也需置空
        delete temp;
    }
};