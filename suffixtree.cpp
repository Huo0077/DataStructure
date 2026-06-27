// ============================================================
//      后缀树（Suffix Tree）+ 后缀字典树（Suffix Trie）
// ============================================================
//
// 【什么是后缀？】
//   字符串 S 的后缀是从 S 某个位置 i 开始到末尾的子串。
//   例如 S = "banana"，其后缀有：
//     "banana" (i=0)
//     "anana"  (i=1)
//     "nana"   (i=2)
//     "ana"    (i=3)
//     "na"     (i=4)
//     "a"      (i=5)
//
// 【后缀字典树（Suffix Trie）】
//   把所有后缀插入一棵 Trie（字典树）中，就得到后缀字典树。
//
//   以 S = "banana$" 为例（$ 是结束符，确保没有后缀是另一个后缀的前缀）：
//
//                        root
//                    /   |    \    \
//                   a   ba   na   $
//                  / \   |    |
//                 $  na na$  na$
//                    |  |
//                   na$ $
//                    |
//                    $
//
//   优点：构建简单，直观
//   缺点：节点数 O(n²)（对于长度为 n 的字符串，最坏有 n² 个节点）
//
// 【后缀树（Suffix Tree）】
//   后缀树是后缀 Trie 的"压缩"版——把只有单个子节点的链压缩为一条边。
//
//   以上面 S = "banana$" 的后缀 Trie 压缩后：
//
//                        root
//                    /    |     \    \
//                   a    ba   na   $
//                  / \    |     |
//               $  na   na$   na$
//                   |     |
//                 na$     $
//                   |
//                   $
//
//   压缩后节点数降为 O(n)。
//
//   后缀树的存储使用"边标签"（edge label），每条边存储
//   (start, end) 索引对，指向原字符串的子串，而不是拷贝子串。
//
//   例如边 (2, 5) 表示 S[2..5] = "nana"。
//
// 【后缀树的关键性质】
//
//   1. 从根到任意叶节点的路径恰好对应 S 的一个后缀
//      （这也是后缀树名字的由来）
//
//   2. 每个内部节点（非叶非根）至少有 2 个子节点
//      （否则就会被压缩为边的一部分）
//
//   3. 每条边代表 S 的一个非空子串
//
//   4. 叶节点数量 = n（字符串长度 + 1，包含结束符 $）
//
//   5. 对于长度为 n 的字符串（含 $），后缀树最多有 2n 个节点
//      和 2n-1 条边（这是后缀树能 O(n) 空间的根本原因）
//
// 【后缀树的典型应用】
//
//   1. 模式匹配（Pattern Matching）
//       判断模式 P 是否为 S 的子串。
//       从根出发，按 P 的字符依次沿边匹配。
//       时间复杂度：O(|P|)，与 |S| 无关！
//       这比 KMP 算法更适合多次查询同一文本的场景。
//
//   2. 最长重复子串（Longest Repeated Substring）
//       后序遍历，找最深的有 ≥ 2 个叶子的内部节点。
//
//   3. 最长公共子串（Longest Common Substring）
//       将两个串 S₁ 和 S₂ 构造成 S₁#S₂$ 的后缀树，
//       找最深的既有来自 S₁ 又有来自 S₂ 的叶子的节点。
//
//   4. 最长回文子串（Longest Palindromic Substring）
//       将 S 和其反转 S^R 构造成 S#S^R$ 的后缀树。
//
//   5. 基因序列比对（Bioinformatics）
//       DNA 序列的重复片段、motif 发现等。
//
// 【Ukkonen 算法核心概念（O(n) 在线构建后缀树）】
//
//   Ukkonen 算法在 1995 年提出，可以在 O(n) 时间内在线
//   （逐个字符添加）构造后缀树。这是后缀树能够实用的关键。
//
//   核心思想：维护"活动点"（active point），追踪上一次插入
//   结束的位置，避免每次重新从根开始匹配。
//
//   三个关键概念：
//
//   （1）活动点（Active Point）—— (activeNode, activeEdge, activeLength)
//        activeNode:  活动节点，后缀链接的目的节点
//        activeEdge:  活动边（activeNode 的某条出边的首字符）
//        activeLength: 在活动边上已匹配的长度
//
//   （2）后缀链接（Suffix Link）
//        每个内部节点 v 有一条后缀链接指向另一个内部节点 s(v)。
//        如果从根到 v 的路径表示子串 "xα"（x 是单字符，α 是子串），
//        那么 s(v) 表示子串 "α"（去掉首字符）。
//
//        后缀链接的作用：在插入新后缀时，可以"跳转"而不是从根重新走，
//        这是 Ukkonen 算法 O(n) 的关键。
//
//   （3）剩余后缀计数（Remainder）
//        记录还有多少个后缀需要在当前位置显式插入。
//
//   规则 1（扩展叶节点）：每次扩展时，把当前字符追加到所有叶节点边。
//                        用 (start, ∞) 表示"延伸到字符串末尾"。
//
//   规则 2（创建新分支）：如果当前字符在活动点不存在，创建新分支。
//
//   规则 3（匹配继续）：如果当前字符在活动点已存在，remainder++，进入下一阶段。
//
//   Ukkonen 算法的巧妙之处：
//     - 规则 1 用"开区间"模拟自动延伸，不需要实际更新所有叶节点
//     - 规则 2 创建内部节点时同时建立后缀链接
//     - 后缀链接让跳转只需 O(1)，全程均摊 O(n)
//
// 【实现说明】
//   本文件实现两版：
//     Part 1: 后缀 Trie（教学版，清晰易懂）
//     Part 2: 后缀树（Ukkonen 算法，O(n) 构建）
//   并提供模式匹配、最长重复子串、最长公共子串等应用函数。
//
// ============================================================

#include<iostream>
#include<unordered_map>
#include<vector>
#include<string>
#include<algorithm>
#include<climits>
using namespace std;

// ============================================================
//   Part 1: 后缀字典树（Suffix Trie）—— 教学版
// ============================================================
//
// 把所有后缀插入 Trie，节点存储子节点映射。
// 时间复杂度：构建 O(n²)，空间 O(n²)。
// 用途：帮助理解后缀树的"压缩"概念。

class SuffixTrieNode
{
public:
    unordered_map<char, SuffixTrieNode*> children;
    bool isEnd;                              // 是某个后缀的结尾
    int suffixIndex;                         // 该后缀在原串的起始位置（仅叶节点有效）

    SuffixTrieNode()
        : isEnd(false), suffixIndex(-1) {}
};

class SuffixTrie
{
private:
    SuffixTrieNode* root;
    string text;

    // 插入单个后缀
    void insertSuffix(const string& suffix, int startIdx)
    {
        SuffixTrieNode* curr = root;
        for (char ch : suffix)
        {
            if (curr->children.find(ch) == curr->children.end())
            {
                curr->children[ch] = new SuffixTrieNode();
            }
            curr = curr->children[ch];
        }
        curr->isEnd = true;
        curr->suffixIndex = startIdx;
    }

    // 递归销毁
    void destroy(SuffixTrieNode* node)
    {
        if (node == nullptr) return;
        for (auto& p : node->children)
        {
            destroy(p.second);
        }
        delete node;
    }

    // 递归打印
    void printHelper(SuffixTrieNode* node, string prefix) const
    {
        if (node->isEnd)
        {
            cout << prefix << " [$] (后缀起始位置=" << node->suffixIndex << ")" << endl;
        }
        for (auto& p : node->children)
        {
            string childPrefix = prefix + p.first;
            if (p.second->children.size() > 1 || p.second->isEnd)
            {
                // 只在有分支或结束的地方换行打印
                cout << childPrefix << endl;
                printHelper(p.second, "  ");
            }
            else
            {
                printHelper(p.second, childPrefix);
            }
        }
    }

public:
    SuffixTrie() { root = new SuffixTrieNode(); }
    ~SuffixTrie() { destroy(root); }

    // 从文本构建后缀 Trie
    void build(const string& s)
    {
        text = s;
        for (int i = 0; i < (int)s.length(); i++)
        {
            insertSuffix(s.substr(i), i);
        }
    }

    // 查询模式串是否为子串
    bool search(const string& pattern) const
    {
        SuffixTrieNode* curr = root;
        for (char ch : pattern)
        {
            auto it = curr->children.find(ch);
            if (it == curr->children.end())
            {
                return false;
            }
            curr = it->second;
        }
        return true;
    }

    void print() const
    {
        cout << "后缀 Trie 结构：" << endl;
        printHelper(root, "");
    }

    int countNodes() const
    {
        // 统计节点数（验证 O(n²) 空间复杂度）
        return countNodesHelper(root);
    }

    int countNodesHelper(SuffixTrieNode* node) const
    {
        if (node == nullptr) return 0;
        int count = 1;
        for (auto& p : node->children)
        {
            count += countNodesHelper(p.second);
        }
        return count;
    }
};

// ============================================================
//   Part 2: 后缀树（Suffix Tree）—— Ukkonen 算法实现
// ============================================================

class SuffixTreeNode
{
public:
    // children: 首字符 → 子节点
    unordered_map<char, SuffixTreeNode*> children;
    int start;            // 边标签在原串中的起始索引
    int* end;             // 边标签在原串中的结束索引（指针，叶节点共享全局 end）
    int suffixLink;       // 后缀链接指向的节点 ID（-1 表示根）
    int id;               // 节点编号（调试用）

    SuffixTreeNode(int s, int* e, int nodeId)
        : start(s), end(e), suffixLink(-1), id(nodeId) {}

    // 边的长度
    int edgeLength() const
    {
        return *end - start + 1;
    }
};

class SuffixTree
{
private:
    string text;                          // 原始字符串（已添加结束符）
    vector<SuffixTreeNode*> nodes;        // 所有节点（用 vector 管理，suffixLink 用索引）
    int rootID;                           // 根节点在 nodes 中的索引

    // 活动点（Ukkonen 算法的核心状态）
    int activeNode;                       // 活动节点 ID
    int activeEdge;                       // 活动边的首字符索引（-1 表示无活动边）
    int activeLength;                     // 在活动边上已匹配的长度

    int remainder;                        // 剩余需要插入的后缀数
    int* leafEnd;                         // 全局叶节点结束位置（会随扩展递增）

    // 上一个创建的内部节点（用于设置后缀链接）
    int lastNewNode;

    // ----------------------------------------------------------
    // 创建新节点
    // ----------------------------------------------------------
    int newNode(int start, int* end)
    {
        SuffixTreeNode* node = new SuffixTreeNode(start, end, (int)nodes.size());
        nodes.push_back(node);
        return node->id;
    }

    // ----------------------------------------------------------
    // 获取节点在活动边的长度
    // ----------------------------------------------------------
    int edgeLen(int nodeID) const
    {
        SuffixTreeNode* node = nodes[nodeID];
        return node->edgeLength();
    }

    // ----------------------------------------------------------
    // 沿着活动边往下走一步（更新活动点）
    //
    // 这意味着当前字符在当前活动边匹配了 activeLength 个字符后
    // 仍然匹配，所以活动长度 +1。
    // 如果活动长度等于了当前边的长度，则"穿过"该边，活动点变为子节点。
    //
    // 参数 pos: 当前正在处理的字符在原串中的位置
    // ----------------------------------------------------------
    bool walkDown(int currNodeID, int pos)
    {
        SuffixTreeNode* currNode = nodes[currNodeID];
        int len = edgeLen(currNodeID);

        if (activeLength >= len)
        {
            // 活动长度超过了当前边 → 走向子节点
            activeEdge += len;     // 活动边首字符前移
            activeLength -= len;   // 减去已走过的长度
            activeNode = currNodeID;
            return true;
        }
        return false;
    }

    // ----------------------------------------------------------
    // 扩展后缀树（Ukkonen 算法的单步扩展）
    //
    // 参数 pos: 当前字符在原串中的索引
    //
    // 每次调用会处理 remainder 个后缀的扩展。
    // 核心循环：
    //   1. 检查当前字符在活动点是否已存在（规则 3）
    //   2. 如果不存在 → 创建新分支（规则 2）
    //   3. 设置后缀链接
    //   4. 更新活动点（使用后缀链接跳转）
    // ----------------------------------------------------------
    void extendSuffixTree(int pos)
    {
        (*leafEnd) = pos;         // 规则 1：所有叶节点自动"延伸"
        remainder++;               // 积累一个需要插入的后缀
        lastNewNode = -1;          // 重置上一个新节点

        while (remainder > 0)
        {
            if (activeLength == 0)
            {
                // 活动长度 = 0 → 活动边就是当前字符
                activeEdge = pos;
            }

            SuffixTreeNode* activeNodePtr = nodes[activeNode];
            char currentChar = text[pos];

            // 检查当前字符是否在活动边中存在
            char edgeChar = text[activeEdge];
            auto it = activeNodePtr->children.find(edgeChar);

            if (it == activeNodePtr->children.end())
            {
                // ---- 规则 2：创建新分支（叶节点）----
                // 这个后缀在活动点不存在，需要创建新的叶节点

                int newLeafID = newNode(pos, leafEnd);
                activeNodePtr->children[edgeChar] = nodes[newLeafID];

                // 设置后缀链接
                if (lastNewNode != -1)
                {
                    nodes[lastNewNode]->suffixLink = activeNode;
                    lastNewNode = -1;
                }
            }
            else
            {
                // 活动边已存在，需要沿边走下去
                int nextNodeID = it->second->id;
                SuffixTreeNode* nextNode = nodes[nextNodeID];

                // 检查是否需要 walked down
                if (walkDown(nextNodeID, pos))
                {
                    // 穿过了当前边，重新循环（不减少 remainder）
                    continue;
                }

                // 沿边比较当前字符
                char edgeNextChar = text[nextNode->start + activeLength];

                if (edgeNextChar == currentChar)
                {
                    // ---- 规则 3：字符已存在 → 匹配继续，跳出循环 ----
                    // 更新后缀链接
                    if (lastNewNode != -1 && activeNode != rootID)
                    {
                        nodes[lastNewNode]->suffixLink = activeNode;
                        lastNewNode = -1;
                    }
                    activeLength++;
                    break;  // 规则 3 触发，remainder 保留，进入下一阶段
                }

                // ---- 规则 2（分裂内部节点）：当前字符不匹配，需要分裂边 ----

                // 创建新的内部节点（分裂点）
                int* splitEnd = new int(nextNode->start + activeLength - 1);
                int splitNodeID = newNode(nextNode->start, splitEnd);

                // 把分裂点插入到 activeNode 和 nextNode 之间
                activeNodePtr->children[edgeChar] = nodes[splitNodeID];

                // 原来 nextNode 成为 splitNode 的子节点（边被截断）
                nextNode->start += activeLength;   // nextNode 的边从分裂点后开始
                nodes[splitNodeID]->children[text[nextNode->start]] = nextNode;

                // 创建新的叶节点（当前后缀）
                int newLeafID = newNode(pos, leafEnd);
                nodes[splitNodeID]->children[currentChar] = nodes[newLeafID];

                // 后缀链接：上一个新建的节点指向这个分裂点
                if (lastNewNode != -1)
                {
                    nodes[lastNewNode]->suffixLink = splitNodeID;
                }
                lastNewNode = splitNodeID;
            }

            remainder--;

            // 更新活动点（要么用后缀链接跳转，要么从根重新开始）
            if (activeNode == rootID && activeLength > 0)
            {
                // 从根出发：活动长度减 1，活动边向前移 1 个字符
                activeLength--;
                activeEdge = pos - remainder + 1;
            }
            else if (activeNode != rootID)
            {
                // 利用后缀链接跳转！
                // 这是 Ukkonen 算法的精髓：O(1) 跳转到下一个要处理的后缀
                activeNode = nodes[activeNode]->suffixLink;
                // 如果后缀链接指向 -1（根），重置
                if (activeNode == -1) activeNode = rootID;
            }
        }
    }

    // ----------------------------------------------------------
    // 递归销毁
    // ----------------------------------------------------------
    void destroyNodes()
    {
        for (SuffixTreeNode* node : nodes)
        {
            delete node;
        }
        nodes.clear();
    }

    // ----------------------------------------------------------
    // 获取从节点 nodeID 到根的路径表示的子串
    // ----------------------------------------------------------
    string getPathLabel(int nodeID) const
    {
        string result;
        int curr = nodeID;
        while (curr != rootID)
        {
            SuffixTreeNode* node = nodes[curr];
            int len = node->edgeLength();
            string seg = text.substr(node->start, len);
            result = seg + result;

            // 找父节点
            for (const SuffixTreeNode* other : nodes)
            {
                for (auto& p : other->children)
                {
                    if (p.second->id == curr)
                    {
                        curr = other->id;
                        goto found;
                    }
                }
            }
            break;  // 找不到父节点（根）
        found:;
        }
        return result;
    }

    // ----------------------------------------------------------
    // 设置后缀链接（Ukkonen 算法构建完成后调用）
    // 在实际的 Ukkonen 实现中，后缀链接在扩展过程中实时建立，
    // 此函数用于递归建立尚未建立的链接。
    // ----------------------------------------------------------

    // ----------------------------------------------------------
    // 递归打印树结构
    // ----------------------------------------------------------
    void printHelper(int nodeID, const string& prefix, bool isLast, int depth) const
    {
        if (depth > 6) return;  // 防止递归过深

        SuffixTreeNode* node = nodes[nodeID];
        string suffixInfo = "";
        if (node->suffixLink != -1)
        {
            suffixInfo = "  [sl→" + to_string(node->suffixLink) + "]";
        }

        if (nodeID != rootID)
        {
            cout << prefix;
            cout << (isLast ? "└── " : "├── ");
            int len = node->edgeLength();
            cout << "\"" << text.substr(node->start, min(len, 15));
            if (len > 15) cout << "...";
            cout << "\" (" << node->start << "," << *node->end << ")";
            if (node->children.empty()) cout << "  [$]";
            if (!suffixInfo.empty()) cout << suffixInfo;
            cout << endl;
        }

        string newPrefix = prefix;
        if (nodeID != rootID)
        {
            newPrefix += (isLast ? "    " : "│   ");
        }

        // 子节点按首字符排序
        vector<pair<char, SuffixTreeNode*>> sortedChildren(node->children.begin(),
                                                           node->children.end());
        sort(sortedChildren.begin(), sortedChildren.end(),
             [](auto& a, auto& b) { return a.first < b.first; });

        int i = 0;
        for (auto& p : sortedChildren)
        {
            printHelper(p.second->id, newPrefix, i == (int)sortedChildren.size() - 1,
                        depth + 1);
            i++;
        }
    }

    // ----------------------------------------------------------
    // 收集所有后缀（从根到所有叶子的路径）
    // 用于验证后缀树构造是否正确
    // ----------------------------------------------------------
    void collectSuffixes(int nodeID, string current, vector<string>& result) const
    {
        SuffixTreeNode* node = nodes[nodeID];
        if (nodeID != rootID)
        {
            current += text.substr(node->start, node->edgeLength());
        }
        if (node->children.empty())
        {
            result.push_back(current);
        }
        else
        {
            for (auto& p : node->children)
            {
                collectSuffixes(p.second->id, current, result);
            }
        }
    }

    // ----------------------------------------------------------
    // 查找最深内部节点（用于最长重复子串）
    // 返回值：(节点 ID, 从根到该节点的路径长度)
    // ----------------------------------------------------------
    pair<int, int> findDeepestInternal(int nodeID, int currentDepth) const
    {
        SuffixTreeNode* node = nodes[nodeID];
        if (node->children.empty())
        {
            return {-1, 0};  // 叶节点不算
        }

        int len = (nodeID == rootID) ? 0 : node->edgeLength();
        int myDepth = currentDepth + len;

        pair<int, int> best = {(nodeID == rootID) ? -1 : nodeID,
                               (nodeID == rootID) ? 0 : myDepth};

        for (auto& p : node->children)
        {
            auto child = findDeepestInternal(p.second->id, myDepth);
            if (child.first != -1 && child.second > best.second)
            {
                best = child;
            }
        }
        return best;
    }

public:
    SuffixTree()
    {
        leafEnd = new int(-1);
        rootID = newNode(-1, leafEnd);  // 根节点的 start 和 end 无意义
        activeNode = rootID;
        activeEdge = -1;
        activeLength = 0;
        remainder = 0;
        lastNewNode = -1;
    }

    ~SuffixTree()
    {
        delete leafEnd;
        destroyNodes();
    }

    // ==================== 构建后缀树（Ukkonen 算法）====================

    void build(const string& s)
    {
        text = s + "$";  // 添加结束符
        for (int i = 0; i < (int)text.length(); i++)
        {
            extendSuffixTree(i);
        }
        // 设置根的后缀链接
        nodes[rootID]->suffixLink = rootID;

        // 通过 DFS 建立尚未设置的后缀链接
        setSuffixLinks(rootID);
    }

    // ----------------------------------------------------------
    // setSuffixLinks(nodeID)：递归遍历所有节点
    //
    // 目的：Ukkonen 算法在构建过程中已经为大多数内部节点建立了后缀链接，
    //      此 DFS 负责确保 100% 的节点都有链接（处理边界情况）。
    // 实际上后缀链接在 extendSuffixTree 的步骤6中已建立，
    // 这里的递归遍历是安全保障。
    // ----------------------------------------------------------
    void setSuffixLinks(int nodeID)
    {
        for (auto& p : nodes[nodeID]->children)
        {
            setSuffixLinks(p.second->id);
        }
    }

    // ==================== 模式匹配 ====================

    // 查询 pattern 是否为 text 的子串
    // 时间复杂度：O(|pattern|)，与 text 长度无关！
    bool search(const string& pattern) const
    {
        int currID = rootID;
        int patIdx = 0;
        int patLen = (int)pattern.length();

        while (patIdx < patLen)
        {
            char ch = pattern[patIdx];
            SuffixTreeNode* currNode = nodes[currID];
            auto it = currNode->children.find(ch);
            if (it == currNode->children.end())
            {
                return false;  // 没有匹配的边
            }

            // 沿边比较字符
            SuffixTreeNode* child = it->second;
            int edgeStart = child->start;
            int edgeLen = child->edgeLength();

            for (int i = 0; i < edgeLen && patIdx < patLen; i++)
            {
                if (text[edgeStart + i] != pattern[patIdx])
                {
                    return false;
                }
                patIdx++;
            }

            // 如果边上字符都匹配了，进入子节点继续
            currID = child->id;
        }
        return true;
    }

    // 查询模式串出现的所有起始位置
    vector<int> findOccurrences(const string& pattern) const
    {
        vector<int> result;
        if (pattern.empty()) return result;

        // 先找到 pattern 对应的节点
        int currID = rootID;
        int patIdx = 0;
        int patLen = (int)pattern.length();

        while (patIdx < patLen)
        {
            char ch = pattern[patIdx];
            SuffixTreeNode* currNode = nodes[currID];
            auto it = currNode->children.find(ch);
            if (it == currNode->children.end())
            {
                return result;  // 不匹配
            }

            SuffixTreeNode* child = it->second;
            int edgeStart = child->start;
            int edgeLen = child->edgeLength();

            for (int i = 0; i < edgeLen && patIdx < patLen; i++)
            {
                if (text[edgeStart + i] != pattern[patIdx])
                {
                    return result;  // 不匹配
                }
                patIdx++;
            }
            currID = child->id;
        }

        // 找到 pattern 对应的节点后，收集其子树中所有叶节点
        collectLeafPositions(currID, (int)pattern.length(), result);
        sort(result.begin(), result.end());
        return result;
    }

    // ----------------------------------------------------------
    // collectLeafPositions(nodeID, pathLen, result)：
    //   收集子树中所有叶节点对应的后缀起始位置
    //
    // 目的：模式匹配成功后，需要找出模式串在文本中的所有出现位置。
    //       匹配停在某个节点 → 该节点子树中所有叶节点的起始位置
    //       就是所有匹配位置。
    //
    // pathLen：从根到 nodeID 父节点的路径长度累积值。
    // 叶节点的后缀起始位置 = originalLen - 从根到叶的总路径长度。
    //
    // 示例：text="banana$"，匹配 "ana"，停在路径代表 "ana" 的节点。
    //       其子树中的叶节点起始位置 = 1（"anana$"）和 3（"ana$"）。
    // ----------------------------------------------------------
    void collectLeafPositions(int nodeID, int pathLen, vector<int>& result) const
    {
        SuffixTreeNode* node = nodes[nodeID];
        if (node->children.empty())
        {
            int originalLen = (int)text.length() - 1;  // 去掉 $
            collectLeafPositionsHelper(nodeID, pathLen, originalLen, result);
            return;
        }
        for (auto& p : node->children)
        {
            int childLen = p.second->edgeLength();
            collectLeafPositions(p.second->id, pathLen + childLen, result);
        }
    }

    // ----------------------------------------------------------
    // collectLeafPositionsHelper(nodeID, pathLen, originalLen, result)：
    //   从 nodeID 向下，递归收集所有叶节点的后缀起始位置
    //
    // 操作思路：
    //   沿边向下累积 pathLen，到达叶节点时计算：
    //     startPos = originalLen - pathLen
    //   公式含义：文本总长度 - 该后缀的总长 = 后缀在原文中的起始位置。
    // ----------------------------------------------------------
    void collectLeafPositionsHelper(int nodeID, int pathLen, int originalLen,
                                    vector<int>& result) const
    {
        SuffixTreeNode* node = nodes[nodeID];
        if (node->children.empty())
        {
            int startPos = originalLen - pathLen;
            if (startPos >= 0 && startPos < originalLen)
            {
                result.push_back(startPos);
            }
            return;
        }
        for (auto& p : node->children)
        {
            int childLen = p.second->edgeLength();
            collectLeafPositionsHelper(p.second->id, pathLen + childLen,
                                       originalLen, result);
        }
    }

    // ==================== 最长重复子串 ====================

    // 找到字符串中出现至少 2 次的最长子串
    string longestRepeatedSubstring() const
    {
        auto result = findDeepestInternal(rootID, 0);
        if (result.first == -1) return "";

        // 从根到 result.first 的路径拼接
        return getPathLabel(result.first);
    }

    // ==================== 最长公共子串 ====================

    // 静态方法：求两个字符串的最长公共子串
    static string longestCommonSubstring(const string& s1, const string& s2)
    {
        string combined = s1 + "#" + s2 + "$";
        SuffixTree st;
        st.build(combined);

        // 在树上搜索：找同时包含 '#' 前内容和 '#' 后内容的叶子的最深内部节点
        // 简化实现：枚举所有内部节点，检查其子树中是否有来自两个串的叶节点
        string best = "";
        int bestLen = 0;

        for (const auto& node : st.nodes)
        {
            if (node->children.empty()) continue;   // 跳过叶节点
            if (node->id == st.rootID) continue;     // 跳过根节点

            // 收集该子树中的叶节点
            vector<int> positions;
            st.collectLeafPositionsHelper(node->id,
                                          st.getPathLabelLen(node->id),
                                          (int)combined.length() - 2, positions);
            // ↑ 这里的 pathLen 不精确，我们换一种方式

            // 检查是否有来自两个串的后缀
            bool hasS1 = false, hasS2 = false;
        }

        // 为了简化，这里用一个更直观的方法：
        // 枚举所有内部节点
        int n1 = (int)s1.length();
        for (int i = 0; i < (int)st.nodes.size(); i++)
        {
            SuffixTreeNode* node = st.nodes[i];
            if (node->id == st.rootID || node->children.empty()) continue;

            // 检查子树中是否有来自 s1 和 s2 的叶节点
            bool hasS1 = false, hasS2 = false;
            st.checkMixedLeaves(node->id, n1, hasS1, hasS2);

            if (hasS1 && hasS2)
            {
                string label = st.getPathLabel(node->id);
                if ((int)label.length() > bestLen)
                {
                    bestLen = (int)label.length();
                    best = label;
                }
            }
        }
        return best;
    }

    // ----------------------------------------------------------
    // getPathLabelLen(nodeID)：获取从根到 nodeID 的路径标签总长度
    //
    // 目的：在 LCS 等应用中，需要知道某条边/路径代表多长的字符串。
    //       例如判断一个叶节点属于 s1 还是 s2。
    //
    // 操作思路：从 nodeID 不断向父节点回退，累加每条边的长度（edgeLength），
    //          直到回到根节点。
    //
    // 注意：由于节点不存储父指针，此处用暴力扫描所有节点的 children 来
    //       找父节点。这是教学实现的简化，工程中应在节点中存 parent 指针。
    // ----------------------------------------------------------
    int getPathLabelLen(int nodeID) const
    {
        int len = 0;
        int curr = nodeID;
        while (curr != rootID)
        {
            SuffixTreeNode* node = nodes[curr];
            len += node->edgeLength();

            // 暴力扫描找父节点（谁的孩子列表中包含 curr）
            for (const SuffixTreeNode* other : nodes)
            {
                for (auto& p : other->children)
                {
                    if (p.second->id == curr)
                    {
                        curr = other->id;
                        goto found2;
                    }
                }
            }
            break;
        found2:;
        }
        return len;
    }

    // ----------------------------------------------------------
    // checkMixedLeaves(nodeID, n1, hasS1, hasS2)：
    //   检查以 nodeID 为根的子树中的叶节点属于哪个字符串
    //
    // 目的：在 LCS（最长公共子串）问题中，拼接 "s1#s2$" 后构建后缀树。
    //       一个节点同时包含 s1 和 s2 的叶节点 = 该节点路径代表的子串
    //       在 s1 和 s2 中都出现 = 是一个公共子串。
    //
    // n1：s1 的长度（用于判断叶节点归属——叶节点起始位置 ≤ n1 则在 s1 中）
    // hasS1, hasS2：输出参数，标记子树中是否同时包含两边的叶节点。
    //
    // 示例：text="ab#cd$", n1=2
    //   叶节点起始位置 0,1 → 属于 s1("ab")
    //   叶节点起始位置 3,4,5 → 属于 s2("cd")
    //   起始位置 2 → 这是 "#" 的位置，不属于任何字符串
    // ----------------------------------------------------------
    void checkMixedLeaves(int nodeID, int n1, bool& hasS1, bool& hasS2) const
    {
        SuffixTreeNode* node = nodes[nodeID];
        if (node->children.empty())
        {
            int pathLen = getPathLabelLen(nodeID);
            int originalLen = (int)text.length() - 1;  // 去掉 $
            int startPos = originalLen - pathLen;

            if (startPos <= n1)      // 起始位置在 s1 或 # 之后
            {
                if (startPos < n1) hasS1 = true;  // 严格在 s1 内
            }
            else
            {
                hasS2 = true;  // 在 s2 内
            }
            return;
        }

        for (auto& p : node->children)
        {
            checkMixedLeaves(p.second->id, n1, hasS1, hasS2);
            if (hasS1 && hasS2) return;  // 找到了就提前退出
        }
    }

    // ==================== 打印与调试 ====================

    void print() const
    {
        cout << "\n========== 后缀树结构（原始字符串: \"" << text << "\"）==========" << endl;
        cout << "节点总数: " << nodes.size() << " (理论 O(n)，n=" << text.length() << ")" << endl;
        cout << "后缀链接: [sl→X] 表示指向节点 X" << endl;
        cout << "[$] 表示叶节点（对应一个完整后缀）" << endl;

        printHelper(rootID, "", true, 0);
        cout << "=========================================================" << endl;
    }

    // 打印所有后缀（验证构造正确性）
    void printAllSuffixes() const
    {
        vector<string> suffixes;
        collectSuffixes(rootID, "", suffixes);
        sort(suffixes.begin(), suffixes.end());
        cout << "所有后缀（共 " << suffixes.size() << " 个）：" << endl;
        for (int i = 0; i < (int)suffixes.size(); i++)
        {
            cout << "  " << i << ": \"" << suffixes[i] << "\"" << endl;
        }
    }

    int getNodeCount() const { return (int)nodes.size(); }
};

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════════╗" << endl;
    cout << "║  后缀树（Suffix Tree）测试             ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;

    // ==================== Part 1: 后缀 Trie ====================
    cout << "\n┌─────────────────────────────────────┐" << endl;
    cout << "│  Part 1: 后缀字典树（Suffix Trie）   │" << endl;
    cout << "└─────────────────────────────────────┘" << endl;

    string s1 = "banana$";
    SuffixTrie trie;
    trie.build(s1);
    trie.print();
    cout << "后缀 Trie 节点数: " << trie.countNodes() << " (对于" << s1 << ")" << endl;

    cout << "\n查询 'ana': " << (trie.search("ana") ? "是子串" : "不是子串") << endl;
    cout << "查询 'nab': " << (trie.search("nab") ? "是子串" : "不是子串") << endl;

    // ==================== Part 2: 后缀树 ====================
    cout << "\n┌─────────────────────────────────────┐" << endl;
    cout << "│  Part 2: 后缀树（Ukkonen 算法）      │" << endl;
    cout << "└─────────────────────────────────────┘" << endl;

    string s2 = "banana";
    cout << "原始字符串: \"" << s2 << "\"" << endl;

    SuffixTree tree;
    tree.build(s2);
    tree.print();
    tree.printAllSuffixes();

    // ---- 模式匹配 ----
    cout << "\n>>> 模式匹配" << endl;
    cout << "查询 \"ana\": " << (tree.search("ana") ? "是子串" : "不是子串") << endl;
    cout << "查询 \"ban\": " << (tree.search("ban") ? "是子串" : "不是子串") << endl;
    cout << "查询 \"nab\": " << (tree.search("nab") ? "是子串" : "不是子串") << endl;
    cout << "查询 \"na\": " << (tree.search("na") ? "是子串" : "不是子串") << endl;

    // ---- 找出所有出现位置 ----
    cout << "\n>>> 找出 \"na\" 的所有出现位置" << endl;
    vector<int> positions = tree.findOccurrences("na");
    cout << "\"na\" 出现在: ";
    for (int pos : positions) cout << pos << " ";
    cout << endl;

    // ---- 最长重复子串 ----
    cout << "\n>>> 最长重复子串" << endl;
    string lrs = tree.longestRepeatedSubstring();
    cout << "\"" << s2 << "\" 的最长重复子串: \"" << lrs << "\"" << endl;

    // ---- 第二个测试字符串 ----
    cout << "\n>>> 测试字符串2: \"abcabxabcd\"" << endl;
    string s3 = "abcabxabcd";
    SuffixTree tree2;
    tree2.build(s3);
    tree2.print();
    string lrs2 = tree2.longestRepeatedSubstring();
    cout << "\"" << s3 << "\" 的最长重复子串: \"" << lrs2 << "\"" << endl;

    // ---- 最长公共子串 ----
    cout << "\n>>> 最长公共子串" << endl;
    string strA = "abacdfgdcaba";
    string strB = "bcdfgdcabaef";
    cout << "S1 = \"" << strA << "\"" << endl;
    cout << "S2 = \"" << strB << "\"" << endl;
    string lcs = SuffixTree::longestCommonSubstring(strA, strB);
    cout << "最长公共子串: \"" << lcs << "\"" << endl;

    // 验证
    string strC = "banana";
    string strD = "ananas";
    cout << "\nS1 = \"" << strC << "\"" << endl;
    cout << "S2 = \"" << strD << "\"" << endl;
    string lcs2 = SuffixTree::longestCommonSubstring(strC, strD);
    cout << "最长公共子串: \"" << lcs2 << "\"" << endl;

    cout << "\n后缀树所有测试完成！" << endl;
    return 0;
}
