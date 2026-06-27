# 数据结构学习项目

18 个独立 C++ 源文件，覆盖大学数据结构课程全部知识点。每个文件包含详细中文注释、图解和可运行的测试用例，仅依赖 C++ 标准库。

## 快速开始

```bash
g++ -std=c++17 -Wall -Wextra -o output 文件名.cpp && ./output
```

编译器需支持 C++17（GCC 8+ / MSVC 2019+ / Clang 7+），无需安装任何第三方库。

## 文件总览

| # | 文件 | 涵盖内容 | 难度 |
|---|------|----------|:---:|
| 1 | [complexity_adt.cpp](complexity_adt.cpp) | 时间复杂度（Big-O/Ω/Θ）、主定理、ADT 形式化定义 | ⭐ |
| 2 | [Linked list.cpp](Linked%20list.cpp) | 单向链表（头插/尾插/反转）、双向链表（增删查改、升序/降序插入） | ⭐ |
| 3 | [linear_structures.cpp](linear_structures.cpp) | 顺序栈/链式栈、循环队列/链式队列、最小栈、双栈队列、双端队列、循环链表 | ⭐⭐ |
| 4 | [binary_tree.cpp](binary_tree.cpp) | BST 插入/删除（3 种情况）、中序遍历验证 BST、四种遍历、增强树查第 K 小 | ⭐⭐ |
| 5 | [heap.cpp](heap.cpp) | 最小堆（上浮/下沉）、Top-K 最大算法、左式堆（NPL + 可合并） | ⭐⭐ |
| 6 | [sort.cpp](sort.cpp) | 插入/冒泡×4/快排×5/归并/桶/希尔×2/基数/堆/计数/外部排序 + 逆序对计数 | ⭐⭐⭐ |
| 7 | [avl_tree.cpp](avl_tree.cpp) | AVL 树四种旋转（LL/RR/LR/RL）、平衡因子、深拷贝（三法则） | ⭐⭐⭐ |
| 8 | [redblacktree.cpp](redblacktree.cpp) | 红黑树五条性质、插入修正 3 种情况、删除修正 4 种情况 | ⭐⭐⭐ |
| 9 | [splaytree.cpp](splaytree.cpp) | 伸展树 Zig/Zig-Zig/Zig-Zag、自适应特性、均摊分析 | ⭐⭐⭐ |
| 10 | [multiwaytree.cpp](multiwaytree.cpp) | B 树插入/删除（分裂/借位/合并）、2-3 树 + 2-3-4 树与红黑树等价关系 | ⭐⭐⭐ |
| 11 | [bplustree.cpp](bplustree.cpp) | B+ 树内部节点路由、叶节点链表、范围查询 | ⭐⭐⭐ |
| 12 | [hashing.cpp](hashing.cpp) | 分离链接、线性探测、二次探测、双重散列、完美哈希（FKS 两级结构） | ⭐⭐ |
| 13 | [suffixarray.cpp](suffixarray.cpp) | 后缀数组（倍增算法 O(n log² n)）、LCP（Kasai O(n)）、二分查找模式匹配、LRS、LCS | ⭐⭐⭐ |
| 14 | [suffixtree.cpp](suffixtree.cpp) | 后缀 Trie、后缀树（Ukkonen 算法、active point、suffix link、在线构造） | ⭐⭐⭐ |
| 15 | [kdtree.cpp](kdtree.cpp) | KD 树（交替维度划分）、最近邻搜索、KNN、范围搜索 | ⭐⭐ |
| 16 | [disjointset.cpp](disjointset.cpp) | 并查集（路径压缩 + 按大小合并、均摊 O(α(n))） | ⭐ |
| 17 | [string_match.cpp](string_match.cpp) | 朴素匹配、KMP 算法（next 数组 + nextval 优化）、O(n+m) | ⭐⭐ |
| 18 | [graph.cpp](graph.cpp) | 邻接矩阵、DFS/BFS、拓扑排序（AOV）、关键路径（AOE）、MST（Prim/Kruskal）、最短路径（Dijkstra/Floyd） | ⭐⭐⭐ |

## 建议学习路径

### 阶段一：基础（文件 1-3）

> 复杂度理论 → 链表 → 栈与队列

- `complexity_adt` — 先理解 Big-O 的真正含义（不是"运行时间"而是"增长趋势"），主定理是分析递归复杂度的利器，ADT 教会你"接口与实现分离"
- `Linked list` — 链表是"指针思维"的入门课，插入/删除只需改指针不需要搬数据。从单向链表理解基本概念，再用双向链表体会 prev 指针带来的便利
- `linear_structures` — 栈和队列是受限的线性表，"受限"反而让它们用途更广（函数调用栈、BFS 队列、表达式求值）。注意变体设计：最小栈用辅助栈、双栈队列用均摊分析

### 阶段二：树与排序（文件 4-6）

> BST → 堆 → 排序算法

- `binary_tree` — BST 是所有平衡树的前置。**删除的三种情况必须滚瓜烂熟**。四种遍历各有用途：前序复制树、中序得升序、后序释放内存、层序按层处理。增强树 leftSize 技巧是"有序集合"的入门
- `heap` — 堆是完全二叉树用数组存储的典范。上浮/下沉各 4 行代码，Top-K 用最小堆做门槛 O(n log k)。左式堆引入 NPL 概念，解决普通堆不能高效合并的痛点
- `sort` — 建议按路线：插入排序 → 冒泡四变体 → 快排演进（基础→三数取中→截断→三路→终极版）→ 归并 → 堆/基数/计数。逆序对是归并计数的经典应用，外部排序是加分项

### 阶段三：平衡树（文件 7-11）

> AVL → 红黑树 → 伸展树 → B 树 → B+ 树

- `avl_tree` — 最严格的平衡（|bf| ≤ 1），四种旋转背口诀。关键是旋转后高度更新顺序（先下后上）
- `redblacktree` — 放松平衡换取更少旋转。先背熟五条性质，用性质推导修正逻辑，不要死记
- `splaytree` — 不存平衡信息，靠每次访问后的伸展自适应。区分 Zig-Zig（先父后子）和 Zig-Zag（先子后父），这个顺序差异是均摊 O(log n) 的保证
- `multiwaytree` — "一个节点存多个 key"为什么适合磁盘。分裂和借位操作。包含 2-3 树（3路树）专项测试
- `bplustree` — 内部节点只做路由，数据全在叶子。叶子用链表串联 → 范围查询。MySQL InnoDB 的默认索引

### 阶段四：字符串与高级专题（文件 12-16, 17-18）

> 哈希 → 字符串匹配 → 后缀结构 → KD 树 → 并查集 → 图

- `hashing` — 理解装载因子与性能的关系。分离链接→线性探测→二次/双重散列→完美哈希（FKS），逐级递进
- `string_match` — KMP 的 next 数组计算和匹配过程是算法设计的经典——利用已匹配信息避免回退。nextval 优化体现出"更进一步"的工程思维
- `suffixarray` — 倍增排序 + LCP 的 Kasai 算法（h 每次减 1 降 O(n²)→O(n)）。用二分做模式匹配、LRS 找相邻最大 LCP
- `suffixtree` — Ukkonen 在线构造 O(n)。core: active point + suffix link + 剩余后缀数
- `kdtree` — 多维 BST 推广。交替维度划分 + 剪枝（不相交直接返回）
- `disjointset` — 代码最短但思想深远。路径压缩+按大小合并 → 均摊 O(α(n))
- `graph` — 图论全集：DFS/BFS → 拓扑排序 → 关键路径 → MST（Prim+Kruskal）→ 最短路径（Dijkstra+Floyd）。并查集在 Kruskal 中得到实战应用

## 复杂度速查表

### 数据结构

| 数据结构 | 插入 | 删除 | 查找 | 空间 |
|----------|:---:|:---:|:---:|:---:|
| 数组（无序） | O(1) | O(n) | O(n) | O(n) |
| 有序数组 | O(n) | O(n) | O(log n) | O(n) |
| 单向/双向链表 | O(1) | O(1) | O(n) | O(n) |
| BST（平均） | O(log n) | O(log n) | O(log n) | O(n) |
| AVL 树 | O(log n) | O(log n) | O(log n) | O(n) |
| 红黑树 | O(log n) | O(log n) | O(log n) | O(n) |
| 伸展树（均摊） | O(log n) | O(log n) | O(log n) | O(n) |
| B 树 | O(log n) | O(log n) | O(log n) | O(n) |
| 最小堆 | O(log n) | O(log n) | O(1) top | O(n) |
| 哈希表（平均） | O(1) | O(1) | O(1) | O(n) |
| 完美哈希（最坏） | O(1) | — | O(1) | O(n) |
| 并查集 | O(α(n)) | — | O(α(n)) | O(n) |

### 排序算法

| 排序算法 | 最好 | 平均 | 最坏 | 空间 | 稳定 |
|----------|:---:|:---:|:---:|:---:|:---:|
| 插入排序 | O(n) | O(n²) | O(n²) | O(1) | 是 |
| 冒泡排序 | O(n) | O(n²) | O(n²) | O(1) | 是 |
| 快速排序 | O(n log n) | O(n log n) | O(n²) | O(log n) | 否 |
| 归并排序 | O(n log n) | O(n log n) | O(n log n) | O(n) | 是 |
| 堆排序 | O(n log n) | O(n log n) | O(n log n) | O(1) | 否 |
| 基数排序 | — | O(nk) | O(nk) | O(n+k) | 是 |
| 计数排序 | — | O(n+k) | O(n+k) | O(k) | 是 |

### 图算法

| 算法 | 时间复杂度 | 用途 |
|------|:---:|------|
| DFS / BFS | O(V+E) | 遍历、连通性 |
| 拓扑排序 (Kahn) | O(V+E) | DAG 任务调度 |
| 关键路径 (AOE) | O(V+E) | 项目管理、工期优化 |
| Prim | O(V²) / O((V+E)log V) | 最小生成树（稠密图） |
| Kruskal | O(E log E) | 最小生成树（稀疏图） |
| Dijkstra | O(V²) / O((V+E)log V) | 单源最短路径（非负权） |
| Floyd-Warshall | O(V³) | 所有点对最短路径 |
| KMP | O(n+m) | 字符串模式匹配 |

## 面试高频考点

- **反转链表** / 链表环检测 → [Linked list.cpp](Linked%20list.cpp)
- **用两个栈实现队列** / 最小栈 → [linear_structures.cpp](linear_structures.cpp)
- **BST 验证** / 第 K 小元素 → [binary_tree.cpp](binary_tree.cpp)
- **Top-K 问题** → [heap.cpp](heap.cpp)
- **快排手写** / 归并排序 / 逆序对计数 → [sort.cpp](sort.cpp)
- **KMP 算法** / next 数组 → [string_match.cpp](string_match.cpp)
- **哈希表实现** / 冲突解决 → [hashing.cpp](hashing.cpp)
- **AVL 与红黑树对比** → [avl_tree.cpp](avl_tree.cpp) + [redblacktree.cpp](redblacktree.cpp)
- **B+ 树范围查询** → [bplustree.cpp](bplustree.cpp)
- **图遍历 (DFS/BFS)** / 拓扑排序 → [graph.cpp](graph.cpp)
- **最短路径 (Dijkstra)** / 最小生成树 → [graph.cpp](graph.cpp)
- **并查集** / 连通性判断 → [disjointset.cpp](disjointset.cpp)

## 常见问题

**Q: 为什么不拆成 .h + .cpp？**
A: 每个文件都是独立可编译单元，方便快速测试和理解。所有实现在一个文件内，免去跨文件跳转的认知负担。

**Q: AVL 和红黑树该先学哪个？**
A: 先 AVL。AVL 的平衡条件直观（高度差 ≤ 1），旋转规则容易记忆。红黑树的颜色规则是人为约定的，理解"为什么这样能保证平衡"需要先用 AVL 建立直觉。

**Q: 后缀数组和后缀树怎么选？**
A: 后缀数组实现简单、空间小（一个 int 数组），实际竞赛和工程中用得更多。后缀树的理论更优美，Ukkonen 算法是算法设计的艺术品。建议先数组后树。

**Q: 这些代码能用在生产环境吗？**
A: 不建议直接用于生产。本项目的目的是教学——代码优先可读性和注释完整性，未做异常安全、迭代器、移动语义等工程优化。生产环境请用 STL。

## 参考资料

- 《数据结构与算法分析——C 语言描述》（Mark Allen Weiss）
- 《算法导论》第三版（CLRS）
- 《算法》第四版（Robert Sedgewick）
- [Visualgo](https://visualgo.net/) — 算法可视化，配合代码阅读效果极佳
