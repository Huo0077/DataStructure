// ============================================================
//            单向循环链表（Circular Singly Linked List）
// ============================================================
//
// 【核心概念】
//   尾节点的 next 指向头节点，形成一个首尾相连的"环"。
//   从任意节点出发，沿着 next 一直走，最终会回到起点。
//
// 【循环链表的特点】
//   1. 没有真正的"尾"——每个节点都有后继
//   2. 判断是否遍历完一圈：current == first（回到起点）
//   3. 找尾节点：current->next == first
//   4. 遍历必须用 do-while（先执行一次再判断），因为初始时 current == first
//
// 【与普通单向链表的区别】
//   普通链表：最后一个节点 next = nullptr，遍历用 while(current)
//   循环链表：最后一个节点 next = first，遍历用 do-while 且判断 current != first
//
// 【空链表处理】
//   first == nullptr 表示空链表。
//   插入第一个节点时，需要将其 next 指向自己（单节点成环）。
//
// 【删除头节点的特殊处理】
//   需要先找到尾节点，更新其 next 指向新的头节点。
//   如果只有一个节点，删除后 first = nullptr。
//
// ============================================================

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
    Node* first;  // 头节点指针，空链表时为 nullptr

public:
    Circularlist() { first = nullptr; }

    // 尾插：找到尾节点（next == first），在其后插入新节点
    void insert(int val)
    {
        Node* newnode = new Node(val);
        if (!first)
        {
            first = newnode;
            newnode->next = first;  // 自己指向自己，形成单节点环
            return;
        }
        Node* current = first;
        while (current)
        {
            if (current->next == first)  // 找到了尾节点
            {
                current->next = newnode;  // 尾 → 新节点
                newnode->next = first;    // 新节点 → 头（保持循环）
                break;
            }
            else
            {
                current = current->next;
            }
        }
    }

    // 循环遍历打印：do-while 确保至少执行一次
    // 从 first 开始，沿 next 走到回到 first 为止
    void Print()
    {
        if (!first)
        {
            cout << "链表为空" << endl;
            return;
        }
        Node* current = first;
        do
        {
            cout << current->data << "->";
            current = current->next;
        } while (current != first);  // 回到起点时停止
        cout << endl;
    }

    // 按值删除
    void Delete(int val)
    {
        if (!first)
        {
            cout << "list is empty" << endl;
            return;
        }

        Node* current = first;
        Node* prev = nullptr;
        do
        {
            if (current->data == val)
            {
                if (current == first)
                {
                    // 情况 A：删除头节点

                    // 子情况 A1：只有一个节点
                    if (first->next == first)
                    {
                        delete first;
                        first = nullptr;
                        return;
                    }

                    // 子情况 A2：多个节点，需要找到尾节点更新其 next
                    Node* last = first;
                    while (last->next != first)
                    {
                        last = last->next;  // 找到尾节点
                    }
                    first = first->next;   // 头指针后移
                    last->next = first;    // 尾节点指向新头
                    delete current;
                }
                else
                {
                    // 情况 B：删除非头节点，前驱直接跳过当前节点
                    prev->next = current->next;
                    delete current;
                }
                return;
            }
            prev = current;
            current = current->next;
        } while (current != first);  // 已遍历一圈

        cout << "value not found" << endl;
    }
};
