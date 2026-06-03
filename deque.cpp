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
    Node<T>* front;
    Node<T>* rear;
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

    T pop_front()
    {
        if(Isempty())
        {
            cout<<"deque is empty"<<endl;
            return T();
        }
        Node<T>* temp = front;
        T data = temp->data;
        front = front->next;
        if(front == nullptr)
        {
            rear = nullptr;
        }
        else
        {
            front->prev = nullptr;
        }
        delete temp;
        return data;
    }

    T pop_back()
    {
        if(Isempty())
        {
            cout<<"deque is empty"<<endl;
            return T();
        }
        Node<T>* temp = rear;
        T data = temp->data;
        rear = rear->prev;
        if(rear == nullptr)
        {
            front = nullptr;
        }
        else
        {
            rear->next = nullptr;
        }
        delete temp;
        return data;
    }

};
