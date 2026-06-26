// ============================================================
//     线性结构合集：栈（Stack）& 队列（Queue）& 变体
// ============================================================
//
// 本文件涵盖"栈和队列"知识点的所有实现：
//   Part 1: 顺序栈 (Array Stack)
//   Part 2: 链式栈 (List Stack) + 括号匹配 + 逆波兰表达式
//   Part 3: 顺序队列 (Array Queue) —— 循环数组
//   Part 4: 链式队列 (List Queue) —— 单链表+双指针
//   Part 5: 最小栈 (Min Stack) —— O(1) 获取最小值
//   Part 6: 双栈实现队列 (Two Stacks → Queue)
//   Part 7: 双端队列 (Deque) —— 双向链表
//   Part 8: 单向循环链表 (Circular Singly Linked List)
//
// ============================================================

#include<iostream>
#include<vector>
#include<stack>
#include<string>
using namespace std;

// ============================================================
//   Part 1: 顺序栈（Array Stack）—— 基于动态数组
// ============================================================
//
// 【核心概念】
//   栈 = LIFO（Last In First Out）。数组尾部作为栈顶，O(1)。
//   top_index = -1 表示空栈，top_index++ 再放入元素。
//   pop() 不返回值（STL 风格），先 top() 再 pop()。
//   顺序栈：内存连续、缓存友好、容量固定。
//   链式栈：容量无限、每次 push 需 new。
//
// 【时间复杂度】push/pop/top/IsEmpty 均为 O(1)
// ============================================================

template <typename T>
class ArrStack
{
private:
    int capacity;
    T* data;
    int top_index;    // -1=空栈

public:
    ArrStack(int size) : capacity(size), data(new T[capacity]), top_index(-1) {}
    ~ArrStack() { delete[] data; }

    bool IsEmpty()       { return top_index == -1; }
    bool IsFull()        { return top_index == capacity - 1; }

    void Push(T val)
    {
        if (IsFull()) { cout << "error:stack is full" << endl; return; }
        top_index++;
        data[top_index] = val;
    }

    void Pop()
    {
        if (IsEmpty()) { cout << "error:stack is empty" << endl; return; }
        top_index--;
    }

    T Top()
    {
        if (IsEmpty()) { cout << "error:stack is empty" << endl; return T(); }
        return data[top_index];
    }
};

// ============================================================
//   Part 2: 链式栈（List Stack）—— 基于单链表
// ============================================================
//
// 【核心概念】
//   链表头部 = 栈顶。push 在头部插入，pop 从头部删除，均为 O(1)。
//   如果尾部作为栈顶，pop 需要 O(n) 找前驱。
//
// 【附带应用】
//   1. Match() —— 括号匹配：左括号入栈，右括号比对栈顶
//   2. evalRPN() —— 逆波兰表达式求值：数字入栈，遇运算符弹出两个计算
//     注意弹出顺序：先弹出的是右操作数，后弹出的是左操作数
// ============================================================

template <typename T>
struct LNode
{
    T data;
    LNode* next;
    LNode(T val) : data(val), next(nullptr) {}
};

template <typename T>
class ListStack
{
private:
    LNode<T>* top;

public:
    ListStack() : top(nullptr) {}
    ~ListStack()
    {
        while (top) { LNode<T>* tmp = top; top = top->next; delete tmp; }
    }

    bool IsEmpty() { return top == nullptr; }

    void Push(T val)
    {
        LNode<T>* newNode = new LNode<T>(val);
        newNode->next = top;
        top = newNode;
    }

    void Pop()
    {
        if (IsEmpty()) { cout << "error:stack is empty" << endl; return; }
        LNode<T>* tmp = top;
        top = top->next;
        delete tmp;
    }

    T Peek()
    {
        if (IsEmpty()) { cout << "error:stack is empty" << endl; return T(); }
        return top->data;
    }
};

// ---- 括号匹配 O(n) ----
bool Match(string str)
{
    ListStack<char> st;
    for (char c : str)
    {
        if (c == '(' || c == '[' || c == '{')
        {
            st.Push(c);
        }
        else if (c == ')' || c == ']' || c == '}')
        {
            if (st.IsEmpty()) return false;
            char topChar = st.Peek(); st.Pop();
            if ((c == ')' && topChar != '(') ||
                (c == ']' && topChar != '[') ||
                (c == '}' && topChar != '{'))
                return false;
        }
    }
    return st.IsEmpty();
}

// ---- 逆波兰表达式求值 O(n) ----
int evalRPN(vector<string>& Input)
{
    ListStack<int> st;
    for (string& token : Input)
    {
        if (token == "+" || token == "-" || token == "*" || token == "/")
        {
            int num2 = st.Peek(); st.Pop();      // 右操作数先弹出
            int num1 = st.Peek(); st.Pop();      // 左操作数后弹出
            if (token == "+")      st.Push(num1 + num2);
            else if (token == "-") st.Push(num1 - num2);
            else if (token == "*") st.Push(num1 * num2);
            else if (token == "/") st.Push(num1 / num2);
        }
        else
        {
            st.Push(stoi(token));
        }
    }
    int result = st.Peek(); st.Pop();
    return result;
}

// ============================================================
//   Part 3: 顺序队列（Array Queue）—— 基于循环数组
// ============================================================
//
// 【核心概念】
//   循环数组避免出队后空间浪费。front/rear 顺时针移动。
//   牺牲一个位置区分空/满：空=front==rear，满=(rear+1)%cap==front
//   实际可用容量 = capacity - 1 = 用户传入的size
// ============================================================

template <typename T>
class ArrQueue
{
private:
    int capacity;    // 实际容量 = size+1（牺牲一个位置）
    T* arr;
    int front;       // 队头（出队位置）
    int rear;        // 队尾（入队位置）

public:
    ArrQueue(int size) : capacity(size + 1), arr(new T[capacity]), front(0), rear(0) {}
    ~ArrQueue() { delete[] arr; }

    bool IsEmpty() { return rear == front; }
    bool IsFull()  { return (rear + 1) % capacity == front; }

    void Push(T val)    // Enqueue
    {
        if (IsFull()) { cout << "queue is full" << endl; return; }
        arr[rear] = val;
        rear = (rear + 1) % capacity;
    }

    T Front()
    {
        if (IsEmpty()) { cout << "queue is empty" << endl; return T(); }
        return arr[front];
    }

    void Pop()          // Dequeue
    {
        if (IsEmpty()) { cout << "queue is empty" << endl; return; }
        front = (front + 1) % capacity;
    }
};

// ============================================================
//   Part 4: 链式队列（List Queue）—— 单链表 + 双指针
// ============================================================
//
// 【核心概念】
//   维护 front (队头/出队端) 和 rear (队尾/入队端) 两个指针。
//   入队 O(1)：rear->next = newNode, rear = newNode
//   出队 O(1)：front = front->next
//   关键细节：出队后变空时 rear 也必须置空！
// ============================================================

template <typename T>
class ListQueue
{
private:
    LNode<T>* front;
    LNode<T>* rear;

public:
    ListQueue() : front(nullptr), rear(nullptr) {}
    ~ListQueue() { while (!IsEmpty()) Pop(); }

    bool IsEmpty() { return front == nullptr; }

    void Push(T val)
    {
        LNode<T>* newNode = new LNode<T>(val);
        if (IsEmpty()) { front = newNode; rear = newNode; }
        else           { rear->next = newNode; rear = newNode; }
    }

    T Front()
    {
        if (IsEmpty()) { cout << "queue is empty" << endl; return T(); }
        return front->data;
    }

    void Pop()
    {
        if (IsEmpty()) { cout << "queue is empty" << endl; return; }
        LNode<T>* tmp = front;
        front = front->next;
        if (front == nullptr) rear = nullptr;  // 关键！空了尾指针也要置空
        delete tmp;
    }
};

// ============================================================
//   Part 5: 最小栈（Min Stack）—— O(1) 获取最小值
// ============================================================
//
// 【问题】设计栈支持 push/pop/top/getMin，全部 O(1)
//
// 【解法：辅助栈同步法】
//   维护两个栈：data_stack（存数据）和 min_stack（存最小值）。
//   min_stack 栈顶始终 = data_stack 当前所有元素的最小值。
//
//   push(val):
//     data_stack 直接压入 val
//     min_stack 压入 min(val, 当前最小值)
//     —— 如果 min_stack 为空或 val ≤ 当前最小值，压入 val
//     —— 否则，重复压入当前最小值（保持两栈深度一致！）
//
//   为什么要"重复压入"而不是只在新最小值时压入？
//     如果只在 val 更小时压入 min_stack，pop 时就无法判断
//     "这个元素是最小值吗？"，需要额外判断逻辑。
//     同步压入 → pop 时两个栈同步弹出即可，getMin 永远是 min_stack.top()
//
// 【空间换时间】O(n) 额外空间
// ============================================================

class MinStack
{
private:
    stack<int> data_stack;
    stack<int> min_stack;

public:
    void Push(int val)
    {
        data_stack.push(val);
        if (min_stack.empty() || min_stack.top() >= val)
            min_stack.push(val);
        else
            min_stack.push(min_stack.top());  // 保持两栈深度一致
    }

    void Pop()
    {
        if (data_stack.empty()) { cout << "stack is empty" << endl; return; }
        data_stack.pop();
        min_stack.pop();
    }

    int Top()
    {
        if (data_stack.empty()) { cout << "stack is empty" << endl; return -1; }
        return data_stack.top();
    }

    int GetMin()
    {
        if (min_stack.empty()) { cout << "stack is empty" << endl; return -1; }
        return min_stack.top();
    }
};

// ============================================================
//   Part 6: 双栈实现队列（Two Stacks → Queue）
// ============================================================
//
// 【核心思想】
//   栈 = LIFO（后进先出），队列 = FIFO（先进先出）。
//   一个 LIFO 结构做不到 FIFO，但 两个 LIFO 串联 = FIFO！
//
//   stackIn  —— 入队端（push 到这里）
//   stackOut —— 出队端（pop/peek 从这里取）
//
//   当 stackOut 为空时，把 stackIn 全部倒进 stackOut：
//     stackIn  [底→顶] = 1, 2, 3    (1最早入栈，在底部)
//     stackOut [底→顶] = 3, 2, 1    (倒过来，1在顶部)
//     弹出 1 ← 正是最早入队的元素！
//
//   两次 LIFO 反转 = FIFO：入队时反转一次，倒入时再反一次 = 回正。
//
// 【均摊 O(1) 分析】
//   push 永远 O(1)。pop/peek 有时需要 O(n) 的 Transfer。
//   但 每个元素只被倒入一次：push→stackIn, Transfer→stackOut, pop 掉。
//   所以 n 次 push + n 次 pop 总共 O(n)，每次 pop 均摊 O(1)。
//   这是"会计方法"的经典例子。
// ============================================================

template <typename T>
class TwoStackQueue
{
private:
    stack<T> stackIn;
    stack<T> stackOut;

    void Transfer()
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
    void Push(T val) { stackIn.push(val); }

    void Pop()
    {
        Transfer();
        if (stackOut.empty()) { cout << "queue is empty" << endl; return; }
        stackOut.pop();
    }

    T Peek()
    {
        Transfer();
        if (stackOut.empty()) { cout << "queue is empty" << endl; return T(); }
        return stackOut.top();
    }

    bool IsEmpty() { return stackIn.empty() && stackOut.empty(); }
};

// ============================================================
//   Part 7: 双端队列（Deque）—— 双向链表
// ============================================================
//
// 【核心概念】
//   前后两端都可插入/删除。双向链表保证所有操作 O(1)。
//
// 【关键边界：空队处理】
//   两个规则必须严格遵守，否则出现野指针：
//
//   规则 1：当 Pop 导致队列变空时
//     front 或 rear 被删掉后，另一个仍指向已释放的节点！
//     必须同时将 front 和 rear 都置为 nullptr。
//
//   规则 2：当 Push 进入空队时
//     新节点既是队头也是队尾 → front = rear = node。
//
//   示例（PopFront 导致变空）：
//     front=rear=[5]         只含一个节点
//     front = front->next    → front = nullptr ✓
//     rear 还指向 [5]！      → 必须同时 rear = nullptr ✓
//     delete tmp              → 释放 [5]
// ============================================================

template <typename T>
struct DNode
{
    T data;
    DNode* prev;
    DNode* next;
    DNode(T val) : data(val), prev(nullptr), next(nullptr) {}
};

template <typename T>
class Deque
{
private:
    DNode<T>* front;
    DNode<T>* rear;

public:
    Deque() : front(nullptr), rear(nullptr) {}
    ~Deque() { while (!IsEmpty()) PopFront(); }

    bool IsEmpty() { return front == nullptr; }

    void PushFront(T val)
    {
        DNode<T>* node = new DNode<T>(val);
        if (IsEmpty()) { front = node; rear = node; }
        else { node->next = front; front->prev = node; front = node; }
    }

    void PushBack(T val)
    {
        DNode<T>* node = new DNode<T>(val);
        if (IsEmpty()) { front = node; rear = node; }
        else { node->prev = rear; rear->next = node; rear = node; }
    }

    T FrontVal()
    {
        if (IsEmpty()) { cout << "deque is empty" << endl; return T(); }
        return front->data;
    }

    T BackVal()
    {
        if (IsEmpty()) { cout << "deque is empty" << endl; return T(); }
        return rear->data;
    }

    void PopFront()
    {
        if (IsEmpty()) { cout << "deque is empty" << endl; return; }
        DNode<T>* tmp = front;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        else                  front->prev = nullptr;
        delete tmp;
    }

    void PopBack()
    {
        if (IsEmpty()) { cout << "deque is empty" << endl; return; }
        DNode<T>* tmp = rear;
        rear = rear->prev;
        if (rear == nullptr) front = nullptr;
        else                 rear->next = nullptr;
        delete tmp;
    }
};

// ============================================================
//   Part 8: 单向循环链表（Circular Singly Linked List）
// ============================================================
//
// 【核心概念】
//   尾节点 next 指向头节点，形成环。没有真正的 nullptr。
//
//   【遍历】用 do-while（而不是 while），因为起始就是 first，
//      必须至少走一步才能判断是否回到起点。
//
//   【删除头节点的特殊性】
//      普通节点删除：prev->next = cur->next，简单。
//      头节点删除：需要找到尾节点（其 next 是 first），
//      更新尾节点的 next 指向新的头节点。
//      如果只有一个节点：first->next == first 自己指向自己，
//      删除后链表为空 → first = nullptr。
// ============================================================

class CircularList
{
private:
    LNode<int>* first;

public:
    CircularList() : first(nullptr) {}

    void Insert(int val)
    {
        LNode<int>* node = new LNode<int>(val);
        if (!first)
        {
            first = node;
            node->next = first;    // 自己指向自己成环
            return;
        }
        LNode<int>* cur = first;
        while (cur->next != first) cur = cur->next;  // 找尾节点
        cur->next = node;
        node->next = first;
    }

    void Print()
    {
        if (!first) { cout << "链表为空" << endl; return; }
        LNode<int>* cur = first;
        do { cout << cur->data << "->"; cur = cur->next; } while (cur != first);
        cout << endl;
    }

    void Delete(int val)
    {
        if (!first) { cout << "list is empty" << endl; return; }
        LNode<int>* cur = first;
        LNode<int>* prev = nullptr;
        do
        {
            if (cur->data == val)
            {
                if (cur == first)
                {
                    if (first->next == first)  // 只有一个节点
                    {
                        delete first; first = nullptr; return;
                    }
                    LNode<int>* last = first;
                    while (last->next != first) last = last->next;
                    first = first->next;
                    last->next = first;
                    delete cur;
                }
                else
                {
                    prev->next = cur->next;
                    delete cur;
                }
                return;
            }
            prev = cur;
            cur = cur->next;
        } while (cur != first);
        cout << "value not found" << endl;
    }
};

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║   栈 & 队列 综合测试               ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    // ---- Part 1: 顺序栈 ----
    cout << "\n┌─ Part 1: 顺序栈 ────────────────┐" << endl;
    ArrStack<int> ast(5);
    ast.Push(10); ast.Push(20); ast.Push(30);
    cout << "Top=" << ast.Top() << " (expect 30)" << endl;
    ast.Pop();
    cout << "After pop, Top=" << ast.Top() << " (expect 20)" << endl;

    // ---- Part 2: 链式栈 + 应用 ----
    cout << "\n┌─ Part 2: 链式栈 + 括号匹配 ────┐" << endl;
    cout << "\"({[]})\" → " << (Match("({[]})") ? "匹配" : "不匹配") << endl;
    cout << "\"({[})\" → " << (Match("({[})") ? "匹配" : "不匹配") << endl;
    vector<string> rpn = {"2","1","+","3","*"};
    cout << "\"2 1 + 3 *\" = " << evalRPN(rpn) << " (expect 9)" << endl;

    // ---- Part 3-4: 队列 ----
    cout << "\n┌─ Part 3-4: 顺序队列 & 链式队列 ─┐" << endl;
    ArrQueue<int> aq(5);
    aq.Push(1); aq.Push(2); aq.Push(3);
    cout << "Front=" << aq.Front() << " (expect 1)" << endl;
    aq.Pop();
    cout << "After Dequeue, Front=" << aq.Front() << " (expect 2)" << endl;

    ListQueue<int> lq;
    lq.Push(100); lq.Push(200);
    cout << "ListQueue Front=" << lq.Front() << " (expect 100)" << endl;
    lq.Pop();
    cout << "After Dequeue, Front=" << lq.Front() << " (expect 200)" << endl;

    // ---- Part 5: 最小栈 ----
    cout << "\n┌─ Part 5: 最小栈 ─────────────────┐" << endl;
    MinStack ms;
    ms.Push(5); ms.Push(2); ms.Push(3); ms.Push(1);
    cout << "Min=" << ms.GetMin() << " (expect 1)" << endl;
    ms.Pop();
    cout << "After pop, Min=" << ms.GetMin() << " (expect 2)" << endl;

    // ---- Part 6: 双栈队列 ----
    cout << "\n┌─ Part 6: 双栈实现队列 ──────────┐" << endl;
    TwoStackQueue<int> tsq;
    tsq.Push(1); tsq.Push(2); tsq.Push(3);
    cout << "Peek=" << tsq.Peek() << " (expect 1)" << endl;
    tsq.Pop();
    cout << "After pop, Peek=" << tsq.Peek() << " (expect 2)" << endl;

    // ---- Part 7: 双端队列 ----
    cout << "\n┌─ Part 7: 双端队列 ───────────────┐" << endl;
    Deque<int> dq;
    dq.PushFront(10); dq.PushBack(20); dq.PushFront(5);
    cout << "Front=" << dq.FrontVal() << " (expect 5)" << endl;
    cout << "Back=" << dq.BackVal() << " (expect 20)" << endl;
    dq.PopFront(); dq.PopBack();
    cout << "After pops, Front=" << dq.FrontVal() << " (expect 10)" << endl;

    // ---- Part 8: 循环链表 ----
    cout << "\n┌─ Part 8: 循环链表 ────────────────┐" << endl;
    CircularList cl;
    cl.Insert(1); cl.Insert(2); cl.Insert(3);
    cout << "循环链表: "; cl.Print();
    cl.Delete(2);
    cout << "删除 2 后: "; cl.Print();

    cout << "\n所有测试完成！" << endl;
    return 0;
}
