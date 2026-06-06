#include<iostream>
#include<algorithm>
using namespace std;
class Node
{
    public:
    int data;
    int npl;
    Node* Left;
    Node* Right;
    Node(int val):data(val),npl(0),Left(nullptr),Right(nullptr){}
};

class LeftistHeap
{
    private:
    Node* root;

    // 递归释放所有节点
    void clear(Node* node)
    {
        if (node == nullptr)
        {
            return;
        }
        clear(node->Left);
        clear(node->Right);
        delete node;
    }

    // 合并两棵左式堆，返回合并后的根
    Node* merge(Node* node1, Node* node2)
    {
        if(node1 == nullptr)
        {
            return node2;
        }
        if(node2 == nullptr)
        {
            return node1;
        }
        if(node1->data > node2->data)
        {
            swap(node1,node2);
        }

        node1->Right = merge(node1->Right,node2);

        if(node1->Left == nullptr || node1->Left->npl < node1->Right->npl)
        {
            swap(node1->Left,node1->Right);
        }

        if(node1->Right == nullptr)
        {
            node1->npl = 0;
        }
        else
        {
            node1->npl = node1->Right->npl+1;
        }
        return node1;
    }  


    public:
    LeftistHeap(): root(nullptr) {}
    ~LeftistHeap()
    {
        clear(root);
    }

    bool Isempty()
    {
        return root == nullptr;
    }

    int top()
    {
        if (root == nullptr)
        {
            cout << "heap is empty" << endl;
            return 0;
        }
        return root->data;
    }

    void push(int val)
    {
        Node* newnode = new Node(val);
        root = merge(root,newnode);
    }

    int pop()
    {
        if(root == nullptr)
        {
            cout<<"heap is empty"<<endl;
            return 0;
        }
        int Min = root->data;
        Node* oldroot = root;
        root = merge(oldroot->Left,oldroot->Right);
        delete oldroot;
        return Min;
    }
    
};