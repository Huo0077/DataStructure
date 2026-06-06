#include<iostream>
#include<vector>
#include<cstdlib>
#include<ctime>
using namespace std;

// ============================================================
// 判断一个数是否为素数
// ============================================================
bool isPrime(int n)
{
    if (n < 2)
    {
        return false;
    }
    for (int i = 2; i * i <= n; i++)
    {
        if (n % i == 0)
        {
            return false;
        }
    }
    return true;
}

// ============================================================
// 找到 >= n 的最小素数
// ============================================================
int nextPrime(int n)
{
    while (!isPrime(n))
    {
        n++;
    }
    return n;
}

// ============================================================
//               完美哈希表（FKS 两级哈希方案）
// ============================================================
//
// 【核心概念】
//
//   什么是"完美哈希"？
//   ─────────────────
//   完美哈希保证：查找操作在 最坏情况 下也是 O(1)，且没有任何冲突。
//
//   回顾普通哈希表（如 hash(arr).cpp 中的线性探测）：
//     - 平均情况 O(1)，但最坏情况所有 key 映射到同一槽位
//     - 查找退化为 O(n)，需要逐个探测
//
//   完美哈希解决了这个"最坏情况退化为 O(n)"的问题。
//   代价是：键集合必须提前已知（静态），构建时间较长。
//
//
//   【两级结构】
//   ──────────────
//
//   第一级：n 个键 → n 个桶
//     用全域哈希函数 h1 将键分到 n 个桶中。
//     每个桶 i 包含 b_i 个键（有些桶可能为空）。
//
//   第二级：每个桶 i → 一张大小为 b_i² 的子表
//     为该桶单独找一个 无冲突 的哈希函数 h2_i，
//     将桶内的 b_i 个键完美地映射到 b_i² 个槽位中。
//
//   直观理解：
//     桶越大（b_i 大），留给它的槽位越多（b_i²），
//     槽位数是元素数的平方，找到无冲突函数的概率非常高。
//
//   图示（n=9）：
//     第一级（9个桶）          第二级（每桶 b_i² 个槽）
//     ┌───┬───┬───┬───┬───┬───┬───┬───┬───┐
//     │ 0 │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │   ← n 个桶
//     └─┬─┴───┴─┬─┴───┴───┴─┬─┴───┴───┴───┘
//       │       │           │
//       ▼       ▼           ▼
//     ┌─┐     ┌─┬─┬─┐     ┌─┐
//     │1│     │ │ │ │     │1│          ← 大小为 b_i² 的子表
//     └─┘     └─┴─┴─┘     └─┘
//    b₀=1    b₂=2        b₅=1
//    size=1  size=4      size=1
//
//
//   【为什么总空间是 O(n)？】
//   ──────────────────────────
//   关键在于随机选择第一级哈希参数，使得：
//
//       Σ (b_i²) ≤ 4n
//
//   其中 b_i 是桶 i 中的元素个数。
//
//   这个不等式保证所有第二级子表的总大小 ≤ 4n，
//   加上第一级的 n 个桶指针，总空间为 O(n)。
//
//   理论依据（来自 FKS 论文）：
//     从全域哈希函数族中随机选 h1，
//     E[Σ b_i²] < 2n（期望值小于 2n）。
//     由马尔可夫不等式，Σ b_i² ≤ 4n 的概率 ≥ 1/2，
//     所以期望只需重试约 2 次就能找到合适的 h1。
//
//
//   【全域哈希函数族】
//   ──────────────────
//   函数形式：
//       h(x) = ((a·x + b) mod p) mod m
//
//   其中：
//     p —— 一个大于所有键的素数
//     a —— [1, p-1] 中的随机整数
//     b —— [0, p-1] 中的随机整数
//     m —— 表的大小（第一级中 m=n，第二级中 m=b_i²）
//
//   为什么叫"全域"（Universal）？
//     对于任意两个不同的键 x ≠ y：
//       Pr[ h(x) = h(y) ] ≤ 1/m
//     冲突概率与随机均匀分布相同，这是哈希理论中的重要性质。
//
//   分两步取模的原因：
//     ((a·x + b) mod p)  这一步将键映射到 [0, p-1]，分布均匀（数学保证）
//     (... ) % m          这一步将 [0, p-1] 映射到 [0, m-1]
//     两步结合保证最终分布接近均匀。
//
//
//   【查找过程】
//   ────────────
//   search(key):
//     1. bucket = h1(key)           → 确定在哪个桶（1 次哈希）
//     2. slot   = h2_bucket(key)    → 在桶内定位（1 次哈希）
//     3. 检查 table[bucket][slot] == key
//
//   总共只需要 2 次哈希 + 1 次数组访问 = O(1)，绝无冲突！
//
// ============================================================

class PerfectHash
{
private:
    int n;          // 键的总数
    int prime;      // 大于所有键的素数，全域哈希的核心参数
    int a1, b1;     // 第一级哈希参数: h1(x) = ((a1·x + b1) % p) % n

    vector<int> bucketSize;     // bucketSize[i] = 桶 i 的第二级表大小（即 b_i²）
    vector<int> a2, b2;         // 每个桶的第二级哈希参数: h2_i(x) = ((a2[i]·x + b2[i]) % p) % bucketSize[i]
    vector<vector<int>> table;  // 第二级哈希表：table[i] 是桶 i 的子表，初始化为 -1（空）

    // ----------------------------------------------------------
    // 全域哈希函数：h(x) = ((a·x + b) mod p) mod m
    // 使用 long long 防止 a*x 的中间结果溢出 int
    // ----------------------------------------------------------
    int universalHash(int key, int a, int b, int m)
    {
        return (int)(((long long)a * key + b) % prime % m);
    }

public:
    // 构造函数：传入 静态键集合，构建完美哈希表
    PerfectHash(vector<int>& keys);

    // 查找：O(1) 最坏情况，绝无冲突
    bool search(int key);

    // 打印内部结构（用于学习和调试）
    void display();
};

// ============================================================
// 构造函数：构建完美哈希表
// ============================================================
PerfectHash::PerfectHash(vector<int>& keys)
{
    n = keys.size();
    if (n == 0)
    {
        return;
    }

    srand((unsigned)time(0));

    // ----------------------------------------------------------
    // 第一步：找到一个大于所有键的素数 p
    //
    // 为什么需要素数？
    //   全域哈希 ((a·x + b) mod p) 需要 p 为素数才能保证
    //   "均匀分布"的数学性质。如果 p 不是素数，取模运算
    //   会产生周期性偏差，破坏全域性质。
    // ----------------------------------------------------------
    int maxKey = keys[0];
    for (int i = 1; i < n; i++)
    {
        if (keys[i] > maxKey)
        {
            maxKey = keys[i];
        }
    }
    prime = nextPrime(maxKey + 1);

    // ----------------------------------------------------------
    // 第二步：为第一级哈希找参数 (a1, b1)
    //
    // 目标：使得 Σ(b_i²) ≤ 4n
    //
    // b_i 是落入桶 i 的键的个数，b_i² 就是该桶第二级表的大小。
    //
    // 我们随机尝试不同的 (a1, b1)，每试一次就计算 Σ(b_i²)，
    // 直到满足 ≤ 4n 为止。期望只需约 2 次尝试。
    //
    // 示例（n=5, keys={10,22,31,4,15}）：
    //   假设某次 (a1,b1) 将键分到 5 个桶中：
    //     桶0: 1个键 → b₀²=1
    //     桶1: 2个键 → b₁²=4
    //     桶2: 0个键 → b₂²=0
    //     桶3: 1个键 → b₃²=1
    //     桶4: 1个键 → b₄²=1
    //     Σ = 1+4+0+1+1 = 7 ≤ 4×5=20 ✓ 满足条件
    // ----------------------------------------------------------
    vector<int> bucketCount(n, 0);  // bucketCount[i] = 桶 i 中的键个数 b_i
    long long sumSquares;

    do
    {
        // 随机选择第一级哈希参数
        a1 = rand() % (prime - 1) + 1;  // a ∈ [1, p-1]，不能为 0
        b1 = rand() % prime;             // b ∈ [0, p-1]

        // 统计每个桶的键个数
        fill(bucketCount.begin(), bucketCount.end(), 0);
        for (int key : keys)
        {
            int bucket = universalHash(key, a1, b1, n);
            bucketCount[bucket]++;
        }

        // 计算 Σ(b_i²)
        sumSquares = 0;
        for (int count : bucketCount)
        {
            sumSquares += (long long)count * count;
        }
    } while (sumSquares > 4 * n);  // 不满足空间约束则重试

    // ----------------------------------------------------------
    // 第三步：为每个非空桶构建第二级无冲突哈希表
    //
    // 对桶 i（有 b_i 个键）：
    //   - 分配大小为 b_i² 的子表
    //   - 随机尝试 (a2[i], b2[i]) 直到 b_i 个键都映射到不同槽位
    //
    // 为什么 b_i² 的尺寸能保证找到无冲突函数？
    //   全域哈希族中，随机选 h，b_i 个键在 b_i² 个槽位中
    //   无冲突的概率 ≥ 1/2（数学证明）。
    //   所以期望每个桶只需重试约 2 次。
    //
    // 极端例子：
    //   b_i=1：子表大小 = 1² = 1，键直接放入，不可能冲突
    //   b_i=3：子表大小 = 3² = 9，3个元素放9个槽，空间充足
    //   b_i=0：空桶，不需要子表
    // ----------------------------------------------------------
    bucketSize.resize(n);
    a2.resize(n);
    b2.resize(n);
    table.resize(n);

    for (int i = 0; i < n; i++)
    {
        int bi = bucketCount[i];
        if (bi == 0)
        {
            bucketSize[i] = 0;  // 空桶标记
            continue;
        }

        // 表大小为 b_i²（b_i=1 时大小为 1）
        int tableSize = bi * bi;
        bucketSize[i] = tableSize;
        table[i].resize(tableSize, -1);  // -1 表示空槽位

        // 收集落入桶 i 的所有键
        vector<int> bucketKeys;
        for (int key : keys)
        {
            if (universalHash(key, a1, b1, n) == i)
            {
                bucketKeys.push_back(key);
            }
        }

        // 为桶 i 找一个无冲突的第二级哈希函数
        bool success = false;
        while (!success)
        {
            a2[i] = rand() % (prime - 1) + 1;
            b2[i] = rand() % prime;

            // 清空子表，重新尝试放置
            fill(table[i].begin(), table[i].end(), -1);
            success = true;

            for (int key : bucketKeys)
            {
                int slot = universalHash(key, a2[i], b2[i], tableSize);
                if (table[i][slot] != -1)
                {
                    // 冲突！两个键映射到同一个槽
                    // 换一组 (a2[i], b2[i]) 重试
                    success = false;
                    break;
                }
                table[i][slot] = key;
            }
        }
    }
}

// ============================================================
// 查找：最坏情况 O(1)，绝无冲突
//
// 步骤：
//   1. 用第一级哈希找到桶号
//   2. 如果桶为空 → 不存在
//   3. 用该桶的第二级哈希找到槽位
//   4. 比较槽位中的值与目标值
//
// 注意：即使 key 不在表中，也只访问 2 个位置（桶 + 子表槽位），
//       不会像线性探测那样沿着冲突链一直找下去。
// ============================================================
bool PerfectHash::search(int key)
{
    if (n == 0)
    {
        return false;
    }

    // 第一级哈希：确定在哪个桶
    int bucket = universalHash(key, a1, b1, n);

    // 该桶为空，key 一定不在表中
    if (bucketSize[bucket] == 0)
    {
        return false;
    }

    // 第二级哈希：在桶内定位
    int slot = universalHash(key, a2[bucket], b2[bucket], bucketSize[bucket]);

    // 直接比较，无需处理冲突（完美哈希！）
    return table[bucket][slot] == key;
}

// ============================================================
// 打印完美哈希表的内部结构
// ============================================================
void PerfectHash::display()
{
    cout << "========== 完美哈希表（FKS 两级方案） ==========" << endl;
    cout << "键的个数 n = " << n << endl;
    cout << "全域哈希素数 p = " << prime << endl;
    cout << endl;
    cout << "第一级哈希函数:" << endl;
    cout << "  h1(x) = ((" << a1 << " * x + " << b1 << ") % "
         << prime << ") % " << n << endl;
    cout << endl;

    int totalSlots = n;  // 第一级的 n 个桶
    int usedBuckets = 0;

    cout << "第二级子表:" << endl;
    for (int i = 0; i < n; i++)
    {
        if (bucketSize[i] > 0)
        {
            usedBuckets++;
            totalSlots += bucketSize[i];

            cout << "  桶 " << i << " ("
                 << bucketSize[i] << " 个槽):  ";
            cout << "h2(x) = ((" << a2[i] << " * x + " << b2[i]
                 << ") % " << prime << ") % " << bucketSize[i];
            cout << endl;
            cout << "    索引: ";
            for (int j = 0; j < bucketSize[i]; j++)
            {
                cout << j << "\t";
            }
            cout << endl;
            cout << "    键值: ";
            for (int j = 0; j < bucketSize[i]; j++)
            {
                if (table[i][j] == -1)
                {
                    cout << "·\t";  // 空槽用点表示
                }
                else
                {
                    cout << table[i][j] << "\t";
                }
            }
            cout << endl;
        }
    }

    cout << endl;
    cout << "统计:" << endl;
    cout << "  非空桶数: " << usedBuckets << " / " << n << endl;
    cout << "  第二级总槽位: " << (totalSlots - n) << endl;
    cout << "  总空间（桶+子表）: " << totalSlots << endl;
    cout << "  装载因子: " << (double)n / totalSlots << endl;
    cout << "==================================================" << endl;
}

// ============================================================
// 主函数：演示完美哈希表的构建与查找
// ============================================================
int main()
{
    // 静态键集合 —— 完美哈希要求提前知道所有键
    vector<int> keys = {10, 22, 31, 4, 15, 28, 17, 88, 59};

    cout << "输入键集合: ";
    for (int k : keys)
    {
        cout << k << " ";
    }
    cout << endl
         << endl;

    // 构建完美哈希表
    PerfectHash ph(keys);

    // 打印内部结构
    ph.display();

    // 查找测试
    cout << endl
         << "========== 查找测试 ==========" << endl;
    int testKeys[] = {10, 22, 31, 4, 15, 99, 100, 28, 88};
    for (int key : testKeys)
    {
        cout << "  search(" << key << ") = ";
        if (ph.search(key))
        {
            cout << "存在 ✓" << endl;
        }
        else
        {
            cout << "不存在 ✗" << endl;
        }
    }

    cout << endl
         << "关键要点：" << endl;
    cout << "  1. 每次查找只做 2 次哈希 + 1 次数组访问 = O(1) 最坏情况" << endl;
    cout << "  2. 表中 没有任何冲突，任意两个键不会映射到同一位置" << endl;
    cout << "  3. 代价是键必须提前已知（静态），且构建需要随机重试" << endl;
    cout << "  4. 总空间 O(n)，实践中约 2n~5n" << endl;

    return 0;
}
