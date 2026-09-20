# C++17 数据结构与算法：原理精要、工程实现与教学指南

> 一部面向原理透彻理解、算法不变量推导与工业级实现的现代 C++17 数据结构与算法教程。
>
> 涵盖 18 个独立可编译运行的模块，从基础线性结构、平衡搜索树、多维与多路索引，到图论算法、十类排序及高级后缀结构；每个模块均包含严密推导、算法不变量（Invariant）、生产级异常与边界防御，以及自验证测试用例。

<p align="center">
  <a href="https://datastructure-1.gitbook.io/datastructure-docs"><strong>📖 在线教材（GitBook）</strong></a>
  ·
  <a href="docs/README.md"><strong>📚 GitHub 分章阅读版</strong></a>
  ·
  <a href="数据结构教学文档.md"><strong>📄 单页完整教材（Markdown）</strong></a>
  ·
  <a href="数据结构教学文档.pdf"><strong>📑 出版级 PDF 打印版</strong></a>
</p>

---

## 目录与快速导航

- [从这里开始](#从这里开始)
- [设计哲学与教学特色](#设计哲学与教学特色)
- [数据结构与算法全景图](#数据结构与算法全景图)
- [工业界应用与实战对照](#工业界应用与实战对照)
- [核心模块地图](#核心模块地图)
- [技术选型与复杂度速查](#技术选型与复杂度速查)
- [构建与测试](#构建与测试)
- [致敬经典参考书目](#致敬经典参考书目)

---

## 从这里开始

根据你的学习偏好，选择最适合的阅读方式：

| 入口 | 适合场景 | 说明 |
| :--- | :--- | :--- |
| [GitBook 在线教材](https://datastructure-1.gitbook.io/datastructure-docs) | **沉浸式在线阅读** | 侧边栏层级导航、自适应排版与目录联动 |
| [GitHub 分章文档](docs/README.md) | **源码与文档对照** | 仓库内分章速查，每个算法直接跳转到对应 `.cpp` 实现 |
| [完整教学文档（Markdown）](数据结构教学文档.md) | **离线搜索与二次编辑** | 单页汇编版，支持全文检索与笔记整理 |
| [完整教学文档（PDF）](数据结构教学文档.pdf) | **出版级平板阅读与打印** | 排版精美、代码高亮、支持书签跳转的高清电子书 |

---

## 设计哲学与教学特色

不同于市面上常见的“纯刷题题解”或“纯理论伪代码”资料，本项目致力于成为一本**接地气、有深度、无 AI 废话套话**的硬核教学参考：

1. **动机先行（Why & Evolution）**  
   任何数据结构都不是凭空出现的。本书在讲解每个结构前，都会还原前人面临的工程瓶颈与技术演进轨迹（例如：*顺序表插入慢促生了链表；单链表无法反向检索促生了双链表；BST 面对倾斜数据退化促生了 AVL 与红黑树；磁盘 I/O 延迟制约促生了 B/B+ 树；全文本搜索空间爆炸促生了后缀数组*）。

2. **不变量（Invariants）驱动设计**  
   不仅告诉你“怎么写”，更通过严格的不变量（如红黑树黑高相同、二叉搜索树整棵子树大小序、大顶堆根节点最大性、Dijkstra 贪心松弛界）推导“为什么它是正确的”，培养严谨的算法证明思维。

3. **分步手算演练（Step-by-step Dry Run）**  
   复杂的算法（如 KMP `next`/`nextval` 计算、AVL 四种旋转、红黑树插入与删除修复的四类变换、AOE 关键路径最早/最迟事件推导、TimSort 奔步合并）均配备**完整的分步追踪表**与**精心对齐的 ASCII 演化图**，拒绝黑盒。

4. **对标工业级实现与现代 C++17 规范**  
   遵循现代 C++ 最佳实践，正确处理深浅拷贝、RAII 内存安全、哨兵节点（Dummy Node）消除边界特判、`std::string_view` 零拷贝视图、`std::optional` 等特性。

---

## 数据结构与算法全景图

```text
                                  【数据结构与算法技术体系】
                                              │
    ┌────────────────────┬────────────────────┼────────────────────┬────────────────────┐
    ▼                    ▼                    ▼                    ▼                    ▼
【线性结构】          【树与二叉树】        【图论算法】          【查找与集合】        【排序与索引】
 ├─ 动态数组(Vector)   ├─ 二叉搜索树(BST)    ├─ 邻接表/矩阵/前向星  ├─ 散列表(Hash Table) ├─ 十大排序(内排序)
 ├─ 单向/双向/循环链表 ├─ 平衡树(AVL/红黑树) ├─ BFS/DFS 遍历       ├─ 并查集(DSU)        ├─ 外部排序(K路归并)
 ├─ 栈与队列(Queue)    ├─ 自适应伸展树(Splay) ├─ 拓扑排序(Kahn)     └─ 完美哈希(FKS)      ├─ 败者树(Loser Tree)
 └─ 串匹配(KMP/优化)   ├─ 优先队列(二叉/左式堆)├─ 最短路径(Dijkstra/Floyd)                ├─ 后缀数组(SA/Kasai)
                       ├─ 多路搜索树(B/B+树) └─ 最小生成树(Prim/Kruskal)                 └─ Ukkonen后缀树
                       └─ 空间索引(KD-Tree)
```

---

## 工业界应用与实战对照

理论终将服务于工程实践。下表展示了本项目中的数据结构在经典系统软件中的核心对应：

| 数据结构 / 算法 | 经典工业级系统应用场景 | 为什么选择它？ |
| :--- | :--- | :--- |
| **红黑树 (Red-Black Tree)** | Linux CFS 进程调度器、`std::map`/`std::set`、epoll | 最坏情况 O(log n)，插入删除至多 3 次旋转，综合写性能优于 AVL |
| **B+ 树 (B+ Tree)** | MySQL (InnoDB)、PostgreSQL、文件系统 (NTFS, XFS) | 树高极矮（3~4层容纳上千万行），叶子节点双向顺序相连，完美贴合磁盘预读与范围查询 |
| **KD 树 (K-D Tree)** | 激光雷达点云匹配、游戏物理引擎最近邻检索、高维特征检索 | 多维欧氏空间的正交超平面切分，有效剪枝空间范围搜索 |
| **跳表 / 哈希表** | Redis (zset / dict)、Memcached | 简单高效的 O(1) 平均查找；跳表实现简洁且范围查询表现优异 |
| **TimSort** | Python `sort()`、Java `Arrays.sort()`、V8 引擎 | 真实世界数据天然具备局部有序段（Run），自适应归并极大减少比较次数 |
| **并查集 (DSU)** | 网络动态连通性判断、图论 Kruskal 最小生成树、社交网络圈子分析 | 路径压缩 + 按秩合并后，单次操作均摊接近常数时间 \(\alpha(n)\) |
| **后缀数组 (Suffix Array)** | 生物信息学 DNA 序列比对、搜索引擎倒排索引、Git diff 核心 | 极少内存占用实现全文本快速子串检索与最长公共子串 (LCS) 分析 |

---

## 核心模块地图

项目中包含 18 个独立的 C++17 实现文件，每个文件均可独立编译、运行自测：

| 分类 | 源文件 | 核心数据结构 / 算法 | 关键技术与设计不变量 |
| :--- | :--- | :--- | :--- |
| **理论基础** | [`complexity_adt.cpp`](complexity_adt.cpp) | 渐进记号、ADT、均摊分析 | 大 O/Ω/Θ 严格界、主定理三种情况、记账法与势能法 |
| **线性结构** | [`Linked list.cpp`](Linked%20list.cpp) | 单向/双向/循环链表 | 哨兵节点消除边界、快慢指针判环与寻中、三指针原地翻转 |
| **受限结构** | [`linear_structures.cpp`](linear_structures.cpp) | 顺序栈、循环队列、双端队列、最小栈 | 循环队列判满两种方案、单调栈/单调队列模型、双栈模拟队列 |
| **模式匹配** | [`string_match.cpp`](string_match.cpp) | 朴素匹配、经典 KMP、`nextval` 优化 | 最长相等真前后缀、主串指针不回退不变量、连续失配消除 |
| **二叉树** | [`binary_tree.cpp`](binary_tree.cpp) | 二叉树、BST、顺序统计树 (Rank) | 递归与显式栈遍历、BST 删除三情况（后继替换）、子树大小维护 |
| **优先队列** | [`heap.cpp`](heap.cpp) | 最大堆、最小堆、左式堆 (Leftist) | 数组索引映射、下沉建堆 O(n) 推导、零路径长 (NPL) 可并堆 |
| **自平衡树** | [`avl_tree.cpp`](avl_tree.cpp) | AVL 树 | 平衡因子 (BF ∈ {-1,0,1})、LL/RR/LR/RL 单双旋高度维护 |
| **自平衡树** | [`redblacktree.cpp`](redblacktree.cpp) | 红黑树 (Red-Black Tree) | 五大红黑性质、黑高不变量、插入修复 (3种)、删除修复 (4种) |
| **自适应树** | [`splaytree.cpp`](splaytree.cpp) | 伸展树 (Splay Tree) | 局部性访问优化、Zig/Zig-Zig/Zig-Zag 旋转、势能均摊证明 |
| **多路搜索树**| [`multiwaytree.cpp`](multiwaytree.cpp) | B 树 (B-Tree) | 节点容量平衡、满节点中间键上提分裂、下溢向兄弟借位与合并 |
| **磁盘索引** | [`bplustree.cpp`](bplustree.cpp) | B+ 树 (B+ Tree) | 路由键与数据分离、叶子双向链表、区间检索与分页机制 |
| **空间索引** | [`kdtree.cpp`](kdtree.cpp) | KD 树 (k-d Tree) | 维度轮换超平面划分、超球-超矩形相交剪枝、KNN 最近邻搜索 |
| **图论算法** | [`graph.cpp`](graph.cpp) | 遍历、拓扑、最短路、MST、AOE | 邻接表/矩阵、Dijkstra 贪心证明、Kruskal 环检测、关键路径手算 |
| **哈希技术** | [`hashing.cpp`](hashing.cpp) | 拉链法、开放寻址、二次探测、FKS | 散列分布均匀性、二次聚集抑制、动态 Rehashing、完美哈希 |
| **并查集** | [`disjointset.cpp`](disjointset.cpp) | Disjoint Set Union (DSU) | 路径压缩 (Path Compression)、按秩合并 (Union by Rank) |
| **排序大全** | [`sort.cpp`](sort.cpp) | 十大排序、TimSort、外部排序 | 比较排序下界 Ω(n log n)、Lomuto/Hoare 划分、败者树 K 路归并 |
| **后缀索引** | [`suffixarray.cpp`](suffixarray.cpp) | 后缀数组 (SA)、LCP 数组 | 倍增算法 (O(n log n))、Kasai 线性时间求 Height、子串检索 |
| **后缀索引** | [`suffixtree.cpp`](suffixtree.cpp) | Ukkonen 后缀树 | 隐式后缀树、Active Point 状态机、Suffix Link 快速跳转 |

---

## 技术选型与复杂度速查

下表梳理了常用数据结构在最坏/平均情况下的时空开销与适用场景：

| 数据结构 | 查找 | 插入 | 删除 | 空间复杂度 | 最适合场景 |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **动态数组 (Vector)** | \(O(1)\) 下标 | \(O(1)\) 均摊尾插 | \(O(n)\) | \(O(n)\) | 频繁随机访问、元素量稳定或末尾操作为主 |
| **双向链表 (List)** | \(O(n)\) | \(O(1)\)* | \(O(1)\)* | \(O(n)\) | 频繁在已知位置插入/删除、LRU 缓存淘汰 |
| **AVL 树** | \(O(\log n)\) | \(O(\log n)\) | \(O(\log n)\) | \(O(n)\) | 查找操作极其频繁、插入删除相对较少的静态字典 |
| **红黑树** | \(O(\log n)\) | \(O(\log n)\) | \(O(\log n)\) | \(O(n)\) | 读写综合频繁的通用关联容器（如 `std::map`） |
| **哈希表** | \(O(1)\) 平均 | \(O(1)\) 平均 | \(O(1)\) 平均 | \(O(n)\) | 高频键值点对点快速检索，无需范围与顺序遍历 |
| **二叉堆 (Heap)** | \(O(1)\) 取最值 | \(O(\log n)\) | \(O(\log n)\) | \(O(n)\) | 优先级任务调度、Top-K 实时筛选、Dijkstra 优化 |
| **B+ 树** | \(O(\log_m n)\) | \(O(\log_m n)\) | \(O(\log_m n)\) | \(O(n)\) | 磁盘文件系统、关系型数据库主键与二级索引 |
| **并查集 (DSU)** | \(O(\alpha(n))\) | \(O(\alpha(n))\) | - | \(O(n)\) | 图连通分支维护、Kruskal 判环、等价类合并 |

> `*` 链表的 \(O(1)\) 插入与删除前提为已持有目标节点的前驱或当前指针。

---

## 构建与测试

### 使用 CMake 与 CTest（推荐）

本项目支持标准 CMake 构建流程，每个源文件均注册为独立的编译目标和 CTest 测试项：

```bash
# 1. 配置并生成构建文件（推荐 Release 模式以获得最佳性能）
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 2. 多核并发编译所有模块
cmake --build build --parallel

# 3. 运行全套自动化测试并输出诊断信息
ctest --test-dir build --output-on-failure
```

### 单文件快速编译运行

你也可以直接使用支持 C++17 的编译器独立编译任意单个模块：

```powershell
# 以排序算法模块为例
g++ -std=c++17 -O2 -Wall -Wextra sort.cpp -o sort.exe
.\sort.exe

# 以红黑树模块为例
g++ -std=c++17 -O2 -Wall -Wextra redblacktree.cpp -o redblacktree.exe
.\redblacktree.exe
```

---

## 致敬经典参考书目

在编写本教程与代码实现的过程中，深度借鉴并汲取了以下国内外经典著作的教学精华：

1. **严蔚敏, 李冬梅, 吴伟民** —— 《数据结构（C语言版）》  
   *国内计算机专业奠基教材，奠定了线性表、二叉树、图论遍历与关键路径的标准教学范式。*
2. **邓俊辉** —— 《数据结构（C++语言版）》（清华大学）  
   *以严格的算法不变量（Invariants）推导、分摊复杂度分析与面向对象架构著称，极具思维深度。*
3. **程杰** —— 《大话数据结构》  
   *用通俗幽默的生活比喻与演进动机拆解抽象概念，是入门与直观理解的典范。*
4. **Thomas H. Cormen, Charles E. Leiserson, Ronald L. Rivest, Clifford Stein (CLRS)** —— *Introduction to Algorithms*（《算法导论》）  
   *全球算法圣经，在渐进分析、红黑树、动态规划与图论证明上提供了无可替代的理论基石。*
5. **Robert Sedgewick, Kevin Wayne** —— *Algorithms (4th Edition)*  
   *图文并茂的经典之作，红黑树与排序算法的讲解尤为精辟。*

---

## 开源协议与贡献

本项目代码与教学文档遵循 [MIT License](LICENSE) 协议开源。欢迎提交 Issue 探讨算法细节，或发起 Pull Request 完善教学案例！
