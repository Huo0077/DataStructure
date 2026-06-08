// ============================================================
//          双端队列（Deque）—— 基于双向链表
// ============================================================
//
// 【核心概念】
//   Deque = Double-Ended Queue，允许在两端进行插入和删除。
//   结合了栈和队列的优点——想从哪端操作就从哪端操作。
//
// 【四种操作】
//   push_front(val) —— 在队头前插入
//   push_back(val)  —— 在队尾后插入
//   pop_front()     —— 删除队头
//   pop_back()      —— 删除队尾
//
// 【为什么用双向链表？】
//   pop_back() 需要找到尾节点的前驱来更新 rear。
//   单向链表找前驱需要从头遍历（O(n)），双向链表直接 rear->prev（O(1)）。
//
//   pop_front() 虽然单向链表也能 O(1)，但为了对称性和 pop_back()，
//   双向链表是最佳选择。
//
// 【空队边界条件】
//   和链式队列类似：
//     - 只有一个节点时，front == rear == 该节点
//     - pop 导致队列变空时，front 和 rear 必须同时设为 nullptr
//     - push 入空队时，front 和 rear 必须同时指向新节点
//
// ============================================================

#include<iostream>
using namespace std;

template <typename T>
class Node
{
public:
    T data;
    Node* prev;
    Node* next;
    Node(T val) : data(val), prev(nullptr), next(nullptr) {}
};

template <typename T>
class Deque
{
private:
    Node<T>* front;  // 队头指针
    Node<T>* rear;   // 队尾指针

public:
    Deque() : front(nullptr), rear(nullptr) {}

    ~Deque()
    {
        while (!Isempty())
        {
            pop_front();
        }
    }

    bool Isempty()
    {
        return front == nullptr;
    }

    // 队头插入：新节点 → 挂在 front 前面 → 成为新 front
    void push_front(T val)
    {
        Node<T>* newnode = new Node<T>(val);
        if (Isempty())
        {
            front = newnode;
            rear = newnode;
        }
        else
        {
            newnode->next = front;  // 新节点 → 旧队头
            front->prev = newnode;  // 旧队头 ← 新节点
            front = newnode;        // 更新 front
        }
    }

    // 队尾插入：新节点 → 挂在 rear 后面 → 成为新 rear
    void push_back(T val)
    {
        Node<T>* newnode = new Node<T>(val);
        if (Isempty())
        {
            front = newnode;
            rear = newnode;
        }
        else
        {
            newnode->prev = rear;   // 新节点 ← 旧队尾
            rear->next = newnode;   // 旧队尾 → 新节点
            rear = newnode;         // 更新 rear
        }
    }

    T front_val()
    {
        if (Isempty())
        {
            cout << "deque is empty" << endl;
            return T();
        }
        return front->data;
    }

    T back_val()
    {
        if (Isempty())
        {
            cout << "deque is empty" << endl;
            return T();
        }
        return rear->data;
    }

    // 队头删除：front 后移，释放旧头节点
    void pop_front()
    {
        if (Isempty())
        {
            cout << "deque is empty" << endl;
            return;
        }
        Node<T>* temp = front;
        front = front->next;
        if (front == nullptr)
        {
            rear = nullptr;       // 队列变空
        }
        else
        {
            front->prev = nullptr;  // 新队头的 prev 置空
        }
        delete temp;
    }

    // 队尾删除：rear 前移，释放旧尾节点
    void pop_back()
    {
        if (Isempty())
        {
            cout << "deque is empty" << endl;
            return;
        }
        Node<T>* temp = rear;
        rear = rear->prev;
        if (rear == nullptr)
        {
            front = nullptr;      // 队列变空
        }
        else
        {
            rear->next = nullptr;  // 新队尾的 next 置空
        }
        delete temp;
    }
};
