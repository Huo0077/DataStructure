// 顺序栈：基于动态数组实现，模板类
// top_index 指向栈顶元素位置，-1 表示空栈
#include<iostream>
using namespace std;

template <typename T>
class ArrStack
{
    private:
    T* data;         // 存储元素的数组
    int capacity;    // 栈的最大容量
    int top_index;   // 栈顶索引，-1 表示空栈

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

    // 入栈：先移动 top，再放入元素
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

    // 出栈：取出栈顶元素后 top 下移
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

    // 查看栈顶元素，不出栈
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