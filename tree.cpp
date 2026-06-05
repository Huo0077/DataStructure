// 二叉树：二叉搜索树 + 增强二叉树（查第 k 小）
// 支持：插入、删除、高度、前/中/后序递归遍历、层序遍历（BFS）
#include<iostream>
#include<algorithm>
#include<queue>
using namespace std;

class TreeNode
{
public:
    int data;
    TreeNode* Left;
    TreeNode* Right;
    TreeNode(int val): data(val), Left(nullptr), Right(nullptr) {}
};

class Tree
{
private:
    TreeNode* root;

    // ---------- 私有递归辅助函数 ----------

    int height(TreeNode* node)
    {
        if (node == nullptr)
        {
            return -1;
        }
        int left_height = height(node->Left);
        int right_height = height(node->Right);
        return max(left_height, right_height) + 1;
    }

    void PreorderHelper(TreeNode* node)
    {
        if (node == nullptr)
        {
            return;
        }
        cout << node->data << " ";
        PreorderHelper(node->Left);
        PreorderHelper(node->Right);
    }

    void InorderHelper(TreeNode* node)
    {
        if (node == nullptr)
        {
            return;
        }
        InorderHelper(node->Left);
        cout << node->data << " ";
        InorderHelper(node->Right);
    }

    void PostorderHelper(TreeNode* node)
    {
        if (node == nullptr)
        {
            return;
        }
        PostorderHelper(node->Left);
        PostorderHelper(node->Right);
        cout << node->data << " ";
    }

    // 插入辅助：向以 node 为根的 BST 中插入 val
    TreeNode* BSTinsert(TreeNode* node, int val)
    {
        if (node == nullptr)
        {
            return new TreeNode(val);
        }
        if (val < node->data)
        {
            node->Left = BSTinsert(node->Left, val);
        }
        else if (val > node->data)
        {
            node->Right = BSTinsert(node->Right, val);
        }
        return node;
    }

    // 找子树的最小值节点：一直向左走到底
    TreeNode* FindMin(TreeNode* node)
    {
        if (node->Left == nullptr)
        {
            return node;
        }
        return FindMin(node->Left);
    }

    // 删除辅助：删除以 node 为根的 BST 中值为 val 的节点
    TreeNode* BSTdelete(TreeNode* node, int val)
    {
        if (node == nullptr)
        {
            cout << val << " is not found" << endl;
            return nullptr;
        }
        if (node->data > val)
        {
            node->Left = BSTdelete(node->Left, val);
        }
        else if (node->data < val)
        {
            node->Right = BSTdelete(node->Right, val);
        }
        else
        {
            if (node->Left == nullptr)
            {
                TreeNode* temp = node->Right;
                delete node;
                return temp;
            }
            else if (node->Right == nullptr)
            {
                TreeNode* temp = node->Left;
                delete node;
                return temp;
            }
            else
            {
                TreeNode* Min = FindMin(node->Right);
                node->data = Min->data;
                node->Right = BSTdelete(node->Right, Min->data);
            }
        }
        return node;
    }

    // 中序遍历验证 BST：prev 指向前驱节点的 data，检查是否严格升序
    bool isBSTHelper(TreeNode* node, int*& prev)
    {
        if (node == nullptr)
        {
            return true;
        }
        if (!isBSTHelper(node->Left, prev))
        {
            return false;
        }
        if (prev != nullptr && node->data <= *prev)
        {
            return false;
        }
        prev = &node->data;
        return isBSTHelper(node->Right, prev);
    }

    void BFSHelper(TreeNode* node)
    {
        if (node == nullptr)
        {
            return;
        }
        queue<TreeNode*> q;
        q.push(node);
        int level = 0;
        while (!q.empty())
        {
            int levelSize = q.size();
            cout << "Level " << level << ": ";
            for (int i = 0; i < levelSize; i++)
            {
                TreeNode* temp = q.front();
                q.pop();
                cout << temp->data << " ";
                if (temp->Left != nullptr)
                {
                    q.push(temp->Left);
                }
                if (temp->Right != nullptr)
                {
                    q.push(temp->Right);
                }
            }
            cout << endl;
            level++;
        }
    }

public:
    Tree(): root(nullptr) {}

    // ---------- 公有接口 ----------

    void BSTInsert(int val)
    {
        root = BSTinsert(root, val);
    }

    void BSTDelete(int val)
    {
        root = BSTdelete(root, val);
    }

    // 判断是否为合法 BST（中序遍历严格升序）
    bool isBST()
    {
        int* prev = nullptr;
        return isBSTHelper(root, prev);
    }

    // 广度优先遍历（层序遍历）
    void BFS()
    {
        BFSHelper(root);
    }

    // 前序遍历：根 → 左 → 右
    void Preorder()
    {
        PreorderHelper(root);
        cout << endl;
    }

    // 中序遍历：左 → 根 → 右（BST 中序遍历结果有序）
    void Inorder()
    {
        InorderHelper(root);
        cout << endl;
    }

    // 后序遍历：左 → 右 → 根
    void Postorder()
    {
        PostorderHelper(root);
        cout << endl;
    }

    int height()
    {
        return height(root);
    }
};


// 增强二叉树节点：额外记录左子树节点数 leftSize，用于 O(log n) 查第 k 小
class AugmentedNode
{
public:
    int info;
    int leftSize;
    AugmentedNode* left;
    AugmentedNode* right;
    AugmentedNode(int val, int lSize = 0): info(val), leftSize(lSize), left(nullptr), right(nullptr) {}
};

// 在增强 BST 中查找第 k 小的元素
// currentRank = leftSize + 1 即为当前节点在子树中的排名
// k == currentRank → 命中；k < currentRank → 去左子树；k > currentRank → 去右子树
AugmentedNode* findKth(AugmentedNode* root, int k)
{
    if (root == nullptr)
    {
        return nullptr;
    }
    int currentRank = root->leftSize + 1;
    if (k == currentRank)
    {
        return root;
    }
    else if (k < currentRank)
    {
        return findKth(root->left, k);
    }
    else
    {
        return findKth(root->right, k - currentRank);
    }
}