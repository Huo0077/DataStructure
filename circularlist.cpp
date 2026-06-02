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
    Node* first;
    public:
    Circularlist(){first = nullptr;}

    void insert(int val)
    {
        Node* newnode = new Node(val);
        if(!first)
        {
            first = newnode;
            newnode->next = first;
            return;
        }
        Node* current = first;
        while(current)
        {
            if(current->next == first)
            {
                current->next = newnode;
                newnode->next = first;
                break;
            }
            else
            {
                current = current->next;
            }
        }
    }

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
                    if (first->next == first)
                    {
                        delete first;
                        first = nullptr;
                        return;
                    }
                    Node* last = first;
                    while (last->next != first)
                    last = last->next;
                    first = first->next;
                    last->next = first;
                    delete current;
                }
                else
                {
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