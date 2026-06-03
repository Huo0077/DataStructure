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
    Node<T>* front;
    Node<T>* rear;
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

    T pop()
    {
        if (Isempty())
        {
            cout << "queue is empty" << endl;
            return T();
        }
        Node<T>* temp = front;
        T data = temp->data;
        front = front->next;
        if (front == nullptr)
            rear = nullptr;
        delete temp;
        return data;
    }
};
