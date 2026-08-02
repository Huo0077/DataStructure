# C++17 数据结构与算法

> 一个面向原理理解、代码实现与工程复盘的 C++17 数据结构与算法学习项目。
>
> 18 个可独立编译运行的模块，覆盖线性结构、树、图、哈希、并查集、排序与高级字符串索引；每个模块都包含核心不变量、实现注释与自验证 Demo。

<p align="center">
  <a href="https://datastructure-1.gitbook.io/datastructure-docs"><strong>在线教材（GitBook）</strong></a>
  ·
  <a href="docs/README.md"><strong>GitHub 分章阅读版</strong></a>
  ·
  <a href="数据结构教学文档.md">单页 Markdown</a>
  ·
  <a href="数据结构教学文档.pdf">PDF 打印版</a>
</p>

---

## 从这里开始

| 入口 | 适合场景 |
| --- | --- |
| [GitBook 在线教材](https://datastructure-1.gitbook.io/datastructure-docs) | 推荐首次学习：章节目录、在线阅读与连续导航 |
| [GitHub 分章文档](docs/README.md) | 在仓库内按章节阅读，并直接跳转到源码 |
| [完整教学文档（Markdown）](数据结构教学文档.md) | 全文搜索、离线保存或一次性阅读 |
| [完整教学文档（PDF）](数据结构教学文档.pdf) | 打印、平板阅读与出版物式浏览 |

每章均提供前置知识、理论说明、关键推导、代码入口、章节小结和思考题。GitBook 与 GitHub 分章版由仓库中的 `docs/` 目录维护。

## 项目特点

- **理论与代码对应**：每个主题都链接到可编译的 C++17 实现。
- **以不变量为核心**：不仅解释“怎么写”，也说明“为什么正确”。
- **覆盖完整学习路径**：从复杂度与 ADT，到平衡树、图算法、外部排序和后缀结构。
- **可直接验证**：CMake 为每个模块生成独立目标，并注册 CTest 测试。

## 推荐学习路线

| 阶段 | 学习内容 | 对应章节 |
| --- | --- | --- |
| 1. 基础 | 复杂度、ADT、链表、栈队列、KMP | [第 1-4 章](docs/01-complexity-adt.md) |
| 2. 树与排序 | 二叉树、BST、堆、排序 | [第 5-7 章](docs/05-binary-tree-basics.md) / [第 14 章](docs/14-sorting-algorithms.md) |
| 3. 高级树结构 | AVL、红黑树、伸展树、B/B+ 树、KD 树 | [第 8-10 章](docs/08-balanced-binary-search-trees.md) |
| 4. 综合算法 | 图、哈希、并查集、后缀数组与后缀树 | [第 11-15 章](docs/11-graph-algorithms.md) |

完整章节目录请从 [GitBook 在线教材](https://datastructure-1.gitbook.io/datastructure-docs) 或 [分章学习入口](docs/README.md) 开始。

## 模块地图

| 分类 | 源文件 | 核心数据结构 / 算法 | 关键技术与不变量 |
| :--- | :--- | :--- |
| 理论基础 | [`complexity_adt.cpp`](complexity_adt.cpp) | 渐进符号、ADT | 主定理、均摊分析 |
| 线性结构 | [`Linked list.cpp`](Linked%20list.cpp) | 单/双向链表、循环链表 | 快慢指针、链表反转、哨兵节点 |
| 受限结构 | [`linear_structures.cpp`](linear_structures.cpp) | 栈、队列、Deque、最小栈 | 边界条件、双栈模拟队列、单调栈 |
| 模式匹配 | [`string_match.cpp`](string_match.cpp) | 朴素匹配、KMP | 前缀函数、`next`、`nextval` |
| 二叉搜索树 | [`binary_tree.cpp`](binary_tree.cpp) | 二叉树、BST、顺序统计树 | 遍历有序性、三类删除、Rank |
| 堆 | [`heap.cpp`](heap.cpp) | 最小堆、最大堆、左式堆 | 上浮/下沉、NPL、Top-K |
| 平衡 BST | [`avl_tree.cpp`](avl_tree.cpp) | AVL 树 | 平衡因子、LL/RR/LR/RL 旋转 |
| 平衡 BST | [`redblacktree.cpp`](redblacktree.cpp) | 红黑树 | 红黑性质、黑高、插入/删除修复 |
| 自适应 BST | [`splaytree.cpp`](splaytree.cpp) | 伸展树 | Zig/Zag、均摊复杂度 |
| 多路搜索树 | [`multiwaytree.cpp`](multiwaytree.cpp) | B 树 | 分裂、借位、合并 |
| 磁盘索引 | [`bplustree.cpp`](bplustree.cpp) | B+ 树 | 路由键、叶节点链表、范围查询 |
| 空间索引 | [`kdtree.cpp`](kdtree.cpp) | KD 树 | 维度轮换、KNN、范围剪枝 |
| 图论算法 | [`graph.cpp`](graph.cpp) | BFS、DFS、最短路、MST、拓扑排序、AOE | 松弛、入度、最早/最迟事件 |
| 哈希表 | [`hashing.cpp`](hashing.cpp) | 拉链、开放定址、双重散列、FKS | 负载因子、冲突处理、完美哈希 |
| 并查集 | [`disjointset.cpp`](disjointset.cpp) | Disjoint Set Union | 路径压缩、按秩合并、α(n) |
| 排序 | [`sort.cpp`](sort.cpp) | 十类排序、TimSort | 稳定性、划分、外部排序 |
| 高级字符串 | [`suffixarray.cpp`](suffixarray.cpp) | 后缀数组、LCP | 倍增、Kasai 算法 |
| 高级字符串 | [`suffixtree.cpp`](suffixtree.cpp) | Ukkonen 后缀树 | Active Point、Suffix Link |

## 构建与测试

### CMake（推荐）

```bash
# 在项目根目录执行
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### 单文件编译示例

```powershell
g++ -std=c++17 -Wall -Wextra -Wpedantic sort.cpp -o sort.exe
.\sort.exe
```

## 常用复杂度速查

| 数据结构 | 查找 | 插入 | 删除 | 典型场景 |
| --- | :---: | :---: | :---: | --- |
| 动态数组 | O(1) 下标 | O(1) 均摊尾插 | O(n) | 随机访问、连续存储 |
| 链表 | O(n) | O(1)* | O(1)* | 已知位置的频繁增删 |
| 平衡 BST | O(log n) | O(log n) | O(log n) | 有序查找与遍历 |
| 哈希表 | O(1) 平均 | O(1) 平均 | O(1) 平均 | 无序键值查找 |
| 二叉堆 | O(1) 堆顶 | O(log n) | O(log n) 堆顶 | 优先队列、Top-K |
| 并查集 | O(α(n)) | O(α(n)) | - | 动态连通性 |
| B+ 树 | O(log_m n) | O(log_m n) | O(log_m n) | 数据库索引、范围查询 |

\* 链表的 O(1) 插入/删除以前驱节点或目标节点引用已知为前提。

## 文档结构

```text
.
├── docs/
│   ├── README.md       # GitHub 分章学习入口
│   ├── SUMMARY.md      # GitBook 左侧章节目录
│   ├── 01-...md ~ 15-...md
│   └── appendix-...md
├── .gitbook.yaml       # GitBook 内容根目录配置
├── 数据结构教学文档.md  # 完整单页版
└── 数据结构教学文档.pdf # PDF 打印版
```

## 参考书目

- Thomas H. Cormen et al., *Introduction to Algorithms*（CLRS）
- Robert Sedgewick and Kevin Wayne, *Algorithms*
- Mark Allen Weiss, *Data Structures and Algorithm Analysis*
