// ============================================================
//        开放定址哈希表（Open Addressing / Linear Probing）
// ============================================================
//
// 【核心概念】
//   所有键直接存储在数组中（没有链表）。当发生冲突时，按一定
//   规则在数组中找下一个空位置存放。
//
//   本实现使用 线性探测（Linear Probing）：每次冲突就 +1，直到
//   找到空位为止。探测序列：pos, pos+1, pos+2, ...
//
//   "开放定址"的意思：键的存储位置是"开放的"——不仅限于哈希函数
//   算出的那个位置，而是整个数组范围内寻找。
//
// 【三种槽位状态】
//   Empty    — 从未存过数据（可插入）
//   Occupied — 有数据
//   Deleted  — 曾有数据但已删除（可插入，但查找时不能在此停止）
//
//   为什么需要 Deleted 状态？
//     如果删除时直接把状态改回 Empty，线性探测的查找链会在
//     该位置断开，导致后面可能存在的元素再也找不到了。
//
//   示例（简化的三槽表，插入 5%3=2, 8%3=2→3...实际上探测3个槽）：
//     [0]: Empty    [1]: Empty    [2]: 5(Occupied)
//     插入 8: 8%3=2 冲突 → 探测 (2+1)%3=0 → 插入 [0]
//     [0]: 8(Occ)   [1]: Empty    [2]: 5(Occ)
//     删除 5: [2] → Deleted
//     [0]: 8(Occ)   [1]: Empty    [2]: X(Deleted)
//     查找 8: 8%3=2 → Deleted(不是empty，继续) → (2+1)%3=0 → 找到 8 ✓
//     如果删除后是 Empty 而非 Deleted：
//       查找 8: 8%3=2 → Empty(以为找不到了！) → 错过 [0] 中的 8 ✗
//
// 【查找/插入/删除逻辑】
//   search(val):
//     从 hash(val) 开始，遇到 Empty 停止（确定不存在），
//     遇到 Deleted 时继续找（因为后面的槽可能有数据）。
//
//   insert(val):
//     从 hash(val) 开始，遇到 Empty 或 Deleted 就插入。
//     如果绕一圈回到起点（表满了），触发 rehash 扩容。
//
//   remove(val):
//     找到后改为 Deleted 状态，不能改 Empty。
//
// 【rehash（扩容）】
//   当表满时，创建 2 倍大的新表，将所有 Occupied 元素重新插入。
//   新表使用新的容量取模，元素会自动重新分布。
//
//   rehash 是 O(n) 操作，但发生频率低（只在满时触发），
//   均摊到每次插入仍然是 O(1)。
//
// 【与分离链接法的比较】
//   开放定址：内存连续，缓存友好，但对装载因子敏感，删除需标记
//   分离链接：可容忍高装载因子，删除简单，但有链表指针开销
//
// ============================================================

#include<iostream>
#include<vector>
using namespace std;

// 三种槽位状态
enum State { Empty, Occupied, Deleted };

struct tablestate
{
    int key;
    State state;
    tablestate() : state(Empty) {}  // 默认初始化为空
};

class hash
{
private:
    vector<tablestate> table;
    int currentSize;

    int myHash(int key)
    {
        return key % table.size();
    }

public:
    hash(int size = 10)
    {
        table.resize(size);
        currentSize = 0;
    }

    // 查找：从哈希位置开始线性探测，遇到 Empty 则不存在
    bool search(int val)
    {
        int pos = myHash(val);
        int startpos = pos;
        while (table[pos].state != Empty)  // Empty 标志查找结束
        {
            if (table[pos].state == Occupied && table[pos].key == val)
            {
                return true;  // 找到了
            }
            pos = (pos + 1) % table.size();  // 线性探测下一步
            if (pos == startpos)
            {
                break;  // 绕了一圈，表里全是 Deleted 或 Occupied
            }
        }
        return false;
    }

    // 插入：遇到空位或 Deleted 槽就放入，表满时触发 rehash
    void insert(int val)
    {
        if (search(val))
        {
            cout << "数据已经存在" << endl;
            return;
        }
        int pos = myHash(val);
        int startpos = pos;
        while (table[pos].state == Occupied)  // Occupied 才需要继续找
        {
            pos = (pos + 1) % table.size();
            if (pos == startpos)
            {
                // 绕了一圈——表满了，扩容后递归重新插入
                rehash();
                insert(val);
                return;
            }
        }
        // 找到 Empty 或 Deleted 槽位
        table[pos].key = val;
        table[pos].state = Occupied;
        currentSize++;
    }

    // 删除：标记为 Deleted，不能改回 Empty（否则会破坏查找链）
    void remove(int val)
    {
        int pos = myHash(val);
        int startpos = pos;
        while (table[pos].state != Empty)
        {
            if (table[pos].state == Occupied && table[pos].key == val)
            {
                table[pos].state = Deleted;  // 关键：懒删除标记
                currentSize--;
                cout << "已删除" << endl;
                return;
            }
            pos = (pos + 1) % table.size();
            if (pos == startpos)
            {
                break;
            }
        }
        cout << "表中不存在目标数" << endl;
    }

    // 扩容：将表大小翻倍，所有 Occupied 元素重新插入
    void rehash()
    {
        vector<tablestate> oldtable = table;
        table.resize(table.size() * 2);  // 新表大小为原来的 2 倍
        currentSize = 0;                  // insert 会重新计数
        for (int i = 0; i < oldtable.size(); i++)
        {
            if (oldtable[i].state == Occupied)
            {
                insert(oldtable[i].key);  // 用新表重新哈希
            }
        }
    }
};
