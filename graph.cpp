// ============================================================
//      图（Graph）—— 存储、遍历、拓扑排序、关键路径、MST、最短路径
// ============================================================
//
// 【什么是图？】
//   图 G = (V, E)，V 是顶点集合，E 是边集合。
//   图比树更通用——节点之间可以任意连接，没有父子关系。
//
// 【图的分类】
//   有向图 vs 无向图：边是否有方向
//   有权图 vs 无权图：边是否带权值
//   连通图：任意两顶点间存在路径
//   强连通图（有向）：任意两顶点双向可达
//
// 【两种存储方式】
//
//   1. 邻接矩阵（Adjacency Matrix）
//      二维数组 g[i][j] = 1 或权值。
//      优点：O(1) 判断是否相连
//      缺点：O(V²) 空间，稀疏图浪费
//
//      示例：3 个顶点的有向图
//          0 → 1, 1 → 2, 2 → 0
//          0 1 0
//          0 0 1
//          1 0 0
//
//   2. 邻接表（Adjacency List）
//      每个顶点维护一个链表（或 vector），存储其邻居。
//      优点：O(V+E) 空间，适合稀疏图
//      缺点：判断是否相连需要遍历链表
//
// ============================================================
//   本文件涵盖：
//     Part 1: 邻接矩阵 + BFS / DFS 遍历
//     Part 2: 拓扑排序（AOV 网）—— Kahn BFS + DFS
//     Part 3: 关键路径（AOE 网）—— 最早/最晚开始时间
//     Part 4: 最小生成树 —— Prim + Kruskal
//     Part 5: 最短路径 —— Dijkstra + Floyd
// ============================================================

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <climits>
#include <functional>
using namespace std;

// 【阅读地图】
//   先按问题类型选入口：遍历、依赖、最小生成树、最短路或关键路径，而不是按类名顺序阅读。
//   图算法的共同不变量是“访问 / 距离 / 入度 / 并查集状态与已处理边保持一致”。
//   Dijkstra 只用于非负边权，拓扑排序只用于 DAG；先检查前置条件再解释算法结果。

// ============================================================
//   Part 1: 邻接矩阵 + BFS / DFS
// ============================================================
//
// 【DFS（深度优先）】
//   沿一条路径走到底再回溯。类似走迷宫——遇到死路就返回。
//   实现：递归（调用栈天然适合回溯）或显式 stack。
//   时间复杂度：O(V + E)（邻接表）/ O(V²)（邻接矩阵）
//
// 【BFS（广度优先）】
//   一层一层地扩展。类似水波扩散——先访问距离为 1 的，再距离为 2 的……
//   实现：队列（FIFO 保证"先近后远"）。
//   时间复杂度：同 DFS。
//
// 【DFS vs BFS 经典对比】
//   DFS：找路径存在性、拓扑排序、强连通分量、回溯搜索
//   BFS：最短路径（无权图）、层级遍历、社交网络"几度好友"
// ============================================================

class GraphMatrix
{
private:
    vector<vector<int>> adj;  // 邻接矩阵，0=无边，1=有边
    int V;                     // 顶点数

    void dfsUtil(int v, vector<bool>& visited, vector<int>& result)
    {
        visited[v] = true;
        result.push_back(v);
        for (int u = 0; u < V; u++)
        {
            if (adj[v][u] && !visited[u])
                dfsUtil(u, visited, result);
        }
    }

public:
    GraphMatrix(int n) : V(n)
    {
        adj.assign(n, vector<int>(n, 0));
    }

    void addEdge(int from, int to)
    {
        adj[from][to] = 1;
        // 无向图加下面一行：adj[to][from] = 1;
    }

    // DFS 返回访问序列
    vector<int> DFS(int start)
    {
        vector<bool> visited(V, false);
        vector<int> result;
        dfsUtil(start, visited, result);
        return result;
    }

    // BFS 返回访问序列
    vector<int> BFS(int start)
    {
        vector<bool> visited(V, false);
        vector<int> result;
        queue<int> q;

        visited[start] = true;
        q.push(start);

        while (!q.empty())
        {
            int v = q.front(); q.pop();
            result.push_back(v);

            for (int u = 0; u < V; u++)
            {
                if (adj[v][u] && !visited[u])
                {
                    visited[u] = true;
                    q.push(u);
                }
            }
        }
        return result;
    }

    void printMatrix()
    {
        cout << "  邻接矩阵 (" << V << "x" << V << "):" << endl;
        for (int i = 0; i < V; i++)
        {
            cout << "  ";
            for (int j = 0; j < V; j++)
                cout << adj[i][j] << " ";
            cout << endl;
        }
    }
};

// ============================================================
//   Part 2: 拓扑排序（AOV 网：顶点表示活动，边表示先后顺序）
// ============================================================
//
// 【什么是拓扑排序？】
//   对有向无环图（DAG）的顶点线性排序，使得每条边 u→v 中 u 都在 v 之前出现。
//   应用：课程安排（先修课 before 后续课）、编译依赖、任务调度。
//
//   注意：拓扑排序只对 DAG 有效。如果有环 → 不存在拓扑序。
//
// 【Kahn 算法（BFS 法）—— 删源点法】
//   1. 计算所有顶点的入度（in-degree）
//   2. 入度为 0 的顶点入队（它们是"源头"，没有前置依赖）
//   3. 出队一个顶点，把它加入结果，同时把它所有邻接点的入度 -1
//   4. 如果有邻接点入度变为 0，入队
//   5. 重复直到队列空
//   6. 如果结果数 < V → 图中有环！
//
//   示例：5→0, 5→2, 4→0, 4→1, 2→3, 3→1
//     入度: [2,2,1,1,0,0]（顶点 0,1,2,3 有入度）
//     初始队列: 4, 5（入度为 0）
//     出 4 → 0入度=1, 1入度=1
//     出 5 → 0入度=0 入队, 2入度=0 入队
//     出 0 → 无影响
//     出 2 → 3入度=0 入队
//     出 3 → 1入度=0 入队
//     出 1 → 结束
//     结果: 4 5 0 2 3 1 ✓
//
// 【DFS 法】
//   后序遍历（先处理所有后继，再处理当前节点），最后反转。
// ============================================================

class TopoSort
{
public:
    // Kahn 算法（BFS）
    static vector<int> kahn(vector<vector<int>>& graph, int V)
    {
        vector<int> inDegree(V, 0);
        for (int u = 0; u < V; u++)
            for (int v : graph[u])
                inDegree[v]++;

        queue<int> q;
        for (int i = 0; i < V; i++)
            if (inDegree[i] == 0)
                q.push(i);

        vector<int> result;
        while (!q.empty())
        {
            int u = q.front(); q.pop();
            result.push_back(u);
            for (int v : graph[u])
            {
                inDegree[v]--;
                if (inDegree[v] == 0)
                    q.push(v);
            }
        }

        // 有环检测：结果数 != 顶点数 → 存在环
        if ((int)result.size() != V)
        {
            cout << "  [警告] 图中有环，无法完成拓扑排序！" << endl;
            return {}; // 返回空表示失败
        }
        return result;
    }

    // DFS 法
    static vector<int> dfsTopo(vector<vector<int>>& graph, int V)
    {
        vector<bool> visited(V, false);
        vector<int> result;  // 后序存储，最后 reverse

        // 使用 lambda 递归 DFS（C++14+ 支持 auto&& 自引用）
        function<void(int)> dfs = [&](int u)
        {
            visited[u] = true;
            for (int v : graph[u])
                if (!visited[v])
                    dfs(v);
            result.push_back(u);  // 后处理：后继都处理完了再加入
        };

        for (int i = 0; i < V; i++)
            if (!visited[i])
                dfs(i);

        reverse(result.begin(), result.end());  // 后序反转 = 拓扑序
        return result;
    }
};

// ============================================================
//   Part 3: 关键路径（AOE 网：边表示活动，权值 = 持续时间）
// ============================================================
//
// 【AOE 网 vs AOV 网】
//   AOV：顶点是活动，边是先后关系 → 拓扑排序
//   AOE：边是活动（带权 = 耗时），顶点是事件（"所有入边活动完成"的时刻）
//
// 【关键路径定义】
//   从源点到汇点的最长路径（决定整个工程的最短工期）。
//   关键路径上的活动称为"关键活动"——延迟一天 = 整个工程延迟一天。
//
// 【两个核心量】
//   ve[v] = 顶点 v 的 最早发生时间（Earliest）
//         = 从 v 出发的活动最早什么时候可以开始
//         = max{ ve[u] + weight(u→v) }（取所有前驱的最大值）
//
//   vl[v] = 顶点 v 的 最晚发生时间（Latest）
//         = 在不延误总工期的前提下，v 最晚什么时候必须发生
//         = min{ vl[u] - weight(v→u) }（取所有后继的最小值）
//
//   关键活动判定：e(活动) == l(活动)
//     活动的最早开始 = ve[起点]
//     活动的最晚开始 = vl[终点] - weight
//     如果 e == l → 该活动没有任何缓冲时间 → 关键活动
//
// 【计算流程】
//   1. 拓扑排序确定顶点顺序
//   2. 正向计算 ve：按拓扑序，对每个顶点的每条出边松弛
//   3. 反向计算 vl：汇点 vl = ve[汇点]，按逆拓扑序对入边松弛
//   4. 计算 e 和 l，找出 e == l 的关键活动
//
//   示例：简单工程网络
//        0 --2--> 1 --3--> 3
//         \         /
//          0 --4--> 2 --1--> 3
//   ve[0]=0, ve[1]=max(ve[0]+2)=2, ve[2]=max(ve[0]+4)=4
//   ve[3]=max(ve[1]+3, ve[2]+1)=max(5,5)=5（关键路径长=5）
//   vl[3]=5, vl[2]=vl[3]-1=4, vl[1]=vl[3]-3=2, vl[0]=0
//   活动0→1: e=ve[0]=0, l=vl[1]-2=0 → e==l → 关键
//   活动0→2: e=ve[0]=0, l=vl[2]-4=0 → e==l → 关键
//   活动1→3: e=ve[1]=2, l=vl[3]-3=2 → e==l → 关键
//   活动2→3: e=ve[2]=4, l=vl[3]-1=4 → e==l → 关键
//   所有活动都是关键 → 两条路径都关键！
// ============================================================

class CriticalPath
{
public:
    // 返回：{关键路径长度, 关键活动列表}
    // edges: {from, to, weight}
    static pair<int, vector<string>> compute(int V, vector<tuple<int,int,int>> edges)
    {
        // 构建邻接表（正向）和逆邻接表（反向）
        vector<vector<pair<int,int>>> adj(V);     // adj[u] = {v, w}
        vector<vector<pair<int,int>>> rev(V);     // rev[v] = {u, w}
        vector<int> inDegree(V, 0);

        for (auto& [u, v, w] : edges)
        {
            adj[u].push_back({v, w});
            rev[v].push_back({u, w});
            inDegree[v]++;
        }

        // Step 1: 拓扑排序
        queue<int> q;
        for (int i = 0; i < V; i++)
            if (inDegree[i] == 0) q.push(i);

        vector<int> topoOrder;
        vector<int> ve(V, 0);
        while (!q.empty())
        {
            int u = q.front(); q.pop();
            topoOrder.push_back(u);
            for (auto& [v, w] : adj[u])
            {
                // 正向松弛：ve[v] = max(ve[v], ve[u] + w)
                ve[v] = max(ve[v], ve[u] + w);
                if (--inDegree[v] == 0) q.push(v);
            }
        }

        if ((int)topoOrder.size() != V)
        {
            cout << "  [错误] AOE 网中存在环！" << endl;
            return {0, {}};
        }

        // Step 2: 反向计算 vl
        int sink = topoOrder.back();  // 汇点
        vector<int> vl(V, INT_MAX);
        vl[sink] = ve[sink];  // 汇点的 vl = ve

        for (int i = (int)topoOrder.size() - 1; i >= 0; i--)
        {
            int v = topoOrder[i];
            for (auto& [u, w] : rev[v])
            {
                // 反向松弛：vl[u] = min(vl[u], vl[v] - w)
                vl[u] = min(vl[u], vl[v] - w);
            }
        }

        // Step 3: 找出关键活动
        vector<string> critical;
        for (auto& [u, v, w] : edges)
        {
            int e = ve[u];         // 活动最早开始
            int l = vl[v] - w;    // 活动最晚开始
            if (e == l)
                critical.push_back(
                    to_string(u) + "->" + to_string(v) + " w=" + to_string(w));
        }

        return {ve[sink], critical};
    }
};

// ============================================================
//   Part 4: 最小生成树（MST, Minimum Spanning Tree）
// ============================================================
//
// 【定义】
//   给定连通带权无向图，找一棵包含所有顶点的树，使边权和最小。
//   应用：铺设网络电缆、道路规划（最小成本连接所有城市）。
//
// 【Prim 算法 —— "加点法"】
//   类似 Dijkstra，维护"已选集合"和候选边的优先队列。
//   初始：任选一个顶点加入集合
//   每步：选一条从集合内到集合外的最小权边，把新顶点加入集合
//   O((V+E) log V)（用堆）或 O(V²)（朴素，适合稠密图）
//
//   示例（4 个顶点）：
//      (0)--2--(1)--3--(2)
//       |      / |      |
//       6     8  5      7
//       |   /    |      |
//      (3)--4--(4)     ...
//   Prim 从 0 开始：选 0-1(2) → 选 0-3(6) → 选 3-4(4) → 选 1-2(3)
//   MST 边集：{0-1(2), 0-3(6), 3-4(4), 1-2(3)} 总权 = 15
//
// 【Kruskal 算法 —— "加边法"】
//   把边按权值升序排序，贪心地选择不构成环的最小边。
//   判环：用并查集（两个顶点已经在同一集合 = 加入会成环）。
//   O(E log E)（排序主导），适合稀疏图。
//
//   Kruskal 示例（同一张图）：
//   排序边：0-1(2), 1-2(3), 3-4(4), 0-3(6), 1-4(5), ...
//   选 0-1(2) ✓ → 选 1-2(3) ✓ → 选 3-4(4) ✓
//   → 选 0-3(6) ✓ → 选 1-4(5) ✗（0-1-4-3-0 成环，跳过）
//   总权 = 2+3+4+6 = 15
//
// 【Prim vs Kruskal】
//   Prim：适合稠密图（邻接矩阵 O(V²)），不需要排序
//   Kruskal：适合稀疏图，实现简单（排序 + 并查集）
// ============================================================

// ---------- 简易并查集（供 Kruskal 判环用）----------
class DSU
{
    vector<int> parent, sz;
public:
    DSU(int n)
    {
        parent.resize(n);
        sz.resize(n, 1);
        for (int i = 0; i < n; i++) parent[i] = i;
    }
    int find(int x)
    {
        if (parent[x] != x)
            parent[x] = find(parent[x]);  // 路径压缩
        return parent[x];
    }
    void unite(int a, int b)
    {
        int ra = find(a), rb = find(b);
        if (ra == rb) return;
        if (sz[ra] < sz[rb]) swap(ra, rb);  // 按大小合并
        parent[rb] = ra;
        sz[ra] += sz[rb];
    }
    bool connected(int a, int b) { return find(a) == find(b); }
};

class MST
{
public:
    // Prim 算法：返回 {总权值, 边集合}
    // graph 为邻接矩阵，0 表示无边
    static pair<int, vector<string>> prim(vector<vector<int>>& graph)
    {
        int V = graph.size();
        vector<bool> inMST(V, false);
        vector<int> key(V, INT_MAX);   // key[v] = 连接 v 到 MST 的最小边权
        vector<int> parent(V, -1);     // MST 中 v 的父节点

        key[0] = 0;  // 从顶点 0 开始

        for (int count = 0; count < V; count++)
        {
            // 选 key 最小的未加入顶点
            int u = -1, minKey = INT_MAX;
            for (int i = 0; i < V; i++)
            {
                if (!inMST[i] && key[i] < minKey)
                {
                    minKey = key[i];
                    u = i;
                }
            }
            if (u == -1) break;  // 图不连通

            inMST[u] = true;

            // 更新 u 的邻居
            for (int v = 0; v < V; v++)
            {
                if (graph[u][v] && !inMST[v] && graph[u][v] < key[v])
                {
                    parent[v] = u;
                    key[v] = graph[u][v];
                }
            }
        }

        // 收集结果
        int total = 0;
        vector<string> edges;
        for (int v = 1; v < V; v++)
        {
            if (parent[v] != -1)
            {
                total += graph[parent[v]][v];
                edges.push_back(to_string(parent[v]) + "-" + to_string(v) +
                                "(" + to_string(graph[parent[v]][v]) + ")");
            }
        }
        return {total, edges};
    }

    // Kruskal 算法
    static pair<int, vector<string>> kruskal(int V, vector<tuple<int,int,int>>& edges)
    {
        // 按权值升序
        sort(edges.begin(), edges.end(),
             [](auto& a, auto& b) { return get<2>(a) < get<2>(b); });

        DSU dsu(V);
        int total = 0;
        vector<string> mstEdges;

        for (auto& [u, v, w] : edges)
        {
            if (!dsu.connected(u, v))
            {
                dsu.unite(u, v);
                total += w;
                mstEdges.push_back(to_string(u) + "-" + to_string(v) +
                                    "(" + to_string(w) + ")");
            }
        }
        return {total, mstEdges};
    }
};

// ============================================================
//   Part 5: 最短路径
// ============================================================
//
// 【Dijkstra —— 单源最短路径（非负权）】
//   从起点出发，每次选"当前已知距离最小"的未确定顶点，
//   用它的最短路去更新（松弛）它的邻居。
//
//   松弛操作：if (dist[v] > dist[u] + w)  dist[v] = dist[u] + w
//   含义："走 u 再到 v"比"当前到 v 的路线"更短 → 更新。
//
//   O(V²) 朴素 / O((V+E) log V) 堆优化。
//   不能处理负权边（负边可能让"已确定"的顶点变短）。
//
//   示例（从 0 出发）：
//      (0)--4--(1)--1--(2)
//       |      /|       |
//       2     8  2      3
//       |   /    |       |
//      (3)--7--(4)------+
//   初始: dist=[0,∞,∞,∞,∞]
//   确定0: dist=[0,4,∞,2,∞]（松弛0→1=4, 0→3=2）
//   确定3: dist=[0,4,9,2,9]（松弛3→2=9, 3→4=9）
//   确定1: dist=[0,4,5,2,6]（松弛1→2=5, 1→4=6）
//   确定2: dist=[0,4,5,2,6]（松弛2→4=8, 不更优）
//   确定4: dist=[0,4,5,2,6]
//   答案: 0→0=0, 0→1=4, 0→2=5, 0→3=2, 0→4=6
//
// 【Floyd —— 多源最短路径（可负权，无负环）】
//   动态规划：dp[k][i][j] = 使用前 k 个顶点作为中间点的 i→j 最短距离。
//   降维后：for k: for i: for j: dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j])
//
//   核心思想：每轮允许一个新的中间顶点 k 参与路径。
//   O(V³)，但代码极短且可以处理负权边（不能有负环）。
//
//   示例：dist[0][2] = min(∞, dist[0][1] + dist[1][2])
//         = min(∞, 4+1) = 5（通过中间点 1）
//
// 【Dijkstra vs Floyd vs Bellman-Ford】
//   Dijkstra：单源、非负权、O((V+E)log V)、最常用
//   Floyd：多源（所有点对）、可负权、O(V³)、负环检测
//   Bellman-Ford：单源、可负权、可检测负环、O(VE)
//   本文件实现前两种。
// ============================================================

class ShortestPath
{
public:
    // Dijkstra（朴素 O(V²)）返回 {dist数组, path数组（用于回溯路径）}
    static pair<vector<int>, vector<int>> dijkstra(vector<vector<int>>& graph, int src)
    {
        int V = graph.size();
        vector<int> dist(V, INT_MAX);
        vector<bool> visited(V, false);
        vector<int> parent(V, -1);

        dist[src] = 0;

        for (int count = 0; count < V; count++)
        {
            // 选 dist 最小的未确定顶点
            int u = -1, minDist = INT_MAX;
            for (int i = 0; i < V; i++)
            {
                if (!visited[i] && dist[i] < minDist)
                {
                    minDist = dist[i];
                    u = i;
                }
            }
            if (u == -1) break;  // 剩下的不可达

            visited[u] = true;

            // 松弛 u 的所有邻居
            for (int v = 0; v < V; v++)
            {
                if (graph[u][v] && !visited[v] && dist[u] != INT_MAX)
                {
                    int newDist = dist[u] + graph[u][v];
                    if (newDist < dist[v])
                    {
                        dist[v] = newDist;
                        parent[v] = u;  // 记录来路，用于回溯路径
                    }
                }
            }
        }
        return {dist, parent};
    }

    // 回溯最短路径
    static vector<int> getPath(vector<int>& parent, int dest)
    {
        vector<int> path;
        for (int v = dest; v != -1; v = parent[v])
            path.push_back(v);
        reverse(path.begin(), path.end());
        return path;
    }

    // Floyd-Warshall O(V³)
    // 返回 {dist矩阵, next矩阵（用于重建路径）}
    // 不可达用 INT_MAX/2 表示（避免溢出）
    static pair<vector<vector<int>>, vector<vector<int>>> floyd(vector<vector<int>>& graph)
    {
        int V = graph.size();
        vector<vector<int>> dist(V, vector<int>(V));
        vector<vector<int>> next(V, vector<int>(V, -1));

        for (int i = 0; i < V; i++)
        {
            for (int j = 0; j < V; j++)
            {
                if (i == j)
                    dist[i][j] = 0;
                else if (graph[i][j])
                {
                    dist[i][j] = graph[i][j];
                    next[i][j] = j;  // i→j 的下一步是 j
                }
                else
                    dist[i][j] = INT_MAX / 2;  // 一个很大的数，避免 +1 溢出
            }
        }

        // 核心三重循环：k 是中间点
        for (int k = 0; k < V; k++)
        {
            for (int i = 0; i < V; i++)
            {
                for (int j = 0; j < V; j++)
                {
                    if (dist[i][k] + dist[k][j] < dist[i][j])
                    {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        next[i][j] = next[i][k];  // i→j 要先经过 i→k 的下一步
                    }
                }
            }
        }
        return {dist, next};
    }
};

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════════╗" << endl;
    cout << "║     图 论 综 合 测 试               ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;

    // ============ Part 1: BFS / DFS ============
    cout << "\n===== Part 1: 遍历（DFS & BFS）=====" << endl;

    // 构建示例图（有向）
    // 0 → 1, 0 → 2, 1 → 2, 2 → 0, 2 → 3, 3 → 3（自环）
    GraphMatrix g(4);
    g.addEdge(0, 1); g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(2, 0); g.addEdge(2, 3);
    g.addEdge(3, 3);
    g.printMatrix();

    vector<int> dfsResult = g.DFS(2);
    cout << "DFS(从2出发): ";
    for (int v : dfsResult) cout << v << " ";
    cout << "(expect: 2 0 1 3)" << endl;

    vector<int> bfsResult = g.BFS(2);
    cout << "BFS(从2出发): ";
    for (int v : bfsResult) cout << v << " ";
    cout << "(expect: 2 0 3 1)" << endl;

    // ============ Part 2: 拓扑排序 ============
    cout << "\n===== Part 2: 拓扑排序（AOV 网）=====" << endl;

    // 课程依赖图：6 个顶点（0~5 代表 6 门课）
    // 5→2, 5→0, 4→0, 4→1, 2→3, 3→1
    int V = 6;
    vector<vector<int>> graph(V);
    graph[5].push_back(2); graph[5].push_back(0);
    graph[4].push_back(0); graph[4].push_back(1);
    graph[2].push_back(3);
    graph[3].push_back(1);

    cout << "Kahn 拓扑序: ";
    auto kahnResult = TopoSort::kahn(graph, V);
    for (int v : kahnResult) cout << v << " ";
    cout << "(expect: 4 5 0 2 3 1)" << endl;

    cout << "DFS 拓扑序: ";
    auto dfsResult2 = TopoSort::dfsTopo(graph, V);
    for (int v : dfsResult2) cout << v << " ";
    cout << "(expect: 5 4 2 3 1 0 或类似)" << endl;

    // ============ Part 3: 关键路径（AOE）= ============
    cout << "\n===== Part 3: 关键路径（AOE 网）=====" << endl;

    // 简单工程网络（5 个事件 0~4）
    // 0--2-->1--3-->3
    // 0--4-->2--1-->3
    // 3--2-->4
    // 0--5-->4
    cout << "工程网络（边=活动，权=天数）：" << endl;
    cout << "  0 -2-> 1 -3-> 3 -2-> 4" << endl;
    cout << "  0 -4-> 2 -1-> 3" << endl;
    cout << "  0 -5-> 4" << endl;

    vector<tuple<int,int,int>> aoe = {
        {0,1,2}, {1,3,3}, {0,2,4}, {2,3,1}, {3,4,2}, {0,4,5}
    };
    auto [cpLen, criticalActs] = CriticalPath::compute(5, aoe);
    cout << "关键路径长度 = " << cpLen << " (expect 7)" << endl;
    cout << "关键活动: ";
    for (auto& s : criticalActs) cout << "[" << s << "] ";
    cout << endl;
    // 路径 0→2→3→4 = 4+1+2 = 7

    // ============ Part 4: MST ============
    cout << "\n===== Part 4: 最小生成树（MST）=====" << endl;

    // 5 个顶点的带权无向图
    vector<vector<int>> mstGraph = {
        {0, 2, 0, 6, 0},
        {2, 0, 3, 8, 5},
        {0, 3, 0, 0, 7},
        {6, 8, 0, 0, 4},
        {0, 5, 7, 4, 0}
    };

    cout << "Prim: ";
    auto [primTotal, primEdges] = MST::prim(mstGraph);
    cout << "总权 = " << primTotal << " (expect 14)" << endl;
    cout << "  边: ";
    for (auto& e : primEdges) cout << e << " ";
    cout << endl;

    cout << "Kruskal: ";
    vector<tuple<int,int,int>> edges;
    for (int i = 0; i < 5; i++)
        for (int j = i + 1; j < 5; j++)
            if (mstGraph[i][j])
                edges.push_back({i, j, mstGraph[i][j]});
    auto [kruskalTotal, kruskalEdges] = MST::kruskal(5, edges);
    cout << "总权 = " << kruskalTotal << " (expect 14)" << endl;
    cout << "  边: ";
    for (auto& e : kruskalEdges) cout << e << " ";
    cout << endl;

    // ============ Part 5: 最短路径 ============
    cout << "\n===== Part 5: 最短路径 =====" << endl;

    // Dijkstra
    cout << "\nDijkstra (从 0 出发):" << endl;
    vector<vector<int>> spGraph = {
        {0, 4, 0, 2, 0},
        {0, 0, 1, 8, 2},
        {0, 0, 0, 0, 3},
        {0, 0, 7, 0, 7},
        {0, 0, 0, 0, 0}
    };
    auto [dist, parent] = ShortestPath::dijkstra(spGraph, 0);
    for (int i = 0; i < 5; i++)
    {
        cout << "  0→" << i << " = " << dist[i];
        auto path = ShortestPath::getPath(parent, i);
        cout << "  路径: ";
        for (int v : path) cout << v << " ";
        cout << endl;
    }
    // expect: 0→0=0, 0→1=4, 0→2=5, 0→3=2, 0→4=6

    // Floyd
    cout << "\nFloyd-Warshall (所有点对):" << endl;
    auto [fwDist, fwNext] = ShortestPath::floyd(spGraph);
    cout << "  距离矩阵 (0~4):" << endl;
    for (int i = 0; i < 5; i++)
    {
        cout << "  ";
        for (int j = 0; j < 5; j++)
            cout << (fwDist[i][j] >= INT_MAX/4 ? -1 : fwDist[i][j]) << "\t";
        cout << endl;
    }

    cout << "\n所有测试完成！" << endl;
    return 0;
}
