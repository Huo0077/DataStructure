// ============================================================
//            分离链接哈希表（Separate Chaining Hash Table）
// ============================================================
//
// 【核心概念】
//   解决哈希冲突的第一种方法：每个桶挂一个链表（或 list）。
//   冲突的键存在同一个桶的链表中。
//
// 【数据结构】
//   vector<list<int>> table;
//   table[i] 是一个链表（std::list），存储所有哈希到桶 i 的键。
//
//   图示（table.size() = 5）：
//     [0] → 10 → 5 → nullptr        (10%5=0, 5%5=0)
//     [1] → nullptr                   (空桶)
//     [2] → 7 → nullptr              (7%5=2)
//     [3] → 8 → 13 → nullptr         (8%5=3, 13%5=3)
//     [4] → 9 → nullptr              (9%5=4)
//
// 【冲突处理】
//   所有映射到同一桶的键串在链表中。查找时先定位桶，再遍历链表。
//   链表越长，查找越慢 → 需要保持"装载因子"不要太大。
//
// 【装载因子（Load Factor）】
//   λ = n / m，其中 n = 元素个数，m = 桶的数量。
//   分离链接法中 λ 可以大于 1（多个元素共享一个桶）。
//   一般建议 λ < 1（平均每个桶不到一个元素）。
//
// 【时间复杂度】
//   查找/插入/删除的平均时间复杂度 = O(1 + λ)
//   其中 λ 是链表平均长度。如果 λ 保持在常数范围，就是 O(1)。
//
//   STL 的 std::list::remove 是 O(n_list) 的，
//   但 list 中元素很少时可以看作 O(1) 均摊。
//
// 【与开放定址法的比较】
//   分离链接：链表存冲突元素，装载因子可 >1，删除简单
//   开放定址：所有元素在数组中，冲突时找下一个空位，删除需要标记
//
// ============================================================

#include<iostream>
#include<vector>
#include<list>
using namespace std;

class HashTable
{
private:
    vector<list<int>> table;  // 桶数组，每个桶是一个链表
    int currentSize;          // 当前存储的键的数量

    // 哈希函数：取模法
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

    // 插入：先检查是否已存在（去重），否则加入桶的链表
    void insert(int val)
    {
        int step = myHash(val);
        // 查重：遍历桶中的链表
        for (int x : table[step])
        {
            if (x == val)
            {
                return;  // 已存在，不重复插入
            }
        }
        table[step].push_back(val);  // 挂入链表尾部
        currentSize++;
    }

    // 查找：定位桶后遍历链表
    bool search(int val)
    {
        int step = myHash(val);
        for (int x : table[step])  // range-for 遍历链表
        {
            if (x == val)
            {
                return true;
            }
        }
        return false;
    }

    // 删除：利用 std::list::remove 从链表中移除
    void remove(int val)
    {
        int step = myHash(val);
        table[step].remove(val);  // STL list 的按值删除
        currentSize--;
    }

    int size()
    {
        return currentSize;
    }
};
