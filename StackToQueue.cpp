// ============================================================
//              用两个栈实现队列（Two Stacks → Queue）
// ============================================================
//
// 【核心思想】
//   栈是 LIFO（后进先出），队列是 FIFO（先进先出）。
//   两个栈配合可以"反转"顺序，实现先进先出。
//
//   栈的 LIFO 性质意味着：
//     把数据倒入另一个栈，顺序正好颠倒！
//     颠倒两次 = 恢复原顺序，但数据已从栈变成了"队列"。
//
//   stackIn  → 负责入队（push），直接压栈
//   stackOut → 负责出队（pop/peek），栈顶即队头
//
//   当 stackOut 为空时，把 stackIn 的所有元素倒入 stackOut，
//   stackOut 的栈顶就是最早入队的元素（队头）。
//
// 【图解过程】
//   入队 1, 2, 3:
//     stackIn: [1,2,3  ←栈顶
//
//   第一次出队（调用 pop）:
//     stackOut 为空 → 倒入！
//     stackOut: [3,2,1  ←栈顶（1 是队头！）
//     pop: 1
//     stackOut: [3,2  ←栈顶
//
//   入队 4, 5:
//     stackIn: [4,5  ←栈顶
//     stackOut: [3,2  ←栈顶（继续出队，不影响新入元素）
//
//   出队 2:
//     stackOut 不为空 → 直接 pop
//     stackOut: [3  ←栈顶
//
//   出队 3:
//     stackOut 变空 → 再次倒入！
//     stackIn [4,5] → stackOut [5,4  ←栈顶（4 是队头）
//
// 【均摊时间复杂度分析】
//   每个元素最多：
//     入 stackIn 1 次
//     从 stackIn 移到 stackOut 1 次
//     出 stackOut 1 次
//   即每个元素被移动 O(1) 次。
//   所以 n 次操作的总时间是 O(n)，均摊每次 O(1)。
//
//   个别操作的代价可能较高（如某次倒入 n 个元素），
//   但从整体来看，每个元素只被"倒入"一次，均摊下仍是 O(1)。
//
// 【空队判断】
//   两个栈都为空 → 队列为空
//
// ============================================================

#include<iostream>
#include<stack>
using namespace std;

template <typename T>
class StackToQueue
{
private:
    stack<T> stackIn;   // 入队栈：push 直接压入这里
    stack<T> stackOut;  // 出队栈：pop/peek 从这里取

    // 核心操作：当 stackOut 为空时，将 stackIn 全部倒入 stackOut
    // 倒入后 stackOut 的栈顶 = 最早入队的元素（即队头）
    void Tranfer()
    {
        if (stackOut.empty())
        {
            while (!stackIn.empty())
            {
                stackOut.push(stackIn.top());
                stackIn.pop();
            }
        }
    }

public:
    // 入队：直接压入 stackIn，O(1)
    void push(T val)
    {
        stackIn.push(val);
    }

    // 出队：从 stackOut 弹出栈顶，均摊 O(1)
    void pop()
    {
        Tranfer();  // 确保 stackOut 非空
        if (stackOut.empty())
        {
            cout << "queue is empty" << endl;
            return;
        }
        stackOut.pop();
    }

    // 查看队头：即 stackOut 的栈顶，均摊 O(1)
    T peek()
    {
        Tranfer();
        if (stackOut.empty())
        {
            cout << "queue is empty" << endl;
            return T();
        }
        return stackOut.top();
    }

    // 两个栈都为空时队列为空
    bool Isempty()
    {
        return stackIn.empty() && stackOut.empty();
    }
};
