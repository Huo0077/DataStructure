#include<iostream>
using namespace std;
template <typename T>
class ArrStack
{
    private:
    T* data;
    int capacity;
    int top_index;

    public:
    ArrStack(int size):capacity(size),data(new T[capacity]),top_index(-1){}
    ~ArrStack(){ delete[] data;}

    bool Isempty()
    {
        if(top_index == -1)
        return true;
        else
        return false;
    }

    bool Isfull()
    {
        if(top_index == capacity-1)
        return true;
        else
        return false;
    }

    void push(T val)
    {
        if(Isfull())
        {
            cout<<"error:stack is full"<<endl;
            return;
        }
        top_index++;
        data[top_index] = val;
    }

    T pop()
    {
        if(Isempty())
        {
            cout<<"error:stack is empty"<<endl;
            return T();
        }
        T val = data[top_index];
        top_index--;
        return val;
    }

    T top()
    {
        if(Isempty())
        {
            cout<<"error:stack is empty"<<endl;
            return T();
        }
        return data[top_index];
    }
};