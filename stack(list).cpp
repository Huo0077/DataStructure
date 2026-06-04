// 链式栈：基于单链表实现，模板类
// 栈顶在链表头部，入栈出栈都是 O(1)
// 附带两个栈应用：括号匹配 & 逆波兰表达式求值
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
    Node(T val): data(val), next(nullptr) {}
};

template <typename T>
class ListStack
{
private:
    Node<T>* top;  // 栈顶指针（链表头）
public:
    ListStack(): top(nullptr) {}
    ~ListStack()
    {
        // 逐个弹出释放
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

    // 入栈：新节点插入链表头部
    void push(T val)
    {
        Node<T>* newNode = new Node<T>(val);
        newNode->next = top;
        top = newNode;
    }

    // 出栈：从链表头部删除，不返回值（需要值时先调用 peek()）
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

    // 查看栈顶
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

// 括号匹配：遍历字符串，左括号入栈，右括号时出栈比对
bool Match(string str)
{
    ListStack<char> st;
    for(int i=0;i<str.size();i++)
    {
        char c = str[i];
        if(c == '(' || c == '[' || c == '{')
        {
            st.push(c);
        }
        else if(c == ')' || c == ']' || c == '}')
        {
            if(st.Isempty())        // 右括号多了
                return false;
            char topChar = st.peek();
            st.pop();
            if ((c == ')' && topChar != '(') ||
                (c == ']' && topChar != '[') ||
                (c == '}' && topChar != '{'))
                return false;       // 类型不匹配
        }
    }
    return st.Isempty();  // 栈空说明全部匹配
}

// 逆波兰表达式求值：遇数字入栈，遇运算符弹出两个数计算后压回
int evalRPN(vector<string>& Input)
{
    ListStack<int> st;
    for(int i=0;i<Input.size();i++)
    {
        if(Input[i]=="+" || Input[i]=="-" || Input[i]=="*" || Input[i]=="/")
        {
            int num2 = st.peek(); st.pop();   // 右操作数
            int num1 = st.peek(); st.pop();   // 左操作数
            if(Input[i]=="+")
                st.push(num1 + num2);
            else if(Input[i]=="-")
                st.push(num1 - num2);
            else if(Input[i]=="*")
                st.push(num1 * num2);
            else if(Input[i]=="/")
                st.push(num1 / num2);
        }
        else
        {
            st.push(stoi(Input[i]));  // 数字转 int 后入栈
        }
    }
    int result = st.peek(); st.pop();
    return result;
}