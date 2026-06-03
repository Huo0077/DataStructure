#include<iostream>
using namespace std;
template<typename T>
class Queue
{
    private:
    T* arr;
    int capacity;
    int front;
    int rear;
    
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

    T pop()
    {
        if(Isempty())
        {
            cout<<"queue is empty"<<endl;
            return T();
        }
        T temp = arr[front];
        front = (front+1)%capacity;
        return temp;
    }
};