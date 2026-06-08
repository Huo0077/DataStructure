// ============================================================
//                    并查集（Disjoint Set / Union-Find）
// ============================================================
//
// 【核心概念】
//   并查集用于高效管理"不相交集合"的合并与查询。
//   典型应用：判断图中两个节点是否连通、Kruskal 最小生成树等。
//
// 【两个核心操作】
//   find(x)：找到 x 所属集合的代表元素（根）。通过"路径压缩"
//            让每个节点直接指向根，大幅加速后续查找。
//
//   unionSets(x, y)：合并 x 和 y 所在的两个集合。通过"按大小
//                    合并"让较小的集合挂到较大集合的根下，控制树高。
//
// 【parent 数组的巧妙设计】
//   parent[i] < 0  → i 是根节点，|parent[i]| = 集合大小
//   parent[i] >= 0 → i 不是根，parent[i] = 父节点索引
//
//   一个数组同时存储了"父指针"和"集合大小"两个信息！
//
//   初始化：所有 parent[i] = -1（每个节点都是大小为 1 的独立集合）
//
//   示例（合并 0 和 1）：
//     parent = [-1, -1]        初始状态
//     union(0, 1):             两集合大小相同，将 0 挂到 1 下
//     parent = [1, -2]         1 是根（-2 表示大小 2），0 的父节点是 1
//
// 【路径压缩（find 中）】
//   parent[x] = find(parent[x]);
//   这一行让 x 直接指向根，将沿途的链"拍平"。
//   下次再找 x（及其子孙）时就是 O(1)。
//
//   示例：1→2→3→4（4 是根，parent = [? , 2, 3, 4, -4]）
//   执行 find(1) 后：1→4, 2→4, 3→4
//   路径被压缩，后续查找 1 只需一步。
//
// 【按大小合并（unionSets 中）】
//   比较两个根集合的大小（|parent[root]|），总是把小的挂到大的上。
//   这样树高不会超过 log n。
//
// 【时间复杂度】
//   使用了路径压缩 + 按大小合并后，均摊复杂度接近 O(α(n))，
//   其中 α 是反阿克曼函数——增长极慢，实际上可看作 O(1)。
//
// ============================================================

#include<iostream>
#include<vector>
using namespace std;

class disjointset
{
private:
    // parent[i] < 0  → i 是根，|parent[i]| = 集合大小
    // parent[i] >= 0 → parent[i] 是 i 的父节点索引
    vector<int> parent;

public:
    // 初始化：创建 size 个独立集合（每个元素自成一组）
    disjointset(int size)
    {
        parent.assign(size, -1);  // 全部初始化为 -1（每个集合大小 = 1）
    }

    // ----------------------------------------------------------
    // 查找 x 所属集合的根，同时进行路径压缩
    //
    // 路径压缩：递归时将沿途节点全部直接连到根上。
    //   递归写法等价于：
    //     if (parent[x] >= 0)
    //         parent[x] = find(parent[x]);  // 直接指向根！
    //     return parent[x] < 0 ? x : parent[x];
    // ----------------------------------------------------------
    int find(int x)
    {
        if (parent[x] < 0)
        {
            return x;  // x 就是根
        }
        else
        {
            parent[x] = find(parent[x]);  // 路径压缩：直接连到根
            return parent[x];
        }
    }

    // ----------------------------------------------------------
    // 合并 x 和 y 所在的集合（按大小合并）
    //
    // 策略：将较小集合的根挂到较大集合的根下面。
    // 因为 parent 存储的是负数大小，所以 parent[root1] < parent[root2]
    // 意味着 root1 的集合更大（负数更小 = 绝对值更大）。
    //
    // 示例：
    //   root1（大小 5）：parent[root1] = -5
    //   root2（大小 3）：parent[root2] = -3
    //   parent[root1] < parent[root2]（-5 < -3），root1 更大
    //   → root2 挂到 root1 下
    //   → parent[root1] += parent[root2] → -5 + (-3) = -8（新大小）
    //   → parent[root2] = root1（指向新父节点）
    // ----------------------------------------------------------
    void unionSets(int x, int y)
    {
        int root1 = find(x);  // 找 x 的根（同时路径压缩）
        int root2 = find(y);  // 找 y 的根（同时路径压缩）

        if (root1 == root2)
        {
            return;  // 已经在同一集合中
        }

        // 按大小合并：绝对值大的（集合大）当新根
        if (parent[root1] < parent[root2])
        {
            // root1 的集合更大，root2 挂到 root1 下
            parent[root1] += parent[root2];  // 更新 root1 的集合大小（负数相加）
            parent[root2] = root1;           // root2 不再为根，指向 root1
        }
        else
        {
            // root2 的集合 >= root1，root1 挂到 root2 下
            parent[root2] += parent[root1];
            parent[root1] = root2;
        }
    }
};
