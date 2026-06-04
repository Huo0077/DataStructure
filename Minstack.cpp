// 最小栈：在 O(1) 时间内获取栈中最小值
// 使用辅助栈 min_stack，与数据栈保持同步，栈顶始终是当前最小值
#include<iostream>
#include<stack>
using namespace std;

class Minstack
{
    private:
    stack<int> data_stack;  // 数据栈
    stack<int> min_stack;   // 辅助栈，栈顶为当前 data_stack 中的最小值
    public:
    void push(int val)
    {
        data_stack.push(val);
        // 如果 val 比当前最小值更小（或等于），则入 min_stack
        if(min_stack.empty() || min_stack.top()>=val)
            min_stack.push(val);
        else
            min_stack.push(min_stack.top());  // 重复存入当前最小值，保持两栈深度一致
    }

    void pop()
    {
        if(data_stack.empty())
        {
            cout<<"stack is empty"<<endl;
            return;
        }
        data_stack.pop();
        min_stack.pop();
    }

    int top()
    {
        if(data_stack.empty())
        {
            cout<<"stack is empty"<<endl;
            return -1;
        }
        return data_stack.top();
    }

    int getMin()
    {
        if(min_stack.empty())
        {
            cout<<"stack is empty"<<endl;
            return -1;
        }
        return min_stack.top();
    }
};
