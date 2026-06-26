// ============================================================
//   后缀数组（Suffix Array）—— 字符串处理的瑞士军刀
// ============================================================
//
// 【什么是后缀数组？】
//   后缀数组（Suffix Array）是由 Manber 和 Myers 于 1990 年提出的
//   数据结构。它是字符串所有后缀按字典序排序后的起始索引数组。
//
//   与后缀树相比：
//     - 空间更小（常数因子小得多）
//     - 实现更简单
//     - 配合 LCP 数组（最长公共前缀）可以达到与后缀树等价的查询能力
//     - 在竞赛编程和实际工程（如数据压缩、生物信息学）中更常用
//
//   示例：S = "banana$"
//
//     索引:  0 1 2 3 4 5 6
//     字符:  b a n a n a $
//
//     所有后缀按字典序排列：
//
//     排名  |  后缀       |  起始索引(sa[i])
//     ──────┼─────────────┼─────────────────
//      0    |  $          |  6
//      1    |  a$         |  5
//      2    |  ana$       |  3
//      3    |  anana$     |  1
//      4    |  banana$    |  0
//      5    |  na$        |  4
//      6    |  nana$      |  2
//
//     后缀数组 sa = [6, 5, 3, 1, 0, 4, 2]
//     排名数组 rank = [4, 3, 6, 2, 5, 1, 0]
//       （rank[i] = 后缀 i 在所有后缀中的排名）
//
// 【后缀数组 vs 后缀树】
//
//   ┌──────────────┬──────────────────┬─────────────────┐
//   │              │  后缀树           │  后缀数组        │
//   ├──────────────┼──────────────────┼─────────────────┤
//   │ 构建时间      │ O(n) (Ukkonen)   │ O(n log n)      │
//   │ 空间          │ 约 20n 字节      │ 约 4n 字节      │
//   │ 实现难度      │ 极难              │ 中等            │
//   │ 模式匹配      │ O(|P|)           │ O(|P| log n)    │
//   │               │                  │ 或 O(|P|+log n) │
//   │ 最长公共前缀  │ 自带             │ 需单独计算 LCP   │
//   └──────────────┴──────────────────┴─────────────────┘
//
// 【倍增算法（Doubling Algorithm）—— O(n log² n)】
//
//   核心思想：每次将比较的长度翻倍，用上一次的排名来比较。
//
//   第 1 轮：按每个后缀的第 1 个字符排序（基数排序，O(n)）
//   第 2 轮：按每个后缀的前 2 个字符排序
//            （利用第 1 轮的排名作为"前一半"，当前字符作为"后一半"）
//   第 3 轮：按每个后缀的前 4 个字符排序
//   第 k 轮：按每个后缀的前 2ᵏ 个字符排序
//
//   每轮使用"双关键字基数排序"：
//     第一关键字 = 后缀 i 前 2ᵏ⁻¹ 个字符的排名（rank[i]）
//     第二关键字 = 后缀 i+2ᵏ⁻¹ 前 2ᵏ⁻¹ 个字符的排名（rank[i+2ᵏ⁻¹]）
//
//   为什么第二关键字是 rank[i+len]？
//     后缀 i 的长度为 2×len 的前缀 =
//       前 len 个字符（后缀 i 的前缀）+
//       后 len 个字符（后缀 i+len 的前缀）
//
//   当 2ᵏ ≥ n 时，所有后缀都能被唯一排序，算法结束。
//
//   示例（S = "banana$"）：
//
//   第 1 轮（len=1）：
//     后缀起始 i:  0  1  2  3  4  5  6
//     字符:        b  a  n  a  n  a  $
//     初始排名:    2  1  3  1  3  1  0    (按字符大小)
//
//   第 2 轮（len=2）：
//     后缀 i 的 2-前缀 = S[i] + S[i+1]
//     后缀 0: "ba"  排名按 ("b","a") → rank[0]=2, rank[1]=1
//     后缀 1: "an"                            rank[1]=1, rank[2]=3
//     ...依此类推，用双关键字排序
//
//   排序多轮后，所有后缀被唯一确定顺序。
//
// 【LCP 数组（Longest Common Prefix）—— Kasai 算法】
//
//   LCP[i] = 排名 i 和 i-1 的两个后缀的最长公共前缀长度
//
//   LCP 数组的作用：
//     - 模式匹配优化：二分查找时利用 LCP 跳过字符比较
//     - 最长重复子串：max(LCP[i]) 即为答案
//     - 不同子串数量：n(n+1)/2 - ΣLCP[i]
//     - 最长公共子串（两个串）：类似后缀树的用法
//
//   Kasai 算法的关键观察：
//     如果后缀 i 和后缀 j 的 LCP 为 h，
//     那么后缀 i+1 和后缀 j+1 的 LCP 至少为 h-1。
//     利用这个性质，只需 O(n) 时间计算 LCP！
//
// 【后缀数组的应用】
//   1. 模式匹配：二分查找 sa 数组 O(|P| log n)
//   2. 最长重复子串：max(LCP) O(n)
//   3. 不同子串数量：n(n+1)/2 - ΣLCP O(n)
//   4. 最长公共子串（两个串）：利用 LCP O(n)
//   5. 字符串匹配统计（出现次数）：二分查找上下界
//
// ============================================================

#include<iostream>
#include<vector>
#include<string>
#include<algorithm>
using namespace std;

// ==================== 后缀数组（倍增算法）====================

class SuffixArray
{
private:
    string S;                     // 原始字符串
    int n;                        // 字符串长度
    vector<int> sa;               // 后缀数组：sa[i] = 排名 i 的后缀起始索引
    vector<int> rank;             // 排名数组：rank[i] = 后缀 i 的排名
    vector<int> lcp;              // LCP 数组：lcp[i] = 后缀 sa[i] 和 sa[i-1] 的 LCP

    // ----------------------------------------------------------
    // 倍增算法构建后缀数组 O(n log² n)
    //
    // 【核心思想】
    //   第 k 轮：按长度 2ᵏ 的前缀对所有后缀排序
    //   但不需要真的提取 2ᵏ 个字符来比较！利用上一轮的排名：
    //
    //     后缀 i 的长度为 2×len 的前缀 =
    //       前半段: 后缀 i       的前 len 个字符 (rank[i])
    //       后半段: 后缀 i+len   的前 len 个字符 (rank[i+len])
    //
    //   问题转化为：对 n 个 pair(rank[i], rank[i+len]) 排序。
    //   这就是"双关键字排序"。
    //
    // 【为什么 i+len ≥ n 时取 -1？】
    //   如果后半段不存在（后缀不够长），视为"无穷小"。
    //   因为短后缀在字典序上一定排在长后缀前面
    //   （前提：字符串末尾有 $ 终结符，$ < 任何字符）。
    //
    // 【提前终止条件】
    //   rank[sa[n-1]] == n-1，即最大排名 = n-1
    //   → 所有后缀的排名都不同（0 到 n-1 各出现一次）
    //   → 后缀已完全区分，无需继续倍增
    //
    // 【示例 S="banana$"】
    //   初始化: rank = [2,1,3,1,3,1,0]  (每个字符的ASCII排名)
    //   len=1:  排序 pair(rank[i], rank[i+1])
    //           sa=[6,5,1,3,0,2,4] → $, a$, an, an, ba, na, na
    //           newRank=[4,2,5,2,6,1,0]  (还有重复: 下标1和3都是2)
    //   len=2:  排序 pair(rank[i], rank[i+2])
    //           sa=[6,5,3,1,0,4,2] → $, a$, ana$, anana$, banana$, na$, nana$
    //           newRank=[6,5,3,1,0,4,2]  (全部唯一! 6...0 各出现一次)
    //           maxRank=6=n-1 → 提前结束
    // ----------------------------------------------------------
    void buildSA()
    {
        sa.resize(n);
        rank.resize(n);
        vector<int> newRank(n);

        // --- 第0轮：按每个后缀的第一个字符初始化排名 ---
        // 此时 rank[i] = S[i] 的 ASCII 值（字符本身作为初始排名）
        // 较大的 ASCII 值 → 较大的初始排名
        for (int i = 0; i < n; i++)
        {
            sa[i] = i;
            rank[i] = (unsigned char)S[i];
        }

        // --- 倍增：每轮将比较长度翻倍 ---
        for (int len = 1; len < n; len *= 2)
        {
            // ---- 双关键字排序 ----
            // 比较器：比较后缀 i 和 j 的 pair(rank[i], rank[i+len])
            auto cmp = [&](int i, int j) -> bool
            {
                if (rank[i] != rank[j])
                    return rank[i] < rank[j];   // 第一关键字更小 → 排前面

                // 第一关键字相等 → 比较第二关键字
                // i+len 越界 → 后缀已结束 → 视为 -1（比任何有效排名都小）
                int ri = (i + len < n) ? rank[i + len] : -1;
                int rj = (j + len < n) ? rank[j + len] : -1;
                return ri < rj;
            };
            sort(sa.begin(), sa.end(), cmp);

            // ---- 重新计算排名 ----
            // 技巧：sa 已经按 pair 升序排列
            // 如果相邻的两个后缀 pair 相同 → 给它俩相同的排名
            // 如果不同 → 排名 +1
            newRank[sa[0]] = 0;
            for (int i = 1; i < n; i++)
            {
                // cmp(sa[i-1], sa[i]) == true
                //   → sa[i-1] 严格小于 sa[i] → 排名需要增加
                // cmp(sa[i-1], sa[i]) == false
                //   → sa[i-1] 和 sa[i] 在当前长度下等价 → 排名相同
                if (cmp(sa[i - 1], sa[i]))
                    newRank[sa[i]] = newRank[sa[i - 1]] + 1;
                else
                    newRank[sa[i]] = newRank[sa[i - 1]];
            }
            rank = newRank;

            // 如果排名从 0 到 n-1 各出现一次 → 全部唯一 → 提前结束
            if (rank[sa[n - 1]] == n - 1) break;
        }
    }

    // ----------------------------------------------------------
    // Kasai 算法构建 LCP 数组 O(n)
    //
    // LCP[i] = 排名 i 的后缀与排名 i-1 的后缀的最长公共前缀长度
    //
    // 【朴素算法 O(n²)】
    //   对每个相邻排名对，从头开始比较 → O(n²)
    //
    // 【Kasai 的关键观察——"h 不会降太多"】
    //
    //   设后缀 i（起始位置为 i）的排名为 r。
    //   设后缀 j = sa[r-1]（排名在 i 前一位的后缀）。
    //   设 h = LCP(后缀 i, 后缀 j)。
    //
    //   考虑后缀 i+1 和 后缀 j+1：
    //     LCP(后缀 i+1, 后缀 j+1) ≥ h - 1
    //
    //   为什么？
    //     后缀 i    = S[i] + S[i+1] + S[i+2] + ...
    //     后缀 j    = S[j] + S[j+1] + S[j+2] + ...
    //     前面 h 个字符相同：S[i..i+h-1] = S[j..j+h-1]
    //     ∴ 去掉第一个字符后，后缀 i+1 和 后缀 j+1
    //       的前 h-1 个字符仍然相同！
    //
    //   这意味着：遍历 i=0..n-1 时，h 每次最多减少 1。
    //   h 总共增加不超过 n 次，总减少不超过 n 次 → O(n)！
    //
    // 【算法流程】
    //   按原始字符串顺序(i=0,1,..,n-1)处理每个后缀，
    //   而不是按排名顺序。这样可以利用"h 连续递减"的性质。
    //
    //   示例 S="banana$":
    //     i=0 后缀"banana$": rank=4, j=sa[3]=1 → "anana$", h=0
    //     i=1 后缀"anana$":  rank=3, j=sa[2]=3 → "ana$",   h=3 → h=2 for next
    //     i=2 后缀"nana$":   rank=6, j=sa[5]=4 → "na$",    h=2(从前次h-1开始) → h=0 for next
    //     ... (h 每次 -1 后可能继续增长)
    // ----------------------------------------------------------
    void buildLCP()
    {
        lcp.resize(n);
        // pos[sa[i]] = i, 即后缀 sa[i] 排名为 i
        // 相当于 rank 的另一种计算方式
        vector<int> pos(n);
        for (int i = 0; i < n; i++)
        {
            pos[sa[i]] = i;
        }

        int h = 0;  // LCP(后缀 i, 排名紧邻它的前一个后缀)
        for (int i = 0; i < n; i++)
        {
            int r = pos[i];  // 后缀 i 的排名
            if (r == 0)
            {
                // 排名最小的后缀没有前驱，LCP 定义为 0
                lcp[r] = 0;
                continue;
            }

            int j = sa[r - 1];  // 排名比 i 小一位的后缀起始位置

            // 关键：从 h 而不是 0 开始比较！
            // 由 Kasai 观察: LCP(后缀 i, 后缀 j) ≥ 上一次的 h - 1
            // 所以前面 h 个字符已经匹配，从 S[i+h] vs S[j+h] 开始继续比
            while (i + h < n && j + h < n && S[i + h] == S[j + h])
            {
                h++;
            }
            lcp[r] = h;

            // 为 i+1 做准备：
            //   LCP(后缀 i+1, 后缀 j+1) ≥ h-1
            //   所以下一次可以安全地从 h-1 开始比较
            if (h > 0) h--;
        }
    }

public:
    SuffixArray(const string& s) : S(s), n((int)s.length())
    {
        buildSA();
        buildLCP();
    }

    // ==================== 模式匹配（二分查找）====================
    //
    // 【原理】
    //   后缀数组 sa 是按字典序排列的所有后缀。
    //   模式串 P 若在 S 中出现，则所有以 P 为前缀的后缀
    //   在 sa 中必然连续排列！这就是"二分查找上下界"的依据。
    //
    //   例如 S="banana$", P="na":
    //     sa = [6($), 5(a$), 3(ana$), 1(anana$), 0(banana$), 4(na$), 2(nana$)]
    //     P="na" 匹配后缀 "na$"(位置4) 和 "nana$"(位置2)
    //     在 sa 中它们占据索引 5 和 6 → 连续！
    //
    //   lowerBound(P) = 第一个 ≥ P 的排名  (此处=5)
    //   upperBound(P) = 第一个 > P 的排名  (此处=7=n)
    //   出现次数 = upper - lower = 5,6 → 2 次
    //
    // 【时间复杂度】O(|P| log n)
    //   每次二分比较需要 O(|P|) 字符比较（使用 string::compare）

    // 查找第一个 ≥ P 的排名（下界）
    int lowerBound(const string& P) const
    {
        int low = 0, high = n;
        while (low < high)
        {
            int mid = low + (high - low) / 2;
            // string::compare(pos, len, str):
            //   从 S 的 sa[mid] 位置取 P.length() 个字符与 P 比较
            //   < 0 → S 的子串 < P → P 在更后面
            if (S.compare(sa[mid], P.length(), P) < 0)
                low = mid + 1;
            else
                high = mid;
        }
        return low;
    }

    // 查找第一个 > P 的排名（上界）
    int upperBound(const string& P) const
    {
        int low = 0, high = n;
        while (low < high)
        {
            int mid = low + (high - low) / 2;
            if (S.compare(sa[mid], P.length(), P) <= 0)
                low = mid + 1;   // S子串 ≤ P → 相等也要继续向右
            else
                high = mid;
        }
        return low;
    }

    // 返回模式串出现的次数
    int countOccurrences(const string& P) const
    {
        return upperBound(P) - lowerBound(P);
    }

    // 返回模式串出现所有起始位置
    vector<int> findAllOccurrences(const string& P) const
    {
        int lb = lowerBound(P);
        int ub = upperBound(P);
        vector<int> result;
        for (int i = lb; i < ub; i++)
        {
            result.push_back(sa[i]);
        }
        sort(result.begin(), result.end());
        return result;
    }

    // 判断模式串是否出现
    bool contains(const string& P) const
    {
        return lowerBound(P) < upperBound(P);
    }

    // ==================== 最长重复子串 ====================
    //
    // 【原理】
    //   任何重复子串都是某个后缀的前缀。在 sa 中，相邻后缀的 LCP
    //   就是这个重复子串的长度。最长重复子串 = max(LCP[1..n-1])。
    //
    //   为什么只看相邻后缀就够？
    //     若后缀 A < B < C，则 LCP(A,C) = min(LCP(A,B), LCP(B,C))
    //     所以非相邻后缀的 LCP 不会超过相邻的 LCP。
    //     答案一定出现在某个相邻对中。
    //
    //   示例: S="banana$", LCP=[0,1,3,0,0,2]
    //     max LCP = 3 → "ana"（出现在位置1和3）

    string longestRepeatedSubstring() const
    {
        int maxLen = 0;
        int maxIdx = 0;
        for (int i = 1; i < n; i++)
        {
            if (lcp[i] > maxLen)
            {
                maxLen = lcp[i];
                maxIdx = sa[i];  // 取任意一个都是答案
            }
        }
        return S.substr(maxIdx, maxLen);
    }

    // ==================== 不同子串数量 ====================
    //
    // 【原理】
    //   子串 = 某个后缀的某个前缀。
    //   后缀 i 有 n-i 个前缀（包括空前缀），
    //   所有后缀的前缀总数为 n(n+1)/2。
    //
    //   但多个后缀可能共享前缀（这些就是重复子串）。
    //   具体地：后缀 sa[i] 与前驱 sa[i-1] 共享了 LCP[i] 个前缀。
    //   按排名顺序遍历，每次减去 LCP[i] 即可去重。
    //
    //   公式：不同子串数 = n(n+1)/2 - Σ_{i=1}^{n-1} LCP[i]
    //
    //   示例: S="banana$"(n=7)
    //     LCP = [0, 1, 3, 0, 0, 2], ΣLCP = 6
    //     总数 = 28, 不同 = 28-6 = 22
    //
    //   验证：22个不同子串包括 $, a, an, ana, anan, anana,
    //         b, ba, ban, bana, banan, banana, n, na, nan, nana...

    long long countDistinctSubstrings() const
    {
        long long total = (long long)n * (n + 1) / 2;
        for (int i = 1; i < n; i++)
        {
            total -= lcp[i];
        }
        return total;
    }

    // ==================== 最长公共子串（两个串）====================
    //
    // 【原理】
    //   将两个串用不同的分隔符合并：S = A + "#" + B + "$"
    //   关键：两个分隔符必须不同且都小于任何字符！
    //     #  < $ < 任何字母
    //     这样 A 的后缀和 B 的后缀不会因为分隔符不同而排错序。
    //
    //   对合并后的串构建后缀数组 + LCP。
    //   答案 = 所有满足"一个后缀来自 A，另一个来自 B"的 LCP 最大值。
    //
    //   判断归属：起始索引 < |A| → 来自 A；否则来自 B（跳过分隔符#）
    //
    //   示例: A="banana"(6), B="ananas"(6)
    //     S="banana#ananas$"
    //     检查所有相邻对，若一个来自A、一个来自B：
    //       LCP 最大的相邻对即为答案。"anana"(len=5)
    // ----------------------------------------------------------
    static string longestCommonSubstring(const string& A, const string& B)
    {
        // 用 # 分隔 A, $ 结尾。两个分隔符都小于 'a'
        string combined = A + "#" + B + "$";
        SuffixArray sarray(combined);

        int n1 = (int)A.length();
        int maxLen = 0, maxIdx = 0;
        int n = sarray.n;

        for (int i = 1; i < n; i++)
        {
            int pos1 = sarray.sa[i - 1];
            int pos2 = sarray.sa[i];
            // 判断两个后缀是否分别来自 A 和 B（异或）
            bool fromA1 = (pos1 < n1);
            bool fromA2 = (pos2 < n1);
            if (fromA1 != fromA2)
            {
                if (sarray.lcp[i] > maxLen)
                {
                    maxLen = sarray.lcp[i];
                    maxIdx = sarray.sa[i];
                }
            }
        }
        return combined.substr(maxIdx, maxLen);
    }

    // ==================== 访问器 ====================

    const vector<int>& getSA() const { return sa; }
    const vector<int>& getRank() const { return rank; }
    const vector<int>& getLCP() const { return lcp; }
    const string& getString() const { return S; }

    // 打印后缀数组
    void print() const
    {
        cout << "\n========== 后缀数组 ==========" << endl;
        cout << "字符串: \"" << S << "\"\n" << endl;
        cout << "排名 | 起始索引 | 后缀" << endl;
        cout << "─────┼──────────┼──────" << endl;
        for (int i = 0; i < n; i++)
        {
            cout << "  " << i << "  │    " << sa[i] << "     │ \""
                 << S.substr(sa[i]) << "\"" << endl;
        }
        cout << "\n========== LCP 数组 ==========" << endl;
        for (int i = 1; i < n; i++)
        {
            cout << "LCP[" << i << "] = " << lcp[i]
                 << "  (后缀 \"" << S.substr(sa[i - 1]) << "\""
                 << " 和 \"" << S.substr(sa[i]) << "\")" << endl;
        }
        cout << "==============================" << endl;
    }
};

// ============================================================
//                   手动演示后缀数组构建过程
// ============================================================

void manualDemo()
{
    cout << "\n┌── 手动演示：S=\"banana$\" 的倍增算法过程 ────────┐" << endl;
    cout << "                                                    " << endl;
    cout << "  len=1: 按单个字符排序" << endl;
    cout << "    字符值: b=98, a=97, n=110, a=97, n=110, a=97, $=36" << endl;
    cout << "    排名:   [2, 1, 3, 1, 3, 1, 0]" << endl;
    cout << "                                                    " << endl;
    cout << "  len=2: 比较前 2 个字符" << endl;
    cout << "    后缀0: \"ba\" → (2, 1)" << endl;
    cout << "    后缀1: \"an\" → (1, 3)" << endl;
    cout << "    后缀2: \"na\" → (3, 1)" << endl;
    cout << "    后缀3: \"an\" → (1, 3)" << endl;
    cout << "    后缀4: \"na\" → (3, 1)" << endl;
    cout << "    后缀5: \"a$\" → (1, 0)" << endl;
    cout << "    后缀6: \"$\"  → (0, -1)" << endl;
    cout << "    排序后: [6, 5, 1, 3, 0, 2, 4]" << endl;
    cout << "    → $, a$, an, an, ba, na, na" << endl;
    cout << "                                                    " << endl;
    cout << "  len=4: 比较前 4 个字符" << endl;
    cout << "    最终得到唯一排序 → 算法结束" << endl;
    cout << "                                                    " << endl;
    cout << "  最终 SA = [6, 5, 3, 1, 0, 4, 2]" << endl;
    cout << "  对应后缀: $, a$, ana$, anana$, banana$, na$, nana$" << endl;
    cout << "                                                    " << endl;
    cout << "  LCP 计算（Kasai 算法）：" << endl;
    cout << "    LCP[1]=0  ($ vs a$)" << endl;
    cout << "    LCP[2]=1  (a$ vs ana$)" << endl;
    cout << "    LCP[3]=3  (ana$ vs anana$)" << endl;
    cout << "    LCP[4]=0  (anana$ vs banana$)" << endl;
    cout << "    LCP[5]=0  (banana$ vs na$)" << endl;
    cout << "    LCP[6]=2  (na$ vs nana$)" << endl;
    cout << "                                                    " << endl;
    cout << "  最长重复子串：max LCP = 3 → \"ana\"" << endl;
    cout << "  不同子串数：7*8/2 - (0+1+3+0+0+2) = 28 - 6 = 22" << endl;
    cout << "└────────────────────────────────────────────────────┘" << endl;
}

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║  后缀数组（Suffix Array）测试     ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    // ==================== 测试 1：后缀数组构建 ====================
    cout << "\n>>> 测试 1：构建 \"banana$\" 的后缀数组" << endl;
    cout << "  （$ 是终结符，ASCII=36，比任何字母都小）" << endl;
    SuffixArray sa1("banana$");
    sa1.print();

    // ==================== 测试 2：模式匹配（二分查找）====================
    cout << "\n>>> 测试 2：模式匹配 —— O(|P|·log n) 二分查找" << endl;
    cout << "  方法：在排好序的后缀数组上二分查找 P 的上下界" << endl;
    cout << "  \"ana\" 出现次数: " << sa1.countOccurrences("ana") << " (期待 1)" << endl;
    cout << "  \"na\" 出现次数: " << sa1.countOccurrences("na") << " (期待 2)" << endl;
    cout << "  \"ban\" 出现次数: " << sa1.countOccurrences("ban") << " (期待 1)" << endl;
    cout << "  \"xyz\" 出现次数: " << sa1.countOccurrences("xyz") << " (期待 0, 不存在的模式)" << endl;

    vector<int> occ = sa1.findAllOccurrences("na");
    cout << "  \"na\" 出现位置: ";
    for (int pos : occ) cout << pos << " ";
    cout << "(期待 2 4, 后缀\"na$\"和\"nana$\")" << endl;

    // ==================== 测试 3：最长重复子串 ====================
    cout << "\n>>> 测试 3：最长重复子串 = max(LCP[i])" << endl;
    string lrs = sa1.longestRepeatedSubstring();
    cout << "  \"banana$\" 的最长重复子串: \"" << lrs << "\" (期待 \"ana\")" << endl;

    // ==================== 测试 4：不同子串数量 ====================
    cout << "\n>>> 测试 4：不同子串数量 = n(n+1)/2 - ΣLCP" << endl;
    cout << "  n=7, 所有前缀总数=28" << endl;
    cout << "  LCP 和 = 0+1+3+0+0+2 = 6" << endl;
    cout << "  不同子串数: " << sa1.countDistinctSubstrings() << " (期待 22)" << endl;

    // ==================== 测试 5：最长公共子串（两个串）====================
    cout << "\n>>> 测试 5：最长公共子串 —— A+\"#\"+B+\"$\"" << endl;
    cout << "  用不同的分隔符确保 A、B 的后缀正确排序" << endl;
    string lcs1 = SuffixArray::longestCommonSubstring("banana", "ananas");
    cout << "  \"banana\" ∩ \"ananas\" = \"" << lcs1 << "\" (期待 \"anana\")" << endl;

    string lcs2 = SuffixArray::longestCommonSubstring("ababc", "babcde");
    cout << "  \"ababc\" ∩ \"babcde\" = \"" << lcs2 << "\" (期待 \"babc\")" << endl;

    // ==================== 测试 6：第二个测试串 ====================
    cout << "\n>>> 测试 6：\"mississippi$\" —— 经典例子" << endl;
    SuffixArray sa2("mississippi$");
    sa2.print();
    cout << "  最长重复子串: \"" << sa2.longestRepeatedSubstring() << "\" (期待 \"issi\")" << endl;

    // ==================== 测试 7：手动演示 ====================
    manualDemo();

    cout << "\n后缀数组所有测试完成！" << endl;
    return 0;
}
