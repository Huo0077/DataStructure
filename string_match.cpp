// ============================================================
//      串的模式匹配：朴素匹配 + KMP 算法
// ============================================================
//
// 【问题定义】
//   给定文本串 S（长度 n）和模式串 P（长度 m），找出 P 在 S 中出现的所有位置。
//
// 【朴素匹配（Brute-Force）】
//   对 S 的每个起始位置 i，逐个字符比较 S[i+j] 与 P[j]。
//   失配时 i++，j 归零重新开始。
//
//   时间复杂度：O(n*m) 最坏（例如 S="aaaa...ab", P="aaa...a" 时每次匹配到最后一个字符才失配）
//
//   优点：实现简单，短模式或随机文本下表现尚可。
//
// 【KMP 算法（Knuth-Morris-Pratt, 1977）】
//   核心思想：失配时，利用已经匹配成功的部分信息，
//   避免主串指针 i 回退，只移动模式串指针 j。
//
//   next 数组：next[j] = 在 P[0..j-1] 中，最长相等前后缀的长度。
//   失配时 j = next[j-1]（跳过已知不可能匹配的位置），
//   如果 j == 0 则 j 不变，i++。
//
//   时间复杂度：O(n + m)，预处理 O(m) 匹配 O(n)。
//
// ============================================================
//   KMP 详细示例（理解 next 数组）
// ============================================================
//
//   模式串 P = "ABABC"（5 个字符）
//
//   prefix（前缀）：不包含最后一个字符，从开头起的子串
//   suffix（后缀）：不包含第一个字符，到结尾的子串
//   next[j]（通常定义为不包含 P[j] 的前缀函数值）：
//     或有些教材定义 next[0] = -1 便于代码统一。
//     本实现采用 next[j] = 前缀函数 π[j]（最长相等前后缀长度）。
//
//   计算过程（对于 P = "ABAB" 的子模式）：
//     j=0: "A"   → 无真前后缀 → π[0]=0
//     j=1: "AB"  → 前缀="A", 后缀="B" → 不等 → π[1]=0
//     j=2: "ABA" → 前缀="A"/"AB", 后缀="A"/"BA" → "A"匹配 → π[2]=1
//     j=3: "ABAB"→ 前缀="A"/"AB"/"ABA", 后缀="B"/"AB"/"BAB" → "AB"匹配 → π[3]=2
//
//   失配时如何用 π？
//     S = "ABABABC", P = "ABABC"
//     匹配到 j=4 时：S[4]='A', P[4]='C' → 失配！
//     已匹配 "ABAB" (j=4 个字符)，其最长相等前后缀 = π[3] = 2 ("AB")
//     模式串后移 4-2=2 位，j 跳回 2：相当于 j = π[j-1] = π[3] = 2
//     下一个比较 S[4] vs P[2]='A' → 匹配！
//
//   "跳过"的含义：已匹配的后缀 = 相同的前缀，前缀部分无需重复比较。
//
// 【nextval 优化】
//   当 P[j] == P[next[j]] 时，可以进一步优化：
//   S[i] 已经和 P[j] 失配了，如果 P[j] == P[next[j]]，
//   那么 S[i] 也必然和 P[next[j]] 失配。所以直接跳过。
//
//   nextval[j] = (P[j] == P[next[j]]) ? nextval[next[j]] : next[j]
//
//   示例 P = "AAAB"：
//     next[0]=0, next[1]=1, next[2]=2, next[3]=0
//     nextval[0]=0, nextval[1]=0（P[1]='A'==P[1]'s next='A'→跳过）,
//     nextval[2]=0（同样跳过）, nextval[3]=0
//
// 【KMP 应用场景】
//   - 文本编辑器中的查找替换
//   - 病毒特征码匹配
//   - DNA 序列比对
//   - 网络入侵检测系统（NIDS）的字符串匹配
//
// ============================================================

#include <iostream>
#include <vector>
#include <string>
using namespace std;

// 【阅读地图】
//   推荐顺序：朴素匹配 -> 前缀函数 pi -> KMP 匹配 -> nextval 优化。
//   KMP 循环不变量：已经匹配的 P[0..j-1] 同时是当前文本扫描位置之前的后缀。
//   失配时只回退模式串位置 j，文本指针 i 不回退；这是线性复杂度的关键。

// ============================================================
//   Part 1: 朴素匹配（Brute-Force）
// ============================================================
//   从文本串 S 的每个位置开始，逐字符比较模式串 P
//   O(n*m)
// ============================================================

vector<int> bruteForce(const string& S, const string& P)
{
    vector<int> result;
    int n = S.length(), m = P.length();
    if (m == 0) return result;

    for (int i = 0; i <= n - m; i++)
    {
        int j = 0;
        while (j < m && S[i + j] == P[j])
            j++;
        if (j == m)
            result.push_back(i);  // 匹配成功
    }
    return result;
}

// ============================================================
//   Part 2: KMP 算法
// ============================================================

// ---------- 计算前缀函数 π（即 next 数组）----------
// π[j] = P[0..j] 的最长相等前后缀长度
//
// 算法（双指针 + 自底向上）：
//   维护指针 j 指向"当前已匹配的前缀长度"。
//   对于 i 从 1 到 m-1：
//     while j>0 且 P[i] != P[j]：j = π[j-1]（回溯找一个更短的可能匹配）
//     如果 P[i] == P[j]：j++
//     π[i] = j
//
// 为什么 while 里 j = π[j-1]？
//   当前不匹配时，已有的 j-1 长度前缀的信息不能浪费。
//   π[j-1] 告诉我们"长度为 j-1 的前缀的最长相等前后缀"，
//   也就是当 j 退一步后可以尝试的新位置。
//   这和匹配阶段的回退逻辑完全一致，只是这里是在模式串内部跑。
//
//   手动推演 P = "ABABC"：
//     i=0: j=0, π[0]=0
//     i=1: P[1]='B' ≠ P[0]='A' → j stays 0 → π[1]=0
//     i=2: P[2]='A' == P[0]='A' → j=1 → π[2]=1
//     i=3: P[3]='B' == P[1]='B' → j=2 → π[3]=2
//     i=4: P[4]='C' ≠ P[2]='A' → j=π[1]=0 → 再比: P[4]='C' ≠ P[0]='A' → π[4]=0
//   结果 π = [0,0,1,2,0]
// ============================================================

vector<int> computePrefix(const string& P)
{
    int m = P.length();
    vector<int> pi(m, 0);  // pi = prefix function = next array
    int j = 0;              // j = 当前已匹配的前缀长度

    for (int i = 1; i < m; i++)
    {
        // 不匹配 → 回溯，直到匹配或 j=0
        while (j > 0 && P[i] != P[j])
            j = pi[j - 1];

        // 匹配 → 延长前缀
        if (P[i] == P[j])
            j++;

        pi[i] = j;
    }
    return pi;
}

// ---------- KMP 匹配 ----------
// 对于文本串 S 的每个字符 S[i]：
//   while j>0 且 S[i] != P[j]：j = pi[j-1]（模式串回退）
//   如果 S[i] == P[j]：j++
//   如果 j == m：匹配成功，记录位置 i-m+1，j = pi[j-1]（继续找下一个）
//
// 完整推演示例 S="ABABABC", P="ABABC", pi=[0,0,1,2,0]：
//   i=0: S[0]='A'==P[0]='A' → j=1
//   i=1: S[1]='B'==P[1]='B' → j=2
//   i=2: S[2]='A'==P[2]='A' → j=3
//   i=3: S[3]='B'==P[3]='B' → j=4
//   i=4: S[4]='A'≠P[4]='C' → j=pi[3]=2  (跳过已匹配后缀"AB"对应的前缀)
//        再比 S[4]='A'==P[2]='A' → j=3
//   i=5: S[5]='B'==P[3]='B' → j=4
//   i=6: S[6]='C'==P[4]='C' → j=5 → 匹配! 位置 i-m+1=2 → j=pi[4]=0
//   结果：在位置 2 找到匹配
// ============================================================

vector<int> kmpSearch(const string& S, const string& P)
{
    vector<int> result;
    int n = S.length(), m = P.length();
    if (m == 0) return result;

    vector<int> pi = computePrefix(P);
    int j = 0;

    for (int i = 0; i < n; i++)
    {
        while (j > 0 && S[i] != P[j])
            j = pi[j - 1];

        if (S[i] == P[j])
            j++;

        if (j == m)
        {
            result.push_back(i - m + 1);
            j = pi[j - 1];  // 继续找下一个匹配
        }
    }
    return result;
}

// ============================================================
//   Part 3: KMP 优化 —— nextval 数组
// ============================================================
//   当 P[j] == P[pi[j]] 时，回退到 pi[j] 也会失配（因为值相同），
//   所以直接跳得更远。这是对 next/pi 数组的进一步压缩。
//
//   以下同时输出 pi 和 nextval 以便对比。
// ============================================================

void kmpWithNextval(const string& S, const string& P)
{
    int m = P.length();
    vector<int> pi = computePrefix(P);

    // 计算 nextval
    vector<int> nextval(m, 0);
    for (int i = 1; i < m; i++)
    {
        if (P[i] == P[pi[i]])
            nextval[i] = nextval[pi[i]];  // 跳过相同的
        else
            nextval[i] = pi[i];
    }

    cout << "  模式串 P = \"" << P << "\"" << endl;
    cout << "  j:      ";
    for (int i = 0; i < m; i++) cout << i << " ";
    cout << "\n  P[j]:   ";
    for (int i = 0; i < m; i++) cout << P[i] << " ";
    cout << "\n  next:   ";
    for (int i = 0; i < m; i++) cout << pi[i] << " ";
    cout << "\n  nextval:";
    for (int i = 0; i < m; i++) cout << nextval[i] << " ";
    cout << endl;

    // 用 nextval 匹配（逻辑相同，只是回退用 nextval）
    cout << "  在 S=\"" << S << "\" 中的匹配位置: ";
    int n = S.length(), j = 0;
    for (int i = 0; i < n; i++)
    {
        while (j > 0 && S[i] != P[j])
            j = nextval[j - 1];  // 用 nextval 替代 pi

        if (S[i] == P[j])
            j++;

        if (j == m)
        {
            cout << (i - m + 1) << " ";
            j = nextval[j - 1];
        }
    }
    cout << endl;
}

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════════╗" << endl;
    cout << "║    串模式匹配 / KMP 综合测试         ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;

    // ============ 测试 1：朴素匹配 ============
    cout << "\n===== Part 1: 朴素匹配 (Brute-Force) =====" << endl;
    {
        string S = "ABABDABACDABABCABAB";
        string P = "ABABC";
        auto pos = bruteForce(S, P);
        cout << "S=\"" << S << "\"" << endl;
        cout << "P=\"" << P << "\"" << endl;
        cout << "匹配位置: ";
        for (int p : pos) cout << p << " ";
        cout << "(expect: 10)" << endl;
    }

    // ============ 测试 2：KMP ============
    cout << "\n===== Part 2: KMP 算法 =====" << endl;
    {
        string S = "ABABABCABABABCABABABC";
        string P = "ABABC";
        auto pos = kmpSearch(S, P);
        cout << "S=\"" << S << "\"" << endl;
        cout << "P=\"" << P << "\"" << endl;
        cout << "KMP 匹配位置: ";
        for (int p : pos) cout << p << " ";
        cout << "(expect: 2 9 16)" << endl;

        vector<int> pi = computePrefix(P);
        cout << "next 数组: ";
        for (int v : pi) cout << v << " ";
        cout << "(expect: 0 0 1 2 0)" << endl;
    }

    // ============ 测试 3：nextval 优化 ============
    cout << "\n===== Part 3: nextval 优化 =====" << endl;
    {
        kmpWithNextval("ABCABCABC", "ABCABC");    // 无相同字符的优化空间小
        cout << endl;
        kmpWithNextval("AAAABAAAABAAAAB", "AAAAB");  // 重复字符多 → nextval 效果明显
    }

    // ============ 测试 4：朴素 vs KMP 对比 ============
    cout << "\n===== Part 4: 边界情况 =====" << endl;
    {
        cout << "空模式串: ";
        auto r1 = kmpSearch("hello", "");
        cout << r1.size() << " 个匹配 (expect 0)" << endl;

        cout << "模式串比文本长: ";
        auto r2 = kmpSearch("hi", "hello");
        cout << r2.size() << " 个匹配 (expect 0)" << endl;

        cout << "不存在的模式: ";
        auto r3 = kmpSearch("abcdef", "xyz");
        cout << r3.size() << " 个匹配 (expect 0)" << endl;
    }

    cout << "\n所有测试完成！" << endl;
    return 0;
}
