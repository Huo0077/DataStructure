// 单向循环链表：尾节点的 next 指向头节点，形成环
// 支持：尾插、打印、按值删除
#include<iostream>
using namespace std;

class Node
{
    public:
    int data;
    Node* next;
    Node(int val)
    {
        data = val;
        next = nullptr;
    }
};

class Circularlist
{
    private:
    Node* first;  // 头节点指针
    public:
    Circularlist(){first = nullptr;}

    // 尾插：找到尾节点（next==first），在其后插入
    void insert(int val)
    {
        Node* newnode = new Node(val);
        if(!first)
        {
            first = newnode;
            newnode->next = first;  // 自己指向自己，形成环
            return;
        }
        Node* current = first;
        while(current)
        {
            if(current->next == first)  // 找到尾节点
            {
                current->next = newnode;
                newnode->next = first;
                break;
            }
            else
                current = current->next;
        }
    }

    // 循环遍历打印，直到回到 first
    void Print()
    {
        if(!first)
        {
            cout<<"链表为空"<<endl;
            return;
        }
        Node* current = first;
        do
        {
            cout<<current->data<<"->";
            current = current->next;
        }
        while(current!=first);
        cout<<endl;
    }

    // 按值删除
    void Delete(int val)
    {
        if (!first)
        {
            cout << "list is empty" << endl;
            return;
        }
        Node *current = first, *prev = nullptr;
        do
        {
            if (current->data == val)
            {
                if (current == first)
                {
                    // 只有一个节点
                    if (first->next == first)
                    {
                        delete first;
                        first = nullptr;
                        return;
                    }
                    // 删除头节点：先找到尾节点，更新其 next，再更新 first
                    Node* last = first;
                    while (last->next != first)
                        last = last->next;
                    first = first->next;
                    last->next = first;
                    delete current;
                }
                else
                {
                    // 删除非头节点：前驱跳过当前即可
                    prev->next = current->next;
                    delete current;
                }
                return;
            }
            prev = current;
            current = current->next;
        } while (current != first);
        cout << "value not found" << endl;
    }
};