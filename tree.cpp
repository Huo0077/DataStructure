#include<iostream>
#include<algorithm>
using namespace std;
class TreeNode
{
    public:
    int data;
    TreeNode* Left;
    TreeNode* Right;
    TreeNode(int val):data(val),Left(nullptr),Right(nullptr){}

};

class Tree
{
    private:
    TreeNode* root;

    int height(TreeNode* node)
    {
        if(node == nullptr)
            return -1;
        int left_height = height(node->Left);
        int right_height = height(node->Right);
        return max(left_height, right_height) + 1;
    }

    public:
    Tree(): root(nullptr) {}

    int height()
    {
        return height(root);
    }
};