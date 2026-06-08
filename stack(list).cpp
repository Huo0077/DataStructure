// ============================================================
//        链式栈（List Stack）—— 基于单链表 + 两个栈应用
// ============================================================
//
// 【核心概念】
//   链表头部 = 栈顶。push 在头部插入，pop 从头部删除。
//   链表尾部不关心，入栈出栈都只操作头部 → O(1)。
//
// 【为什么链表头作为栈顶？】
//   头部插入/删除是 O(1)（只需修改头指针）。
//   如果尾部作为栈顶，pop 时需要找到尾节点的前驱，是 O(n)。
//
//   push: newnode → top → ...      (在头部插入新节点)
//   pop:  top → top->next          (删除头部节点)
//
// 【模板类 & 模板友元】
//   Node<T> 和 ListStack<T> 使用模板，可以存储任意类型。
//   使用 T() 作为错误的默认返回值（int=0, char='\0' 等）。
//
// 【附带的两个栈应用】
//
//   1. Match —— 括号匹配
//      遍历字符串，左括号入栈，右括号时弹出栈顶比对。
//      最后栈为空 = 全部匹配成功。
//      例如: "({[]})" → 匹配 ✓
//            "({[})"  → 不匹配 ✗
//
//   2. evalRPN —— 逆波兰表达式求值
//      逆波兰表达式（后缀表达式）把运算符放在操作数后面：
//        "3 4 +"     → 3 + 4 = 7
//        "2 3 4 + *" → 2 * (3 + 4) = 14
//
//      求值规则：
//        - 遇数字 → 入栈
//        - 遇运算符 → 弹出两个数，计算后压回栈顶
//        - 先弹出的是右操作数，后弹出的是左操作数（减法/除法顺序重要！）
//
//      后缀表达式的优点：不需要括号，不需要考虑运算符优先级，
//      计算机求值极简（单次遍历，O(n)）。
//
// ============================================================

#include<iostream>
#include<vector>
#include<string>
using namespace std;

template <typename T>
class Node
{
public:
    T data;
    Node* next;
    Node(T val) : data(val), next(nullptr) {}
};

template <typename T>
class ListStack
{
private:
    Node<T>* top;  // 栈顶指针 = 链表头节点

public:
    ListStack() : top(nullptr) {}

    // 析构：逐个弹出释放所有节点
    ~ListStack()
    {
        while (top)
        {
            Node<T>* tmp = top;
            top = top->next;
            delete tmp;
        }
    }

    bool Isempty()
    {
        return top == nullptr;
    }

    // 入栈：在链表头部插入新节点
    void push(T val)
    {
        Node<T>* newNode = new Node<T>(val);
        newNode->next = top;  // 新节点指向旧栈顶
        top = newNode;        // 更新栈顶为新节点
    }

    // 出栈：从链表头部删除节点
    void pop()
    {
        if (Isempty())
        {
            cout << "error:stack is empty" << endl;
            return;
        }
        Node<T>* tmp = top;
        top = top->next;
        delete tmp;
    }

    // 查看栈顶元素（不出栈）
    T peek()
    {
        if (Isempty())
        {
            cout << "error:stack is empty" << endl;
            return T();
        }
        return top->data;
    }
};

// ============================================================
// 括号匹配
//
// 规则：
//   1. 左括号 → 入栈
//   2. 右括号 → 弹出栈顶，必须类型匹配，否则失败
//   3. 遍历完后栈必须为空（左括号不能有多余）
//
// 时间复杂度：O(n)，n = 字符串长度
// ============================================================
bool Match(string str)
{
    ListStack<char> st;
    for (int i = 0; i < str.size(); i++)
    {
        char c = str[i];
        if (c == '(' || c == '[' || c == '{')
        {
            st.push(c);  // 左括号入栈
        }
        else if (c == ')' || c == ']' || c == '}')
        {
            if (st.Isempty())
            {
                return false;  // 右括号多余
            }
            char topChar = st.peek();
            st.pop();
            // 检查类型是否匹配
            if ((c == ')' && topChar != '(') ||
                (c == ']' && topChar != '[') ||
                (c == '}' && topChar != '{'))
            {
                return false;  // 类型不匹配
            }
        }
    }
    return st.Isempty();  // 栈空 = 全部匹配
}

// ============================================================
// 逆波兰表达式求值
//
// 后缀表达式示例：
//   ["2","1","+","3","*"]  →  (2+1)*3 = 9
//
// 求值过程追踪：
//   读 "2" → 栈: [2]
//   读 "1" → 栈: [2,1]
//   读 "+" → 弹出 1,2 → 算 2+1=3 → 栈: [3]
//   读 "3" → 栈: [3,3]
//   读 "*" → 弹出 3,3 → 算 3*3=9 → 栈: [9]
//   结果 = 9
//
// 注意：先弹出的 num2 是右操作数，后弹出的 num1 是左操作数。
//       对于 "+" 和 "*" 顺序无所谓，但对 "-" 和 "/" 至关重要！
//
// 时间复杂度：O(n)，每个 token 处理一次
// ============================================================
int evalRPN(vector<string>& Input)
{
    ListStack<int> st;
    for (int i = 0; i < Input.size(); i++)
    {
        if (Input[i] == "+" || Input[i] == "-" ||
            Input[i] == "*" || Input[i] == "/")
        {
            // 注意弹出顺序：先右后左
            int num2 = st.peek();
            st.pop();
            int num1 = st.peek();
            st.pop();

            if (Input[i] == "+")
            {
                st.push(num1 + num2);
            }
            else if (Input[i] == "-")
            {
                st.push(num1 - num2);  // 顺序关键：num1 - num2
            }
            else if (Input[i] == "*")
            {
                st.push(num1 * num2);
            }
            else if (Input[i] == "/")
            {
                st.push(num1 / num2);  // 顺序关键：num1 / num2
            }
        }
        else
        {
            st.push(stoi(Input[i]));  // 数字字符串 → int
        }
    }
    int result = st.peek();
    st.pop();
    return result;
}
