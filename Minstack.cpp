// ============================================================
//              最小栈（Min Stack）—— O(1) 时间获取最小值
// ============================================================
//
// 【问题】
//   设计一个栈，支持 push、pop、top 和 getMin 操作，且 getMin
//   必须在 O(1) 时间内完成。
//
// 【核心思路：辅助栈同步法】
//   维护两个栈：
//     data_stack —— 正常存储所有元素
//     min_stack  —— 栈顶始终是当前 data_stack 中的最小值
//
//   关键：两个栈的深度始终保持一致。
//   每次 push 时，min_stack 也压入一个值（当前最小值），
//   每次 pop 时，min_stack 同时弹出。
//
// 【为什么不用只记录最小值的变量？】
//   如果只用 int min_val 记录最小值，一旦最小值被 pop 出栈，
//   就无法在 O(1) 时间内找到"次小值"。辅助栈为每个状态都
//   保存了最小值快照，pop 之后自动回退到上一个状态。
//
// 【push 逻辑】
//   新值 val 入 data_stack；
//   若 min_stack 为空 或 val <= min_stack.top()：
//     → min_stack.push(val)      // val 成为新的最小值
//   否则：
//     → min_stack.push(min_stack.top())  // 重复压入当前最小值
//   （用 <= 而非 < 是为了处理重复的最小值）
//
// 【pop 逻辑】
//   data_stack.pop()
//   min_stack.pop()    // 同步弹出，最小值回退到上一个状态
//
// 【空间换时间】
//   额外空间 O(n)，换取 getMin = O(1)。
//   可以优化：只在最小值变化时压入 min_stack（空间 O(n) 但常数更小），
//   但同步法实现更简洁。
//
// ============================================================

#include<iostream>
#include<stack>
using namespace std;

class Minstack
{
private:
    stack<int> data_stack;  // 数据栈：正常存储所有元素
    stack<int> min_stack;   // 辅助栈：栈顶 = 当前 data_stack 的最小值

public:
    // 入栈：同时维护两个栈的深度一致性
    void push(int val)
    {
        data_stack.push(val);
        // 如果 val 不比当前最小值大，它就成为新的最小值
        if (min_stack.empty() || min_stack.top() >= val)
        {
            min_stack.push(val);
        }
        else
        {
            // 重复存入当前最小值，保持两个栈深度一致
            min_stack.push(min_stack.top());
        }
    }

    // 出栈：两个栈同步弹出
    void pop()
    {
        if (data_stack.empty())
        {
            cout << "stack is empty" << endl;
            return;
        }
        data_stack.pop();
        min_stack.pop();  // 同步弹出，最小值自动回退
    }

    // 查看栈顶元素
    int top()
    {
        if (data_stack.empty())
        {
            cout << "stack is empty" << endl;
            return -1;
        }
        return data_stack.top();
    }

    // O(1) 获取当前栈中的最小值
    int getMin()
    {
        if (min_stack.empty())
        {
            cout << "stack is empty" << endl;
            return -1;
        }
        return min_stack.top();
    }
};
