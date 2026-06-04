// 双向链表：每个节点有 prev 和 next 两个指针
// 支持：增删查、计数、升序/降序插入、尾插、遍历打印
#include<iostream>
using namespace std;

// 链表节点
class Node
{
public:
    int data;
    Node* next;
    Node* prev;
    Node(int d): data(d), next(nullptr), prev(nullptr) {}
};

class Linkedlist
{
private:
    Node* first;  // 头节点指针
public:
    Linkedlist(): first(nullptr) {}         // 创建空链表
    Linkedlist(int n) { first = new Node(n); }  // 创建一个含初始值的链表
    ~Linkedlist()
    {
        // 遍历释放所有节点
        Node* current = first;
        while (current)
        {
            Node* tmp = current;
            current = current->next;
            delete tmp;
        }
    }

    // 返回链表节点个数
    int Count()
    {
        int result = 0;
        Node* current = first;
        while (current)
        {
            result++;
            current = current->next;
        }
        return result;
    }

    // 升序插入：找到第一个比 val 大的位置，插在它前面
    void AscendingInsert(int val)
    {
        Node* newNode = new Node(val);
        if (!first)
        {
            first = newNode;
            return;
        }
        Node* current = first;
        while (current)
        {
            if (current->data <= val)
            {
                if (current->next)
                    current = current->next;
                else
                {
                    // 已到尾部，插入末尾
                    current->next = newNode;
                    newNode->prev = current;
                    return;
                }
            }
            else
            {
                // 在 current 之前插入
                Node* pre = current->prev;
                newNode->next = current;
                newNode->prev = pre;
                current->prev = newNode;
                if (pre)
                    pre->next = newNode;
                else
                    first = newNode;  // 插入到头部
                return;
            }
        }
    }

    // 降序插入：找到第一个比 val 小的位置，插在它前面
    void DescendingInsert(int val)
    {
        Node* newNode = new Node(val);
        if (!first)
        {
            first = newNode;
            return;
        }
        Node* current = first;
        while (current)
        {
            if (current->data >= val)
            {
                if (current->next)
                    current = current->next;
                else
                {
                    current->next = newNode;
                    newNode->prev = current;
                    return;
                }
            }
            else
            {
                Node* pre = current->prev;
                newNode->next = current;
                newNode->prev = pre;
                current->prev = newNode;
                if (pre)
                    pre->next = newNode;
                else
                    first = newNode;
                return;
            }
        }
    }

    // 尾插：遍历到链表末尾插入
    void Insert(int val)
    {
        Node *newnode = new Node(val);
        if(!first)
        {
            first = newnode;
            return;
        }
        Node* current = first;
        while(current)
        {
            if(current->next)
                current = current->next;
            else
            {
                current->next = newnode;
                newnode->prev = current;
                break;
            }
        }
    }

    // 按值删除：找到第一个匹配的节点并删除
    void Delete(int val)
    {
        if (!first)
        {
            cout << "list is empty" << endl;
            return;
        }
        Node* current = first;
        while (current)
        {
            if (current->data == val)
            {
                // 处理前驱指针
                if (current->prev)
                    current->prev->next = current->next;
                else
                    first = current->next;  // 删除头节点，更新 first
                // 处理后继指针
                if (current->next)
                    current->next->prev = current->prev;
                delete current;
                return;
            }
            current = current->next;
        }
        cout << "value not found" << endl;
    }

    // 查找：是否存在值为 val 的节点
    bool Search(int val)
    {
        Node* current = first;
        while (current)
        {
            if (current->data == val)
                return true;
            current = current->next;
        }
        return false;
    }

    // 遍历打印所有节点
    void Display()
    {
        Node* current = first;
        while (current)
        {
            cout << current->data << " ";
            current = current->next;
        }
        cout << endl;
    }
};