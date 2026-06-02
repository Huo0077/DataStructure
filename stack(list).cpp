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
    Node<T>* top;
public:
    ListStack(): top(nullptr) {}
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

    void push(T val)
    {
        Node<T>* newNode = new Node<T>(val);
        newNode->next = top;
        top = newNode;
    }

    T pop()
    {
        if (Isempty())
        {
            cout << "error:stack is empty" << endl;
            return T();
        }
        Node<T>* tmp = top;
        T val = top->data;
        top = top->next;
        delete tmp;
        return val;
    }

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
            if(st.Isempty())
            {
                return false;
            }
            char topChar = st.pop();
            if ((c == ')' && topChar != '(') ||
                (c == ']' && topChar != '[') ||
                (c == '}' && topChar != '{'))
            {
                return false;
            }
        }
    }
    return st.Isempty();
}

int evalRPN(vector<string>& Input)
{
    ListStack<int> st;
    for(int i=0;i<Input.size();i++)
    {
        if(Input[i]=="+" || Input[i]=="-" || Input[i]=="*" || Input[i]=="/")
        {
            int num2 = st.pop();
            int num1 = st.pop();
            if(Input[i]=="+")
            {
                st.push(num1 + num2);
            }
            else if(Input[i]=="-")
            {
                st.push(num1 - num2);
            }
            else if(Input[i]=="*")
            {
                st.push(num1 * num2);
            }
            else if(Input[i]=="/")
            {
                st.push(num1 / num2);
            }
        }
        else
        {
            st.push(stoi(Input[i]));
        }
    }
    return st.pop();
}