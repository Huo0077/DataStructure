// ============================================================
//          链式队列（List Queue）—— 基于单链表
// ============================================================
//
// 【核心概念】
//   用单链表实现 FIFO 队列。维护两个指针：
//     front → 队头（出队端），链表头
//     rear  → 队尾（入队端），链表尾
//
// 【为什么需要两个指针？】
//   如果只有头指针：
//     入队：需要遍历到链表末尾才能插入 → O(n)
//     出队：O(1)
//   维护尾指针后：
//     入队：直接在 rear 后面插入 → O(1)
//     出队：从 front 删除 → O(1)
//   两个操作都是 O(1)，双赢！
//
// 【关键细节：pop 时队列变空】
//   出队后如果 front 变成 nullptr（最后一个元素被删了），
//   必须同时把 rear 也置为 nullptr。否则 rear 还指向已释放
//   的内存，下次 push 时会访问野指针。
//
//   示例：
//     push(10): front=rear=[10]
//     pop():    front=nullptr, rear 必须也设置为 nullptr！
//
// 【与顺序队列的比较】
//   顺序队列：容量固定、内存连续、缓存友好
//   链式队列：容量不限、每次 push 需 new、节点有指针开销
//
// ============================================================

#include<iostream>
using namespace std;

template <typename T>
class Node
{
public:
    T data;
    Node* next;
    Node(T val) : data(val), next(nullptr) {}
};

template <typename T>
class Listqueue
{
private:
    Node<T>* front;  // 队头指针（出队端 = 链表头）
    Node<T>* rear;   // 队尾指针（入队端 = 链表尾）

public:
    Listqueue() : front(nullptr), rear(nullptr) {}

    ~Listqueue()
    {
        while (!Isempty())
        {
            pop();
        }
    }

    bool Isempty()
    {
        return front == nullptr;
    }

    // 入队：在链表尾部插入新节点
    void push(T val)
    {
        Node<T>* newNode = new Node<T>(val);
        if (Isempty())
        {
            // 空队：新节点既是头也是尾
            front = newNode;
            rear = newNode;
        }
        else
        {
            rear->next = newNode;  // 挂在尾部后面
            rear = newNode;        // 更新尾指针
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

    // 出队：从链表头部删除节点
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
        {
            rear = nullptr;  // 关键！队列为空时尾指针也必须置空
        }
        delete temp;
    }
};
