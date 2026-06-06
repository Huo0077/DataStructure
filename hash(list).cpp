// 哈希表：分离链接法处理冲突，使用 vector<list<int>> 实现
#include<iostream>
#include<vector>
#include<list>
using namespace std;

class HashTable
{
    private:
    vector<list<int>> table;
    int currentSize;

    int myHash(int key)
    {
        return key % table.size();
    }

    public:
    HashTable(int size = 10)
    {
        table.resize(size);
        currentSize = 0;
    }

    // 插入
    void insert(int val)
    {
        int step = myHash(val);
        // 检查是否已存在（去重）
        for (int x : table[step])
        {
            if (x == val)
            {
                return;
            }
        }
        table[step].push_back(val);
        currentSize++;
    }

    // 查找
    bool search(int val)
    {
        int step = myHash(val);
        for (int x : table[step])
        {
            if (x == val)
            {
                return true;
            }
        }
        return false;
    }

    // 删除
    void remove(int val)
    {
        int step = myHash(val);
        table[step].remove(val);
        currentSize--;
    }

    int size()
    {
        return currentSize;
    }
};