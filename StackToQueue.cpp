// 用两个栈实现队列（FIFO）
// stackIn 负责入队，stackOut 负责出队
// 当 stackOut 为空时，将 stackIn 的所有元素倒入 stackOut，顺序反转后正好变成先进先出
#include<iostream>
#include<stack>
using namespace std;

template<typename T>
class StackToQueue
{
    private:
    stack<T> stackIn;   // 入队栈
    stack<T> stackOut;  // 出队栈

    // 当 stackOut 为空时，把 stackIn 全部倒入 stackOut
    void Tranfer()
    {
        if(stackOut.empty())
        {
            while(!stackIn.empty())
            {
                stackOut.push(stackIn.top());
                stackIn.pop();
            }
        }
    }
    public:
    // 入队：直接压入 stackIn
    void push(T val)
    {
        stackIn.push(val);
    }

    // 出队：从 stackOut 弹出栈顶，不返回值（需要值时先调用 peek()）
    void pop()
    {
        Tranfer();
        if(stackOut.empty())
        {
            cout<<"queue is empty"<<endl;
            return;
        }
        stackOut.pop();
    }

    // 查看队头：即 stackOut 的栈顶
    T peek()
    {
        Tranfer();
        return stackOut.top();
    }

    // 两个栈都为空时队列为空
    bool Isempty()
    {
        return stackIn.empty() && stackOut.empty();
    }
};