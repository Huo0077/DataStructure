// ============================================================
//              顺序栈（Array Stack）—— 基于动态数组
// ============================================================
//
// 【核心概念】
//   栈 = LIFO（Last In First Out，后进先出）。
//   像一叠盘子：最后放上去的最先被拿走。
//
//   用数组实现栈：数组尾部作为栈顶，插入删除都是 O(1)。
//
// 【关键设计】
//   top_index 指向栈顶元素的位置：
//     -1  → 空栈
//      0  → 栈底有一个元素
//      n-1 → 栈满（n = capacity）
//
//   为什么空栈时 top_index = -1？
//     入栈先 top_index++ 再 data[top_index] = val，
//     第一个元素入栈后 top_index = 0，即第一个位置。
//
//   为什么 pop() 不返回值？
//     STL 中的 stack::pop() 也返回 void。把"获取值"和"删除"
//     分开，代码更清晰。需要值时先调用 top()，再调用 pop()。
//
// 【时间复杂度】
//   push/pop/top/Isempty: O(1)
//   这是固定容量的栈，没有扩容操作。
//
// 【与链式栈的比较】
//   顺序栈：内存连续，缓存友好，但容量固定
//   链式栈：容量不受限，但每次 push 需要 new（有内存开销）
//
// ============================================================

#include<iostream>
using namespace std;

template <typename T>
class ArrStack
{
private:
    T* data;         // 存储元素的数组
    int capacity;    // 栈的最大容量
    int top_index;   // 栈顶索引：-1 = 空栈，capacity-1 = 满栈

public:
    ArrStack(int size) : capacity(size), data(new T[capacity]), top_index(-1) {}
    ~ArrStack() { delete[] data; }

    bool Isempty()
    {
        return top_index == -1;
    }

    bool Isfull()
    {
        return top_index == capacity - 1;
    }

    // 入栈：先上移栈顶指针，再放入元素
    void push(T val)
    {
        if (Isfull())
        {
            cout << "error:stack is full" << endl;
            return;
        }
        top_index++;
        data[top_index] = val;
    }

    // 出栈：只移动栈顶指针，不返回值
    // 需要值时先用 top() 获取
    void pop()
    {
        if (Isempty())
        {
            cout << "error:stack is empty" << endl;
            return;
        }
        top_index--;
    }

    // 查看栈顶元素，不出栈
    T top()
    {
        if (Isempty())
        {
            cout << "error:stack is empty" << endl;
            return T();  // 返回默认值（int→0, string→"" 等）
        }
        return data[top_index];
    }
};
