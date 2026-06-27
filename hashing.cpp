// ============================================================
//     哈希表合集：分离链接 / 开放定址 / 完美哈希（FKS）
// ============================================================
//
// 本文件涵盖"哈希（散列）"知识点的所有实现：
//   Part 1: 分离链接哈希表 (Separate Chaining)
//   Part 2: 开放定址哈希表 (Open Addressing / Linear Probing)
//   Part 3: 完美哈希表 (Perfect Hashing / FKS 两级方案)
//
// ============================================================

#include<iostream>
#include<vector>
#include<list>
#include<cstdlib>
#include<ctime>
using namespace std;

// ============================================================
//   Part 1: 分离链接哈希表（Separate Chaining）
// ============================================================
//
// 【核心概念】
//   每个桶挂一个链表，冲突的键存在同一桶的链表中。
//   查找时先定位桶（O(1) 哈希），再遍历链表。
//
// 【装载因子 λ = n/m】
//   分离链接法可容忍 λ > 1，一般建议 λ < 1。
//   查找/插入/删除 平均 O(1 + λ) ≈ O(1)。
//
// 【与开放定址对比】
//   分离链接：链表存冲突元素，装载因子可 >1，删除简单
//   开放定址：所有元素在数组中，冲突时找下一个空位，删除需懒标记
// ============================================================

class HashTable
{
private:
    vector<list<int>> table;
    int currentSize;

    int myHash(int key) { return key % table.size(); }

public:
    HashTable(int size = 10) : currentSize(0) { table.resize(size); }

    // insert：用哈希定位桶，遍历链表检查重复，无重复则加入。
    void insert(int val)
    {
        int step = myHash(val);
        for (int x : table[step])
            if (x == val) return;       // 已存在，不重复
        table[step].push_back(val);
        currentSize++;
    }

    // search：定位桶后遍历链表查找。平均 O(1+λ) ≈ O(1)。
    bool search(int val)
    {
        int step = myHash(val);
        for (int x : table[step])
            if (x == val) return true;
        return false;
    }

    // remove：定位桶后用 list::remove 删除。分离链接删除最简单——
    // 直接改链表指针即可，不需要懒标记。
    void remove(int val)
    {
        int step = myHash(val);
        table[step].remove(val);
        currentSize--;
    }

    int size() { return currentSize; }
};

// ============================================================
//   Part 2: 开放定址哈希表（Open Addressing / Linear Probing）
// ============================================================
//
// 【核心概念】
//   所有键直接存在数组中，冲突时按规则找下一个空位。
//   本实现使用 线性探测：pos, pos+1, pos+2, ...
//
// 【三种槽位状态】
//   Empty    — 从未存过数据
//   Occupied — 有数据
//   Deleted  — 曾有数据但已删除（懒删除标记）
//
//   为什么需要 Deleted？
//     如果删除时直接改 Empty，线性探测的查找链会断开，
//     导致后面可能存在的元素找不到了。
//
//   示例：
//     [0]:8(Occ)  [1]:Empty  [2]:X(Deleted)
//     查找8: 8%3=2→Deleted(不是empty,继续)→(2+1)%3=0→找到✓
//     若删除后是Empty: 8%3=2→Empty→以为不存在→错过[0]中的8✗
//
// 【rehash 扩容】
//   表满时创建 2x 新表，所有 Occupied 元素重新插入。
//   O(n) 但均摊 O(1)。
// ============================================================

enum State { Empty, Occupied, Deleted };

struct TableSlot
{
    int key;
    State state;
    TableSlot() : state(Empty) {}
};

class OpenHash
{
private:
    vector<TableSlot> table;
    int currentSize;

    int myHash(int key) { return key % table.size(); }

public:
    OpenHash(int size = 10) : currentSize(0) { table.resize(size); }

    bool search(int val)
    {
        int pos = myHash(val), start = pos;
        while (table[pos].state != Empty)
        {
            if (table[pos].state == Occupied && table[pos].key == val)
                return true;
            pos = (pos + 1) % table.size();
            if (pos == start) break;    // 绕了一圈
        }
        return false;
    }

    void rehash()
    {
        vector<TableSlot> old = table;
        table.resize(table.size() * 2);
        currentSize = 0;
        for (auto& slot : old)
            if (slot.state == Occupied) insert(slot.key);
    }

    void insert(int val)
    {
        if (search(val)) { cout << "数据已存在" << endl; return; }
        int pos = myHash(val), start = pos;
        while (table[pos].state == Occupied)
        {
            pos = (pos + 1) % table.size();
            if (pos == start) { rehash(); insert(val); return; }
        }
        table[pos].key = val;
        table[pos].state = Occupied;
        currentSize++;
    }

    void remove(int val)
    {
        int pos = myHash(val), start = pos;
        while (table[pos].state != Empty)
        {
            if (table[pos].state == Occupied && table[pos].key == val)
            {
                table[pos].state = Deleted;  // 懒删除，不能改 Empty
                currentSize--;
                cout << "已删除" << endl;
                return;
            }
            pos = (pos + 1) % table.size();
            if (pos == start) break;
        }
        cout << "表中不存在目标数" << endl;
    }
};

// ============================================================
//   Part 2b: 二次探测（Quadratic Probing）
// ============================================================
//
// 【探测序列】
//   pos, pos+1², pos+2², pos+3², ...
//   即 h(key) + i² (mod tableSize), i = 0, 1, 2, ...
//
// 【与线性探测的区别】
//   线性探测：pos, pos+1, pos+2...  步长固定为1 → "一次聚集"
//   二次探测：pos, pos+1², pos+2²...  步长越来越大 → 跳跃分散
//
// 【注意事项】
//   表大小应为素数且装载因子 < 0.5，否则可能找不到空位。
//   同样需要 Deleted 标记维护查找链。
// ============================================================

class QuadraticHash
{
private:
    vector<TableSlot> table;
    int currentSize;

    int myHash(int key) { return key % table.size(); }

public:
    QuadraticHash(int size = 11) : currentSize(0) { table.resize(size); }

    bool search(int val)
    {
        int pos = myHash(val);
        for (size_t i = 0; i < table.size(); i++)
        {
            int cur = (pos + i * i) % table.size();
            if (table[cur].state == Empty) return false;
            if (table[cur].state == Occupied && table[cur].key == val)
                return true;
        }
        return false;
    }

    void insert(int val)
    {
        if (search(val)) { cout << "数据已存在" << endl; return; }
        int pos = myHash(val);
        for (size_t i = 0; i < table.size(); i++)
        {
            int cur = (pos + i * i) % table.size();
            if (table[cur].state != Occupied)
            {
                table[cur].key = val;
                table[cur].state = Occupied;
                currentSize++;
                return;
            }
        }
        cout << "表满或无法插入" << endl;
    }

    void remove(int val)
    {
        int pos = myHash(val);
        for (size_t i = 0; i < table.size(); i++)
        {
            int cur = (pos + i * i) % table.size();
            if (table[cur].state == Empty) break;
            if (table[cur].state == Occupied && table[cur].key == val)
            {
                table[cur].state = Deleted;
                currentSize--;
                cout << "已删除" << endl;
                return;
            }
        }
        cout << "表中不存在目标数" << endl;
    }
};

// ============================================================
//   Part 2c: 双重散列（Double Hashing）
// ============================================================
//
// 【探测序列】
//   pos, pos+step, pos+2*step, ...  其中 step = hash2(key)
//
// 【两个哈希函数】
//   h1(key) = key % tableSize           —— 起始位置
//   h2(key) = prime - (key % prime)     —— 步长（必须 ≠ 0）
//   探测公式：h(key, i) = (h1(key) + i×h2(key)) % tableSize
//
// 【为什么 h2 不能为 0？】
//   步长为 0 → 永远停在原位置，无法探测其他槽。
//
// 【为什么 h2 与 tableSize 互质？】
//   不互质则探测序列可能只覆盖部分槽，取素数可保证全覆盖。
//
// 【优势】最好的分散性
//   两个键 h1 冲突但 h2 可能不同 → 探测路径不同，
//   极大缓解一次聚集和二次聚集。
//   双重散列是开放定址法中"最接近均匀分布"的策略。
//
// 【计算示例】tableSize=7, prime=5
//   插入 key=10: h1=3, h2=5-(10%5)=5 → 探测: 3,1,6,4,...
//   插入 key=17: h1=3, h2=5-(17%5)=3 → 探测: 3,6,2,5,...
//   两键在槽3冲突但步长不同(5 vs 3)，路径立即分叉！
// ============================================================

class DoubleHash
{
private:
    vector<TableSlot> table;
    int currentSize;
    int prime;  // h2(key) = prime - (key % prime)

    int h1(int key) { return key % table.size(); }
    int h2(int key) { return prime - (key % prime); }

public:
    DoubleHash(int size = 7, int p = 5) : currentSize(0), prime(p)
    {
        table.resize(size);
    }

    bool search(int val)
    {
        int pos = h1(val), step = h2(val);
        for (size_t i = 0; i < table.size(); i++)
        {
            int cur = (pos + i * step) % table.size();
            if (table[cur].state == Empty) return false;
            if (table[cur].state == Occupied && table[cur].key == val)
                return true;
        }
        return false;
    }

    void insert(int val)
    {
        if (search(val)) { cout << "数据已存在" << endl; return; }
        int pos = h1(val), step = h2(val);
        for (size_t i = 0; i < table.size(); i++)
        {
            int cur = (pos + i * step) % table.size();
            if (table[cur].state != Occupied)
            {
                table[cur].key = val;
                table[cur].state = Occupied;
                currentSize++;
                return;
            }
        }
        cout << "表满" << endl;
    }

    void remove(int val)
    {
        int pos = h1(val), step = h2(val);
        for (size_t i = 0; i < table.size(); i++)
        {
            int cur = (pos + i * step) % table.size();
            if (table[cur].state == Empty) break;
            if (table[cur].state == Occupied && table[cur].key == val)
            {
                table[cur].state = Deleted;
                currentSize--;
                cout << "已删除" << endl;
                return;
            }
        }
        cout << "表中不存在目标数" << endl;
    }
};

// ============================================================
//   辅助函数：判断素数 & 找下一个素数
// ============================================================

bool isPrime(int n)
{
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++)
        if (n % i == 0) return false;
    return true;
}

int nextPrime(int n)
{
    while (!isPrime(n)) n++;
    return n;
}

// ============================================================
//   Part 3: 完美哈希表（FKS 两级哈希方案）
// ============================================================
//
// 【核心概念】
//   完美哈希保证：查找在最坏情况下也是 O(1)，且没有任何冲突。
//   代价：键集合必须提前已知（静态），构建时间较长。
//
// 【两级结构】
//   第一级：n 个键 → n 个桶（全域哈希 h1）
//   第二级：桶 i 有 b_i 个键 → 大小为 b_i² 的子表（无冲突哈希 h2_i）
//
//   为什么总空间 O(n)？
//     Σ(b_i²) ≤ 4n（随机选 h1，期望约 2 次尝试满足此不等式）
//
// 【全域哈希函数族】
//   h(x) = ((a·x + b) mod p) mod m
//   p 为大于所有键的素数，a∈[1,p-1], b∈[0,p-1]
//   对任意 x≠y，冲突概率 Pr[h(x)=h(y)] ≤ 1/m
//
// 【查找过程】
//   search(key):
//     1. bucket = h1(key)           → 1 次哈希
//     2. slot = h2_bucket(key)      → 1 次哈希
//     3. table[bucket][slot] == key → 1 次数组访问
//   = O(1) 最坏情况，绝无冲突
// ============================================================

class PerfectHash
{
private:
    int n, prime;
    int a1, b1;                           // 第一级哈希参数
    vector<int> bucketSize;               // bucketSize[i] = b_i²
    vector<int> a2, b2;                   // 第二级哈希参数
    vector<vector<int>> table;            // 第二级子表

    int universalHash(int key, int a, int b, int m)
    {
        return (int)(((long long)a * key + b) % prime % m);
    }

public:
    PerfectHash(vector<int>& keys);
    bool search(int key);
    void display();
};

PerfectHash::PerfectHash(vector<int>& keys)
{
    n = keys.size();
    if (n == 0) return;
    srand((unsigned)time(0));

    // 第一步：找到大于所有键的素数 p
    int maxKey = keys[0];
    for (int k : keys) if (k > maxKey) maxKey = k;
    prime = nextPrime(maxKey + 1);

    // 第二步：为第一级哈希找参数 (a1,b1)，使得 Σ(b_i²) ≤ 4n
    vector<int> bucketCount(n, 0);
    long long sumSquares;
    do
    {
        a1 = rand() % (prime - 1) + 1;
        b1 = rand() % prime;
        fill(bucketCount.begin(), bucketCount.end(), 0);
        for (int key : keys)
            bucketCount[universalHash(key, a1, b1, n)]++;
        sumSquares = 0;
        for (int c : bucketCount) sumSquares += (long long)c * c;
    } while (sumSquares > 4 * n);

    // 第三步：为每个非空桶构建第二级无冲突哈希表
    bucketSize.resize(n);
    a2.resize(n);
    b2.resize(n);
    table.resize(n);

    for (int i = 0; i < n; i++)
    {
        int bi = bucketCount[i];
        if (bi == 0) { bucketSize[i] = 0; continue; }

        int sz = bi * bi;
        bucketSize[i] = sz;
        table[i].resize(sz, -1);

        // 收集该桶的键
        vector<int> bucketKeys;
        for (int key : keys)
            if (universalHash(key, a1, b1, n) == i)
                bucketKeys.push_back(key);

        // 随机试第二级哈希参数直到无冲突
        bool ok = false;
        while (!ok)
        {
            a2[i] = rand() % (prime - 1) + 1;
            b2[i] = rand() % prime;
            fill(table[i].begin(), table[i].end(), -1);
            ok = true;
            for (int key : bucketKeys)
            {
                int slot = universalHash(key, a2[i], b2[i], sz);
                if (table[i][slot] != -1) { ok = false; break; }
                table[i][slot] = key;
            }
        }
    }
}

bool PerfectHash::search(int key)
{
    if (n == 0) return false;
    int bucket = universalHash(key, a1, b1, n);
    if (bucketSize[bucket] == 0) return false;
    int slot = universalHash(key, a2[bucket], b2[bucket], bucketSize[bucket]);
    return table[bucket][slot] == key;
}

void PerfectHash::display()
{
    cout << "========== 完美哈希表（FKS 两级方案） ==========" << endl;
    cout << "键个数 n = " << n << ", 素数 p = " << prime << endl;
    cout << "h1(x) = ((" << a1 << "*x + " << b1 << ") % " << prime << ") % " << n << endl;

    int totalSlots = n, usedBuckets = 0;
    for (int i = 0; i < n; i++)
    {
        if (bucketSize[i] > 0)
        {
            usedBuckets++;
            totalSlots += bucketSize[i];
            cout << "  桶" << i << " (" << bucketSize[i] << "槽): ";
            cout << "h2(x)=(((" << a2[i] << "*x+" << b2[i] << ")%" << prime << ")%" << bucketSize[i] << ")  ";
            for (int j = 0; j < bucketSize[i]; j++)
                cout << (table[i][j] == -1 ? "· " : to_string(table[i][j]) + " ");
            cout << endl;
        }
    }
    cout << "非空桶: " << usedBuckets << "/" << n
         << ", 总空间: " << totalSlots
         << ", 装载因子: " << (double)n / totalSlots << endl;
}

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║     哈 希 表 综 合 测 试         ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    // ---- Part 1: 分离链接哈希表 ----
    cout << "\n┌─ Part 1: 分离链接哈希表 ─────────┐" << endl;
    HashTable ht(5);
    ht.insert(10); ht.insert(5); ht.insert(15); ht.insert(7);
    cout << "search(10)=" << ht.search(10) << " (expect 1)" << endl;
    cout << "search(99)=" << ht.search(99) << " (expect 0)" << endl;
    ht.remove(10);
    cout << "after remove, search(10)=" << ht.search(10) << " (expect 0)" << endl;
    cout << "size=" << ht.size() << " (expect 3)" << endl;

    // ---- Part 2a: 线性探测 ----
    cout << "\n┌─ Part 2a: 线性探测 ──────────────┐" << endl;
    OpenHash oh(5);
    oh.insert(10); oh.insert(5); oh.insert(15);
    cout << "search(10)=" << oh.search(10) << " (expect 1)" << endl;
    oh.remove(10);
    cout << "after remove, search(10)=" << oh.search(10) << " (expect 0)" << endl;
    cout << "search(15)=" << oh.search(15) << " (expect 1, Deleted不破坏查找链)" << endl;

    // ---- Part 2b: 二次探测 ----
    cout << "\n┌─ Part 2b: 二次探测 ──────────────┐" << endl;
    QuadraticHash qh(11);
    qh.insert(10); qh.insert(21); qh.insert(32);  // 10%11=10, 21%11=10冲突→(10+1)%11=0, 32%11=10冲突→(10+1)=0冲突→(10+4)%11=3
    cout << "search(10)=" << qh.search(10) << " (expect 1)" << endl;
    cout << "search(21)=" << qh.search(21) << " (expect 1)" << endl;
    cout << "search(32)=" << qh.search(32) << " (expect 1, 跳过冲突用i²步长)" << endl;
    qh.remove(21);
    cout << "after remove(21), search(32)=" << qh.search(32) << " (expect 1, Deleted保查找链)" << endl;

    // ---- Part 2c: 双重散列 ----
    cout << "\n┌─ Part 2c: 双重散列 ──────────────┐" << endl;
    DoubleHash dh(7, 5);
    dh.insert(10); dh.insert(17);  // 10%7=3, 17%7=3冲突→h2(17)=5-(17%5)=3→探测(3+3)%7=6
    cout << "search(10)=" << dh.search(10) << " (expect 1)" << endl;
    cout << "search(17)=" << dh.search(17) << " (expect 1, 双重散列步长=3)" << endl;
    dh.remove(10);
    cout << "after remove(10), search(17)=" << dh.search(17) << " (expect 1)" << endl;

    // ---- Part 3: 完美哈希表 ----
    cout << "\n┌─ Part 3: 完美哈希表（FKS）───────┐" << endl;
    vector<int> keys = {10, 22, 31, 4, 15, 28, 17, 88, 59};
    PerfectHash ph(keys);
    ph.display();
    cout << "\n查找测试:" << endl;
    for (int k : {10, 22, 99, 88, 4})
        cout << "  search(" << k << ")=" << ph.search(k) << endl;

    cout << "\n所有测试完成！" << endl;
    return 0;
}
