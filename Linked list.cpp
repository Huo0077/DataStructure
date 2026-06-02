#include<iostream>
using namespace std;

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
    Node* first;
public:
    Linkedlist(): first(nullptr) {}
    Linkedlist(int n) { first = new Node(n); }
    ~Linkedlist()
    {
        Node* current = first;
        while (current)
        {
            Node* tmp = current;
            current = current->next;
            delete tmp;
        }
    }
     
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
            {
                current = current->next;
            }
            else
            {
                current->next = newnode;
                newnode->prev = current;
                break;
            }
        }
    }

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
                if (current->prev)
                    current->prev->next = current->next;
                else
                    first = current->next;
                if (current->next)
                    current->next->prev = current->prev;
                delete current;
                return;
            }
            current = current->next;
        }
        cout << "value not found" << endl;
    }

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
