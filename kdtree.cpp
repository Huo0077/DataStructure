// ============================================================
//    KD 树（K-Dimensional Tree）—— 多维空间的高效数据结构
// ============================================================
//
// 【什么是 KD 树？】
//   KD 树（K-Dimensional Tree）是二叉搜索树在 k 维空间的推广，
//   由 Jon Louis Bentley 于 1975 年发明。它通过对 k 维空间的
//   递归划分，将点集组织成一棵二叉树，支持高效的多维查询。
//
//   每个节点代表 k 维空间中的一个"超矩形区域"（bounding box），
//   节点的分裂线平行于某个坐标轴，将空间一分为二。
//
//   名称含义：K = 维度数，D = Dimensional（维度）。
//
//   ┌─────────────── 2D KD 树的分裂示意 ───────────────┐
//   │                                                    │
//   │    第一层（按 x 轴分裂）:                           │
//   │    ┌─────────────┬─────────────┐                   │
//   │    │   x ≤ 5     │   x > 5     │                   │
//   │    │             │             │                   │
//   │    第二层（按 y 轴分裂）:       第二层（按 y 轴分裂）│
//   │    ┌─────┬─────┐ ┌─────┬─────┐                    │
//   │    │y ≤ 3│y > 3│ │y ≤ 4│y > 4│                    │
//   │    └─────┴─────┘ └─────┴─────┘                    │
//   │                                                    │
//   └────────────────────────────────────────────────────┘
//
// 【KD 树的核心思想】
//
//   1. 递归划分空间：
//      每一层选择一个维度，在该维度上找到中位数点，
//      用中位数点作为分裂点，将空间分为左右两个半空间。
//
//   2. 维度轮换：
//      根节点按第 0 维（x 轴）分裂
//      深度 1 按第 1 维（y 轴）分裂
//      ...
//      深度 d 按第 (d % k) 维分裂
//
//   3. 每个节点存储的不只是点数据，还隐含了空间区域信息：
//      - 左子树：当前维度的值 ≤ 分裂值 的区域
//      - 右子树：当前维度的值 > 分裂值 的区域
//
// 【时间复杂度分析】
//
//   构建：O(n log n) — 每层找中位数 O(n)，共 log n 层
//         （使用 nth_element 可以做到 O(n log n)）
//   最近邻搜索：平均 O(log n)，最坏 O(n)
//              （实践中非常快，因为大量分支被剪枝）
//   范围搜索：平均 O(log n + m)，m 为报告的点数
//   插入/删除：O(log n) 平均，但不保证绝对平衡
//
// 【最近邻搜索（Nearest Neighbor）—— KD 树的精髓】
//
//   给定查询点 Q，在 KD 树中找离 Q 最近的点。
//
//   算法（递归）：
//     1. 从根节点出发，按分裂维度选择走向左还是右，
//        直到叶节点，记录当前最近距为 minDist
//     2. 回溯：检查另一侧子树是否需要访问。
//        先看 Q 到另一侧子树对应区域的"分割超平面"的距离。
//        如果这个距离 ≥ minDist → 剪枝！（另一侧不可能有更近的点）
//        如果这个距离 < minDist → 必须递归搜索另一侧。
//
//   为什么能剪枝？
//     因为如果查询点到分裂超平面的距离已经超过了当前最佳距离，
//     那么另一侧整个半空间中的点都至少有这么远，不可能更近。
//
//   示例（2D）：
//     查询点 Q(3, 4)，当前最近距 minDist = 2
//     当前所在分裂线 x=5，Q 在左侧
//     Q 到分裂线 x=5 的距离 = |3-5| = 2
//     2 ≥ minDist=2 → 右半空间可以剪枝！
//     （右半空间所有点 x ≥ 5，到 Q 的最短距离 ≥ 2）
//
//   如果 Q 到超平面距离 < minDist：
//     → 不能剪枝，因为另一侧可能存在更近的点。
//
// 【范围搜索（Range Search / Orthogonal Range Query）】
//
//   查询落在超矩形 [low[0], high[0]] × [low[1], high[1]] × ... 内的所有点。
//
//   递归时，检查当前节点代表的区域是否与查询范围相交：
//     - 不相交 → 剪枝
//     - 完全包含 → 报告中序遍历所有点
//     - 部分相交 → 递归检查左右子树
//
// 【选择分裂维度的策略】
//
//   1. 轮换（Round-Robin）：depth % k —— 最简单，本实现采用
//   2. 最大方差（Max Variance）：选点集分布最散的维度 —— 更平衡
//   3. 最大范围（Max Spread）：选范围 (max-min) 最大的维度
//
// 【KD 树的典型应用】
//
//   1. 最近邻搜索（Nearest Neighbor）
//      → 图像检索、模式识别、推荐系统
//
//   2. K 近邻（K-Nearest Neighbors, KNN）
//      → 机器学习分类/回归算法
//
//   3. 范围查询（Range Search）
//      → 地理信息系统（GIS）："找出我周围 5km 内的所有餐厅"
//      → 数据库中的多维索引
//
//   4. 碰撞检测（Collision Detection）
//      → 游戏物理引擎、机器人路径规划
//
//   5. 光线追踪（Ray Tracing）
//      → 3D 渲染中的加速结构
//
// 【KD 树 vs 其他空间数据结构】
//
//   ┌──────────┬────────────┬─────────────┬──────────────┐
//   │          │  KD 树      │  四叉树      │  R 树        │
//   ├──────────┼────────────┼─────────────┼──────────────┤
//   │ 划分方式  │ 轴对齐超平面 │ 四等分空间   │ 重叠矩形      │
//   │ 树结构    │ 二叉树      │ 4 叉树       │ 多路平衡树    │
//   │ 适合维度  │ 中低维(≤20) │ 2D           │ 中高维 + 磁盘 │
//   │ 平衡性    │ 构建时平衡  │ 取决于分布   │ 保证平衡      │
//   │ 典型应用  │ KNN, 范围   │ 游戏, GIS    │ 数据库空间索引│
//   └──────────┴────────────┴─────────────┴──────────────┘
//
// 【维度诅咒（Curse of Dimensionality）】
//   KD 树在高维（>20 维）空间中性能急剧下降。原因：
//   - 高维空间中所有点几乎等距，剪枝效率极低
//   - 覆盖所有维度需要 2^k 个分裂面
//   解决方案：LSH（局部敏感哈希）、ANN（近似最近邻）、球树等
//
// ============================================================

#include<iostream>
#include<vector>
#include<algorithm>
#include<cmath>
#include<queue>
#include<limits>
#include<functional>
#include<string>
#include<sstream>
using namespace std;

// ==================== K 维点 ====================

struct Point
{
    vector<double> coords;   // 坐标数组：coords[0]=x, coords[1]=y, coords[2]=z, ...
    int id;                   // 可选的标识符（用于索引原数据集）

    Point() : id(-1) {}
    Point(const vector<double>& c, int i = -1) : coords(c), id(i) {}

    double operator[](int dim) const { return coords[dim]; }
    double& operator[](int dim) { return coords[dim]; }
    int dims() const { return (int)coords.size(); }

    // 两点间欧几里得距离（平方）
    double distanceSq(const Point& other) const
    {
        double sum = 0;
        for (int i = 0; i < (int)coords.size(); i++)
        {
            double diff = coords[i] - other.coords[i];
            sum += diff * diff;
        }
        return sum;
    }

    double distance(const Point& other) const
    {
        return sqrt(distanceSq(other));
    }

    string str() const
    {
        string s = "(";
        for (int i = 0; i < (int)coords.size(); i++)
        {
            s += to_string(coords[i]);
            if (i < (int)coords.size() - 1) s += ", ";
        }
        s += ")";
        return s;
    }
};

// ==================== KD 树节点 ====================

struct KDNode
{
    Point point;              // 该节点存储的点
    KDNode* left;             // 左子树（当前维度 ≤ 分裂值的区域）
    KDNode* right;            // 右子树（当前维度 > 分裂值的区域）
    int splitDim;             // 该节点的分裂维度（0, 1, 2, ..., k-1）

    KDNode(const Point& p, int dim)
        : point(p), left(nullptr), right(nullptr), splitDim(dim) {}
};

// ==================== KD 树类 ====================

class KDTree
{
private:
    KDNode* root;
    int k;                    // 维度数
    int size;                 // 树中点的总数

    // KNN 距离堆的比较器：只比较距离，不做 Point 的 tie-break
    // （避免 Point 没有 operator< 导致的编译错误）
    struct DistCmp { bool operator()(const pair<double,Point>& a, const pair<double,Point>& b) const { return a.first < b.first; } };

    // ----------------------------------------------------------
    // 构建 KD 树（递归）
    //
    // 参数：
    //   points: 当前子集的点（按值传递，因为 nth_element 会改变顺序）
    //   depth:  当前递归深度 → splitDim = depth % k
    //
    // 算法：
    //   1. 如果点集为空 → 返回 nullptr
    //   2. 确定分裂维度 = depth % k
    //   3. 在当前维度上找到中位数点
    //      （使用 nth_element：O(n) 找到中位数，并部分排序）
    //   4. 中位数点成为当前节点
    //   5. 中位数左边的点 → 左子树递归
    //   6. 中位数右边的点 → 右子树递归
    //
    //   为什么要找中位数？
    //     保证左右子树点数尽量相等 → 树平衡 → 高度 O(log n)
    //     如果用任意点（如第一个点），树可能退化成链表。
    //
    //   nth_element 解释：
    //     将第 mid 小的元素放到 mid 位置，左边的都 ≤ 它，右边的都 ≥ 它。
    //     时间复杂度 O(n)，不需要完整排序 O(n log n)。
    // ----------------------------------------------------------
    KDNode* build(vector<Point>& points, int depth)
    {
        if (points.empty())
        {
            return nullptr;
        }

        int splitDim = depth % k;
        int mid = (int)points.size() / 2;

        // 在 splitDim 维度上找中位数（O(n) 部分排序）
        nth_element(points.begin(), points.begin() + mid, points.end(),
                    [splitDim](const Point& a, const Point& b)
                    {
                        return a[splitDim] < b[splitDim];
                    });

        // 中位数点成为当前节点
        KDNode* node = new KDNode(points[mid], splitDim);

        // 递归构建左右子树
        vector<Point> leftPts(points.begin(), points.begin() + mid);
        vector<Point> rightPts(points.begin() + mid + 1, points.end());

        node->left = build(leftPts, depth + 1);
        node->right = build(rightPts, depth + 1);

        return node;
    }

    // ----------------------------------------------------------
    // 判断两个点是否相同（所有坐标都相等）
    // ----------------------------------------------------------
    bool pointsEqual(const Point& a, const Point& b) const
    {
        for (int i = 0; i < k; i++)
        {
            if (a[i] != b[i]) return false;
        }
        return true;
    }

    // ----------------------------------------------------------
    // 最近邻搜索（递归核心）
    //
    // 参数：
    //   node:     当前子树根节点
    //   query:    查询点
    //   best:     当前找到的最近点（引用，会被更新）
    //   bestDist: 当前最近距离的平方（引用，会被更新）
    //
    // 算法流程（详见文件头注释中的图解）：
    //
    //   阶段 1 — 二叉搜索下降：
    //     从根出发，每次按分裂维度向左或右走到底，
    //     把访问过的节点压栈（或递归回溯时处理）。
    //     到达叶节点后，叶节点就是初始的候选最近点。
    //
    //   阶段 2 — 回溯剪枝：
    //     回溯到每个祖先节点时：
    //       a) 先用祖先节点更新 best
    //       b) 计算查询点 Q 到分裂超平面的距离（平方）：
    //          planeDistSq = (Q[splitDim] - node.point[splitDim])^2
    //       c) 如果 planeDistSq < bestDist：
    //            → 需要搜索另一侧子树（不能剪枝）
    //          else：
    //            → 剪枝！跳过另一侧
    //
    //   剪枝条件为什么是 planeDistSq < bestDist？
    //     超平面距离是另一侧区域中任意点到 Q 的 下界。
    //     如果这个下界已经 ≥ bestDist，则另一侧不可能有更近的点。
    //
    //   可视化（2D，分裂维度 = x）：
    //
    //         查询点 Q
    //           *
    //           |        bestDist=3
    //           |   ┌─────────────┐
    //     左半空间 │   │  右半空间    │
    //     (已搜索) │   │  (待决定)    │
    //              │   │             │
    //         ←───┼───┼────────→ x
    //         x=4  ↑  x=7
    //           分裂线
    //        Q 在右半空间（x=9）
    //        Q 到分裂线的距离 = |9-7| = 2
    //        2 < 3 → 左半空间不能剪枝！
    //
    //   假设 bestDist=1.5：
    //        Q 到分裂线的距离 = 2 ≥ 1.5 → 剪枝！左半空间安全跳过。
    //
    // ----------------------------------------------------------
    void nearestNeighbor(KDNode* node, const Point& query,
                         Point& best, double& bestDist) const
    {
        if (node == nullptr) return;

        // ---- 检查当前节点是否更近 ----
        double dSq = query.distanceSq(node->point);
        if (dSq < bestDist && !pointsEqual(query, node->point))
        {
            bestDist = dSq;
            best = node->point;
        }

        // ---- 决定先走哪边（二叉搜索）----
        int dim = node->splitDim;
        double diff = query[dim] - node->point[dim];
        KDNode* firstSide = (diff <= 0) ? node->left : node->right;
        KDNode* secondSide = (diff <= 0) ? node->right : node->left;

        // 先往查询点所在的那一侧递归
        nearestNeighbor(firstSide, query, best, bestDist);

        // ---- 回溯：检查是否需要搜索另一侧 ----
        // diff² = 查询点到当前节点"超平面"的距离平方
        // 如果这个距离 < 当前最佳距离 → 另一侧边界内可能有更近的点
        if (diff * diff < bestDist)
        {
            nearestNeighbor(secondSide, query, best, bestDist);
        }
        // else: 剪枝！另一侧区域中所有点的距离 ≥ |diff| ≥ √bestDist
    }

    // ----------------------------------------------------------
    // K 近邻搜索（K-Nearest Neighbors）
    //
    // 使用最大堆维护当前找到的 K 个最近点：
    //   堆顶 = 当前 K 个候选点中 最远 的那个
    //   剪枝条件：如果查询点到超平面的距离 ≥ 堆顶距离 → 剪枝
    //
    // 原理类似最近邻，但将 bestDist 替换为"第 K 近的距离"。
    // 只有当超平面距离 < 第 K 近的距离时，才需要搜索另一侧。
    //
    // 优先队列存储：(距离平方, Point)
    //   最大堆（top = 当前 K 个中最远的距离）
    // ----------------------------------------------------------
    void kNNSearch(KDNode* node, const Point& query, int K,
                   priority_queue<pair<double, Point>,
                                  vector<pair<double, Point>>, DistCmp>& maxHeap) const
    {
        if (node == nullptr) return;

        int dim = node->splitDim;

        // 检查当前节点
        double dSq = query.distanceSq(node->point);
        if (!pointsEqual(query, node->point))
        {
            if ((int)maxHeap.size() < K)
            {
                maxHeap.push({dSq, node->point});
            }
            else if (dSq < maxHeap.top().first)
            {
                maxHeap.pop();
                maxHeap.push({dSq, node->point});
            }
        }

        // 二叉搜索
        double diff = query[dim] - node->point[dim];
        KDNode* firstSide = (diff <= 0) ? node->left : node->right;
        KDNode* secondSide = (diff <= 0) ? node->right : node->left;

        kNNSearch(firstSide, query, K, maxHeap);

        // 剪枝条件：超平面距离 < 第 K 近的距离（或堆未满）
        double kthDist = maxHeap.empty() ? numeric_limits<double>::max()
                                         : maxHeap.top().first;
        if (diff * diff < kthDist || (int)maxHeap.size() < K)
        {
            kNNSearch(secondSide, query, K, maxHeap);
        }
    }

    // ----------------------------------------------------------
    // 范围搜索（Range Search）
    //
    // 查询所有落在超矩形 [low, high] 内的点。
    // 对于节点点 p：所有维度均满足 low[i] ≤ p[i] ≤ high[i]。
    //
    // 【剪枝逻辑】
    //   左子树区域：维度 dim 上的值 ∈ (-∞, node.point[dim]]
    //   右子树区域：维度 dim 上的值 ∈ (node.point[dim], +∞)
    //
    //   - 若 low[dim] > node.point[dim] → 左子树最大值 < low → 完全不相交 → 剪左
    //   - 若 high[dim] ≤ node.point[dim] → 右子树最小值 > high → 完全不相交 → 剪右
    //
    //   直观理解：查询范围完全在某子树区域的一侧 → 跳过该子树。
    // ----------------------------------------------------------
    void rangeSearch(KDNode* node, const vector<double>& low,
                     const vector<double>& high, vector<Point>& result) const
    {
        if (node == nullptr) return;

        // 检查当前节点是否在范围内
        bool inside = true;
        for (int i = 0; i < k; i++)
        {
            if (node->point[i] < low[i] || node->point[i] > high[i])
            {
                inside = false;
                break;
            }
        }
        if (inside)
        {
            result.push_back(node->point);
        }

        int dim = node->splitDim;

        // 左子树：当前维 ≤ 分裂值
        // 只有当 low[dim] ≤ node.point[dim] 时，左子树才可能包含范围内的点
        if (node->left && low[dim] <= node->point[dim])
        {
            rangeSearch(node->left, low, high, result);
        }

        // 右子树：当前维 > 分裂值
        // 只有当 high[dim] > node.point[dim] 时，右子树才可能包含范围内的点
        if (node->right && high[dim] > node->point[dim])
        {
            rangeSearch(node->right, low, high, result);
        }
    }

    // ----------------------------------------------------------
    // 半径搜索（Radius Search）
    // 查询所有到 center 的距离 ≤ radius 的点
    // 这是范围搜索的圆形/球形版本
    // ----------------------------------------------------------
    void radiusSearch(KDNode* node, const Point& center, double radius,
                      vector<pair<Point, double>>& result) const
    {
        if (node == nullptr) return;

        double dSq = center.distanceSq(node->point);
        double rSq = radius * radius;

        if (dSq <= rSq)
        {
            result.push_back({node->point, sqrt(dSq)});
        }

        int dim = node->splitDim;
        double diff = center[dim] - node->point[dim];

        // 二叉搜索：查询点所在的半空间优先
        KDNode* firstSide = (diff <= 0) ? node->left : node->right;
        KDNode* secondSide = (diff <= 0) ? node->right : node->left;

        radiusSearch(firstSide, center, radius, result);

        // 剪枝：如果超平面距离已经 > radius，另一侧不可能有点在半径内
        if (fabs(diff) < radius)
        {
            radiusSearch(secondSide, center, radius, result);
        }
    }

    // ----------------------------------------------------------
    // 插入新点（递归）
    //
    // 和 BST 插入类似：按分裂维度决定走左走右。
    // 注意：KD 树不保证插入后平衡。大量插入后可能需要重建。
    // ----------------------------------------------------------
    KDNode* insertHelper(KDNode* node, const Point& point, int depth)
    {
        if (node == nullptr)
        {
            size++;
            return new KDNode(point, depth % k);
        }

        int dim = node->splitDim;
        if (point[dim] <= node->point[dim])
        {
            node->left = insertHelper(node->left, point, depth + 1);
        }
        else
        {
            node->right = insertHelper(node->right, point, depth + 1);
        }
        return node;
    }

    // ----------------------------------------------------------
    // 寻找子树中的最小节点（在指定维度上）
    // 用于删除操作
    // ----------------------------------------------------------
    KDNode* findMin(KDNode* node, int dim, int depth)
    {
        if (node == nullptr) return nullptr;

        int currDim = depth % k;

        if (currDim == dim)
        {
            // 当前节点的分裂维度 = 要查找的维度
            // 最小值只可能在左子树中
            if (node->left == nullptr)
            {
                return node;
            }
            return findMin(node->left, dim, depth + 1);
        }
        else
        {
            // 最小值可能在左右子树中任何地方
            KDNode* leftMin = findMin(node->left, dim, depth + 1);
            KDNode* rightMin = findMin(node->right, dim, depth + 1);

            KDNode* best = node;
            if (leftMin && leftMin->point[dim] < best->point[dim])
                best = leftMin;
            if (rightMin && rightMin->point[dim] < best->point[dim])
                best = rightMin;
            return best;
        }
    }

    // ----------------------------------------------------------
    // 删除节点
    // ----------------------------------------------------------
    KDNode* removeHelper(KDNode* node, const Point& point, int depth)
    {
        if (node == nullptr) return nullptr;

        int dim = depth % k;

        if (pointsEqual(node->point, point))
        {
            // 找到了要删除的节点
            if (node->right != nullptr)
            {
                // 有右子树：找当前维度上的后继（右子树中当前维度的最小值）
                KDNode* minNode = findMin(node->right, dim, depth + 1);
                node->point = minNode->point;
                node->right = removeHelper(node->right, minNode->point, depth + 1);
            }
            else if (node->left != nullptr)
            {
                // 只有左子树：找当前维度上的前驱
                KDNode* minNode = findMin(node->left, dim, depth + 1);
                node->point = minNode->point;
                node->right = removeHelper(node->left, minNode->point, depth + 1);
                node->left = nullptr;
            }
            else
            {
                // 叶节点：直接删除
                delete node;
                size--;
                return nullptr;
            }
        }
        else if (point[dim] <= node->point[dim])
        {
            node->left = removeHelper(node->left, point, depth + 1);
        }
        else
        {
            node->right = removeHelper(node->right, point, depth + 1);
        }
        return node;
    }

    // ----------------------------------------------------------
    // 中序遍历（按维度顺序）
    // ----------------------------------------------------------
    void inorderHelper(KDNode* node) const
    {
        if (node == nullptr) return;

        inorderHelper(node->left);
        cout << node->point.str() << " (splitDim=" << node->splitDim << ")  ";
        inorderHelper(node->right);
    }

    // ----------------------------------------------------------
    // BFS 层序遍历
    // ----------------------------------------------------------
    void BFSHelper() const
    {
        if (root == nullptr) return;

        queue<KDNode*> q;
        q.push(root);
        int level = 0;

        while (!q.empty())
        {
            int sz = q.size();
            cout << "第 " << level << " 层：";
            for (int i = 0; i < sz; i++)
            {
                KDNode* node = q.front(); q.pop();
                cout << node->point.str()
                     << "[dim=" << node->splitDim << "]  ";
                if (node->left)  q.push(node->left);
                if (node->right) q.push(node->right);
            }
            cout << endl;
            level++;
        }
    }

    // ----------------------------------------------------------
    // 递归销毁
    // ----------------------------------------------------------
    void destroy(KDNode* node)
    {
        if (node == nullptr) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    // ----------------------------------------------------------
    // 打印 2D 树结构（仅在 k==2 时有意义）
    // ----------------------------------------------------------
    void print2DHelper(KDNode* node, const string& prefix, bool isLeft) const
    {
        if (node == nullptr) return;

        cout << prefix;
        cout << (isLeft ? "├── " : "└── ");
        cout << node->point.str();
        cout << " [分" << (node->splitDim == 0 ? "x" : "y") << "]";
        cout << endl;

        string newPrefix = prefix + (isLeft ? "│   " : "    ");
        print2DHelper(node->left, newPrefix, true);
        print2DHelper(node->right, newPrefix, false);
    }

    // ----------------------------------------------------------
    // ASCII 二维空间可视化
    // 仅支持 2D，在 20×20 的网格上标记各点的分裂情况
    // ----------------------------------------------------------
    void visualize2DHelper(int xMin, int xMax, int yMin, int yMax) const
    {
        if (k != 2)
        {
            cout << "(可视化仅支持 2D KD 树)" << endl;
            return;
        }

        vector<vector<char>> grid(21, vector<char>(41, ' '));
        collectPoints(root, grid, xMin, xMax, yMin, yMax);
        // 画坐标轴
        int originX = 20 * (-xMin) / (xMax - xMin);
        int originY = 20 * (-yMin) / (yMax - yMin);
        if (originX >= 0 && originX < 41)
            for (int r = 0; r < 21; r++) if (grid[r][originX] == ' ') grid[r][originX] = '|';
        if (originY >= 0 && originY < 21)
            for (int c = 0; c < 41; c++) if (grid[originY][c] == ' ') grid[originY][c] = '-';

        cout << "  y↑" << endl;
        for (int r = 0; r < 21; r++)
        {
            cout << "   ";
            for (int c = 0; c < 41; c++)
            {
                cout << grid[r][c];
            }
            cout << endl;
        }
        cout << "   " << string(41, ' ') << "→ x" << endl;
    }

    void collectPoints(KDNode* node, vector<vector<char>>& grid,
                       int xMin, int xMax, int yMin, int yMax) const
    {
        if (node == nullptr) return;

        int col = (int)(40.0 * (node->point[0] - xMin) / (xMax - xMin));
        int row = (int)(20.0 * (yMax - node->point[1]) / (yMax - yMin));

        if (col >= 0 && col < 41 && row >= 0 && row < 21)
        {
            char marker = (node->splitDim == 0) ? 'x' : 'y';
            if (node == root) marker = 'R';
            grid[row][col] = marker;
        }

        collectPoints(node->left, grid, xMin, xMax, yMin, yMax);
        collectPoints(node->right, grid, xMin, xMax, yMin, yMax);
    }

public:
    // ==================== 构造函数 ====================

    KDTree() : root(nullptr), k(0), size(0) {}

    // 传入点集构建 KD 树
    KDTree(const vector<Point>& points)
        : root(nullptr), size(0)
    {
        if (points.empty()) return;

        k = points[0].dims();
        vector<Point> pts = points;  // 拷贝，因为 build 会改变顺序
        root = build(pts, 0);
        size = (int)points.size();
    }

    ~KDTree()
    {
        destroy(root);
    }

    // ==================== 公有接口 ====================

    // 插入
    void insert(const Point& point)
    {
        if (root == nullptr)
        {
            k = point.dims();
            root = new KDNode(point, 0);
            size = 1;
        }
        else
        {
            root = insertHelper(root, point, 0);
        }
    }

    // 删除
    void remove(const Point& point)
    {
        root = removeHelper(root, point, 0);
    }

    // 最近邻
    Point nearestNeighbor(const Point& query) const
    {
        if (root == nullptr)
        {
            throw runtime_error("树为空！");
        }
        Point best;
        double bestDist = numeric_limits<double>::max();
        nearestNeighbor(root, query, best, bestDist);
        return best;
    }

    // K 近邻
    vector<pair<Point, double>> kNearestNeighbors(const Point& query, int K) const
    {
        if (root == nullptr || K <= 0) return {};

        // 最大堆：top 是距离最远的
        priority_queue<pair<double, Point>,
                       vector<pair<double, Point>>, DistCmp> maxHeap;
        kNNSearch(root, query, K, maxHeap);

        // 转为有序结果（按距离升序）
        vector<pair<Point, double>> result;
        while (!maxHeap.empty())
        {
            auto& top = maxHeap.top();
            result.push_back({top.second, sqrt(top.first)});
            maxHeap.pop();
        }
        reverse(result.begin(), result.end());
        return result;
    }

    // 范围搜索
    vector<Point> rangeSearch(const vector<double>& low,
                              const vector<double>& high) const
    {
        vector<Point> result;
        rangeSearch(root, low, high, result);
        return result;
    }

    // 半径搜索
    vector<pair<Point, double>> radiusSearch(const Point& center, double radius) const
    {
        vector<pair<Point, double>> result;
        radiusSearch(root, center, radius, result);
        sort(result.begin(), result.end(),
             [](auto& a, auto& b) { return a.second < b.second; });
        return result;
    }

    // 精确搜索点
    bool searchExact(const Point& query) const
    {
        KDNode* node = root;
        while (node != nullptr)
        {
            if (pointsEqual(node->point, query)) return true;

            int dim = node->splitDim;
            if (query[dim] <= node->point[dim])
            {
                node = node->left;
            }
            else
            {
                node = node->right;
            }
        }
        return false;
    }

    // 打印
    void print() const
    {
        cout << "\n========== KD 树结构（k=" << k << ", 点数=" << size << "）==========" << endl;
        if (root == nullptr)
        {
            cout << "(空树)" << endl;
        }
        else
        {
            print2DHelper(root, "", false);
        }
        cout << "========================================" << endl;
    }

    void inorder() const
    {
        cout << "中序遍历（按构建顺序）：" << endl;
        inorderHelper(root);
        cout << endl;
    }

    void BFS() const
    {
        cout << "\n========== BFS 层序遍历 ==========" << endl;
        BFSHelper();
        cout << "==================================" << endl;
    }

    void visualize() const
    {
        if (k != 2) return;

        // 先收集范围
        double xMin = 0, xMax = 10, yMin = 0, yMax = 10;
        getBoundingBox(root, xMin, xMax, yMin, yMax);
        double margin = 1;
        xMin -= margin; xMax += margin;
        yMin -= margin; yMax += margin;

        visualize2DHelper((int)xMin, (int)xMax, (int)yMin, (int)yMax);
    }

    void getBoundingBox(KDNode* node, double& xMin, double& xMax,
                        double& yMin, double& yMax) const
    {
        if (node == nullptr) return;
        xMin = min(xMin, node->point[0]);
        xMax = max(xMax, node->point[0]);
        yMin = min(yMin, node->point[1]);
        yMax = max(yMax, node->point[1]);
        getBoundingBox(node->left, xMin, xMax, yMin, yMax);
        getBoundingBox(node->right, xMin, xMax, yMin, yMax);
    }

    int getSize() const { return size; }
    int getDim() const { return k; }
};

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║   KD 树（K-Dimensional Tree）测试 ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    // ==================== 测试 1：构建 2D KD 树 ====================
    cout << "\n>>> 测试 1：构建 2D KD 树" << endl;
    vector<Point> points = {
        {{3, 6}}, {{2, 7}}, {{17, 15}}, {{6, 12}}, {{9, 1}},
        {{13, 15}}, {{10, 19}}, {{7, 5}}, {{15, 3}}, {{1, 9}}
    };
    cout << "输入点集：";
    for (auto& p : points) cout << p.str() << " ";
    cout << endl;

    KDTree tree(points);
    tree.print();
    tree.BFS();
    tree.inorder();

    // ==================== 测试 2：最近邻搜索 ====================
    cout << "\n>>> 测试 2：最近邻搜索" << endl;
    Point query1{{10, 10}};
    Point nn = tree.nearestNeighbor(query1);
    cout << "查询点 " << query1.str() << " 的最近邻: " << nn.str() << endl;
    cout << "  距离 = " << query1.distance(nn) << endl;

    Point query2{{16, 2}};
    Point nn2 = tree.nearestNeighbor(query2);
    cout << "查询点 " << query2.str() << " 的最近邻: " << nn2.str() << endl;
    cout << "  距离 = " << query2.distance(nn2) << endl;

    Point query3{{5, 5}};
    Point nn3 = tree.nearestNeighbor(query3);
    cout << "查询点 " << query3.str() << " 的最近邻: " << nn3.str() << endl;
    cout << "  距离 = " << query3.distance(nn3) << endl;

    // ==================== 测试 3：K 近邻搜索 ====================
    cout << "\n>>> 测试 3：K 近邻搜索（K=3）" << endl;
    Point queryK{{10, 10}};
    auto knn = tree.kNearestNeighbors(queryK, 3);
    cout << "查询点 " << queryK.str() << " 的 " << 3 << " 个最近邻：" << endl;
    for (int i = 0; i < (int)knn.size(); i++)
    {
        cout << "  " << (i + 1) << ". " << knn[i].first.str()
             << "  距离=" << knn[i].second << endl;
    }

    // ==================== 测试 4：范围搜索 ====================
    cout << "\n>>> 测试 4：范围搜索" << endl;
    vector<double> low = {5, 5};
    vector<double> high = {15, 15};
    cout << "查询范围：x∈[" << low[0] << ", " << high[0]
         << "], y∈[" << low[1] << ", " << high[1] << "]" << endl;
    auto rangeResult = tree.rangeSearch(low, high);
    cout << "范围内的点（共 " << rangeResult.size() << " 个）：" << endl;
    for (auto& p : rangeResult)
    {
        cout << "  " << p.str() << endl;
    }

    // ==================== 测试 5：半径搜索 ====================
    cout << "\n>>> 测试 5：半径搜索" << endl;
    Point center{{10, 10}};
    double radius = 5.0;
    cout << "查询中心 " << center.str() << "，半径 " << radius << endl;
    auto radResult = tree.radiusSearch(center, radius);
    cout << "半径内的点（共 " << radResult.size() << " 个）：" << endl;
    for (auto& p : radResult)
    {
        cout << "  " << p.first.str() << "  距离=" << p.second << endl;
    }

    // ==================== 测试 6：精确搜索 ====================
    cout << "\n>>> 测试 6：精确搜索" << endl;
    Point existPt{{6, 12}};
    Point nonExistPt{{100, 100}};
    cout << existPt.str() << " 存在？" << (tree.searchExact(existPt) ? "是" : "否") << endl;
    cout << nonExistPt.str() << " 存在？" << (tree.searchExact(nonExistPt) ? "是" : "否") << endl;

    // ==================== 测试 7：插入与删除 ====================
    cout << "\n>>> 测试 7：插入新点" << endl;
    tree.insert({{8, 8}, 100});
    tree.insert({{12, 10}, 101});
    tree.insert({{4, 4}, 102});
    cout << "插入 (8,8), (12,10), (4,4) 后：" << endl;
    tree.print();
    cout << "树大小 = " << tree.getSize() << endl;

    cout << "\n>>> 测试 8：删除点" << endl;
    cout << "删除 (6,12)..." << endl;
    tree.remove({{6, 12}});
    tree.print();
    cout << "树大小 = " << tree.getSize() << endl;

    // ==================== 测试 9：高维 KD 树 ====================
    cout << "\n>>> 测试 9：3D KD 树" << endl;
    vector<Point> pts3D = {
        {{2, 3, 7}}, {{5, 4, 2}}, {{9, 6, 5}}, {{4, 7, 9}}, {{8, 1, 5}},
        {{7, 2, 6}}, {{3, 8, 1}}, {{6, 5, 3}}, {{1, 3, 8}}, {{10, 9, 4}}
    };
    KDTree tree3D(pts3D);
    tree3D.print();
    tree3D.BFS();

    Point query3D{{7, 5, 4}};
    Point nn3D = tree3D.nearestNeighbor(query3D);
    cout << "\n查询点 " << query3D.str() << " 的最近邻：" << nn3D.str() << endl;
    cout << "  距离 = " << query3D.distance(nn3D) << endl;

    auto knn3D = tree3D.kNearestNeighbors(query3D, 3);
    cout << "\nK=3 近邻：" << endl;
    for (int i = 0; i < (int)knn3D.size(); i++)
    {
        cout << "  " << (i + 1) << ". " << knn3D[i].first.str()
             << "  距离=" << knn3D[i].second << endl;
    }

    // ==================== 测试 10：验证剪枝效率 ====================
    cout << "\n>>> 测试 10：大规模数据 KNN 效率演示" << endl;
    vector<Point> bigPoints;
    for (int i = 0; i < 200; i++)
    {
        double x = rand() % 1000 / 10.0;
        double y = rand() % 1000 / 10.0;
        bigPoints.push_back({{x, y}, i});
    }
    KDTree bigTree(bigPoints);
    cout << "构建了 " << bigTree.getSize() << " 个点的 2D KD 树" << endl;
    Point bigQuery{{50, 50}};
    auto bigKnn = bigTree.kNearestNeighbors(bigQuery, 5);
    cout << "查询点 " << bigQuery.str() << " 的 5 个最近邻：" << endl;
    for (int i = 0; i < (int)bigKnn.size(); i++)
    {
        cout << "  " << (i + 1) << ". " << bigKnn[i].first.str()
             << "  距离=" << bigKnn[i].second << endl;
    }

    cout << "\nKD 树所有测试完成！" << endl;
    return 0;
}
