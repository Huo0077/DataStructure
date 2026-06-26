# 数据结构学习项目

16 个独立 C++ 源文件，覆盖大学数据结构课程全部知识点。每个文件包含详细中文注释、图解和可运行的测试用例，仅依赖 C++ 标准库。

## 快速开始

```bash
g++ -std=c++17 -Wall -Wextra -o output 文件名.cpp && ./output
```

编译器需支持 C++17（GCC 8+ / MSVC 2019+ / Clang 7+），无需安装任何第三方库。

## 文件总览

| # | 文件 | 涵盖内容 | 难度 |
|---|------|----------|:---:|
| 1 | [complexity_adt.cpp](complexity_adt.cpp) | 时间复杂度（Big-O/Ω/Θ）、主定理、ADT 形式化定义 | ⭐ |
| 2 | [Linked list.cpp](Linked%20list.cpp) | 双向链表（增删查改、升序插入、降序插入） | ⭐ |
| 3 | [linear_structures.cpp](linear_structures.cpp) | 顺序栈/链式栈、循环队列/链式队列、最小栈、双栈队列、双端队列、循环链表 | ⭐⭐ |
| 4 | [binary_tree.cpp](binary_tree.cpp) | BST 插入/删除（3 种情况）、中序遍历验证 BST、四种遍历、增强树查第 K 小 | ⭐⭐ |
| 5 | [heap.cpp](heap.cpp) | 最小堆（上浮/下沉）、Top-K 最大算法、左式堆（NPL + 可合并） | ⭐⭐ |
| 6 | [sort.cpp](sort.cpp) | 插入/冒泡×4/快排×5/归并/桶/希尔×2/基数/堆/计数/外部排序 | ⭐⭐⭐ |
| 7 | [avl_tree.cpp](avl_tree.cpp) | AVL 树四种旋转（LL/RR/LR/RL）、平衡因子、深拷贝（三法则） | ⭐⭐⭐ |
| 8 | [redblacktree.cpp](redblacktree.cpp) | 红黑树五条性质、插入修正 3 种情况、删除修正 4 种情况 | ⭐⭐⭐ |
| 9 | [splaytree.cpp](splaytree.cpp) | 伸展树 Zig/Zig-Zig/Zig-Zag、自适应特性、均摊分析 | ⭐⭐⭐ |
| 10 | [multiwaytree.cpp](multiwaytree.cpp) | B 树插入/删除（分裂/借位/合并）、2-3-4 树与红黑树等价关系 | ⭐⭐⭐ |
| 11 | [bplustree.cpp](bplustree.cpp) | B+ 树内部节点路由、叶节点链表、范围查询 | ⭐⭐⭐ |
| 12 | [hashing.cpp](hashing.cpp) | 分离链接、线性探测、二次探测、双重散列、完美哈希（FKS 两级结构） | ⭐⭐ |
| 13 | [suffixarray.cpp](suffixarray.cpp) | 后缀数组（倍增算法 O(n log² n)）、LCP（Kasai O(n)）、二分查找模式匹配、LRS、LCS | ⭐⭐⭐ |
| 14 | [suffixtree.cpp](suffixtree.cpp) | 后缀 Trie、后缀树（Ukkonen 算法、active point、suffix link、在线构造） | ⭐⭐⭐ |
| 15 | [kdtree.cpp](kdtree.cpp) | KD 树（交替维度划分）、最近邻搜索、KNN、范围搜索 | ⭐⭐ |
| 16 | [disjointset.cpp](disjointset.cpp) | 并查集（路径压缩 + 按大小合并、均摊 O(α(n))） | ⭐ |

## 建议学习路径

### 阶段一：基础（文件 1-3）

> 复杂度理论 → 链表 → 栈与队列

**目标**：建立算法分析的思维框架，掌握最基础的两种线性结构。

- `complexity_adt` — 先理解 Big-O 的真正含义（不是"运行时间"而是"增长趋势"），主定理是分析递归复杂度的利器，ADT 教会你"接口与实现分离"
- `Linked list` — 链表是"指针思维"的入门课，插入/删除只需改指针不需要搬数据，这是它与数组最本质的区别
- `linear_structures` — 栈和队列是受限的线性表，"受限"反而让它们用途更广（函数调用栈、BFS 队列、表达式求值）。注意变体设计：最小栈用辅助栈、双栈队列用均摊分析

**核心能力**：能分析简单算法复杂度，能手写链表和栈/队列的插入删除。

### 阶段二：树与排序（文件 4-6）

> BST → 堆 → 排序算法

**目标**：从线性结构过渡到非线性（树），同时掌握各类排序的适用场景。

- `binary_tree` — BST 是后续所有平衡树的前置知识。**删除的三种情况必须滚瓜烂熟**（无子/一子/两子→后继替换）。四种遍历各有用途：前序复制树、中序得升序、后序释放内存、层序按层处理
- `heap` — 堆是完全二叉树用数组存储的典范。上浮/下沉各 4 行代码，但 Top-K 问题能省掉全排序的 O(n log n)。左式堆引入 NPL 概念，解决普通堆不能高效合并的痛点
- `sort` — 文件最大，建议按路线阅读：插入排序（理解"局部有序"）→ 冒泡四变体（理解优化思路）→ 快排演进（基础 → 三数取中 → 截断 → 三路 → 终极版）→ 归并（理解分治）→ 堆排/基数/计数。外部排序是面试加分项

**核心能力**：能手写 BST 插入删除、堆的 push/pop、快排和归并排序。

### 阶段三：平衡树（文件 7-11）

> AVL → 红黑树 → 伸展树 → B 树 → B+ 树

**目标**：理解"平衡"的不同实现策略，以及它们之间的设计权衡。

- `avl_tree` — 最严格的平衡（|bf| ≤ 1），四种旋转背口诀：LL 右旋、RR 左旋、LR 左右旋、RL 右左旋。关键是旋转后高度更新顺序（先下后上）
- `redblacktree` — 放松平衡换取更少旋转。先背熟五条性质（根黑、叶黑、红子必黑、同黑高），然后用性质推导修正逻辑，不要死记
- `splaytree` — 不存平衡信息，靠每次访问后的伸展操作自适应。关键区分 Zig-Zig（先父后子）和 Zig-Zag（先子后父），这个顺序差异是均摊 O(log n) 的保证
- `multiwaytree` — 理解"一个节点存多个 key"为什么适合磁盘（减少 IO 次数）。分裂和借位操作是对 2-3-4 树的泛化
- `bplustree` — 内部节点只做路由，数据全在叶子。叶子用链表串联 → 范围查询 O(log n + k)。MySQL InnoDB 的默认索引结构

**核心能力**：能手写 AVL 插入/删除、红黑树插入修正、B 树插入/删除。

### 阶段四：字符串与高级专题（文件 12-16）

> 哈希 → 后缀数组 → 后缀树 → KD 树 → 并查集

**目标**：掌握散列思想、字符串算法和空间数据结构。

- `hashing` — 理解装载因子与性能的关系。分离链接最直观，线性探测有聚集问题，二次探测和双重散列解决它。完美哈希（FKS）实现 O(1) 最坏查找——两级结构是关键
- `suffixarray` — 倍增排序 O(n log² n) 是理解后缀数组的入门算法。LCP 的 Kasai 算法用"h 每次减 1"的性质把 O(n²) 降到 O(n)，这个技巧值得细品。模式匹配用二分、LRS 找相邻最大 LCP、LCS 靠拼接+哨兵
- `suffixtree` — Ukkonen 算法是在线构造后缀树的 O(n) 算法。核心三要素：active point（在哪插入）、suffix link（快速跳转）、剩余后缀数（判断要不要显式插入）
- `kdtree` — 多维空间中的 BST 推广。交替维度划分 + 剪枝搜索。范围搜索时判断当前区域是否与查询矩形相交——不相交直接返回，这是剪枝的精髓
- `disjointset` — 代码最短但思想深远。路径压缩 + 按大小合并使均摊复杂度接近 O(1)（反 Ackermann）。图的连通性、Kruskal 最小生成树的核心组件

## 复杂度速查表

| 数据结构 | 插入 | 删除 | 查找 | 空间 |
|----------|:---:|:---:|:---:|:---:|
| 数组（无序） | O(1) | O(n) | O(n) | O(n) |
| 有序数组 | O(n) | O(n) | O(log n) | O(n) |
| 双向链表 | O(1) | O(1) | O(n) | O(n) |
| BST（退化为链） | O(n) | O(n) | O(n) | O(n) |
| AVL 树 | O(log n) | O(log n) | O(log n) | O(n) |
| 红黑树 | O(log n) | O(log n) | O(log n) | O(n) |
| 伸展树（均摊） | O(log n) | O(log n) | O(log n) | O(n) |
| B 树 | O(log n) | O(log n) | O(log n) | O(n) |
| 最小堆 | O(log n) | O(log n) | O(1) top | O(n) |
| 哈希表（平均） | O(1) | O(1) | O(1) | O(n) |
| 完美哈希（最坏） | O(1) | — | O(1) | O(n) |
| 并查集 | O(α(n)) | — | O(α(n)) | O(n) |

| 排序算法 | 最好 | 平均 | 最坏 | 空间 | 稳定 |
|----------|:---:|:---:|:---:|:---:|:---:|
| 插入排序 | O(n) | O(n²) | O(n²) | O(1) | 是 |
| 冒泡排序 | O(n) | O(n²) | O(n²) | O(1) | 是 |
| 快速排序 | O(n log n) | O(n log n) | O(n²) | O(log n) | 否 |
| 归并排序 | O(n log n) | O(n log n) | O(n log n) | O(n) | 是 |
| 堆排序 | O(n log n) | O(n log n) | O(n log n) | O(1) | 否 |
| 基数排序 | — | O(nk) | O(nk) | O(n+k) | 是 |
| 计数排序 | — | O(n+k) | O(n+k) | O(k) | 是 |

## 面试高频考点

- **反转链表** / 链表环检测 → [Linked list.cpp](Linked%20list.cpp)
- **用两个栈实现队列** / 最小栈 → [linear_structures.cpp](linear_structures.cpp)
- **BST 验证** / 第 K 小元素 → [binary_tree.cpp](binary_tree.cpp)
- **Top-K 问题** → [heap.cpp](heap.cpp)（用最小堆做门槛，O(n log k)）
- **快排手写** / 归并排序 → [sort.cpp](sort.cpp)
- **哈希表实现** / 冲突解决 → [hashing.cpp](hashing.cpp)
- **AVL 与红黑树对比** → [avl_tree.cpp](avl_tree.cpp) + [redblacktree.cpp](redblacktree.cpp)
- **B+ 树范围查询** → [bplustree.cpp](bplustree.cpp)
- **并查集** / 连通性判断 → [disjointset.cpp](disjointset.cpp)

## 常见问题

**Q: 为什么不拆成 .h + .cpp？**
A: 每个文件都是一个独立的可编译单元，方便快速测试和理解。所有实现在一个文件内，免去跨文件跳转的认知负担。

**Q: AVL 和红黑树该先学哪个？**
A: 先 AVL。AVL 的平衡条件直观（高度差 ≤ 1），旋转规则容易记忆。红黑树的颜色规则是人为约定的，理解"为什么这样能保证平衡"需要先用 AVL 建立直觉。

**Q: 后缀数组和后缀树怎么选？**
A: 后缀数组实现简单、空间小（一个 int 数组），实际竞赛和工程中用得更多。后缀树的理论更优美，Ukkonen 算法是算法设计的艺术品。建议先数组后树，能体会"等价结构的不同实现"。

**Q: 这些代码能用在生产环境吗？**
A: 不建议直接用于生产。本项目的目的是教学——代码优先可读性和注释完整性，未做异常安全、迭代器、移动语义等工程优化。生产环境请用 STL（`std::map`、`std::unordered_map`、`std::priority_queue` 等）。

## 参考资料

- 《数据结构与算法分析——C 语言描述》（Mark Allen Weiss）
- 《算法导论》第三版（CLRS）
- 《算法》第四版（Robert Sedgewick）
- [Visualgo](https://visualgo.net/) — 算法可视化，配合代码阅读效果极佳
