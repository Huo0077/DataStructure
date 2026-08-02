# C++17 数据结构与算法核心实现

本项目是一个面向底层原理理解与工程复盘的 C++17 数据结构与算法实践库。包含 18 个独立编译、独立运行的源文件，涵盖线性表、受限线性表、字符串匹配、自平衡二叉搜索树、多路搜索树、多维空间索引、图论算法、哈希表、并查集、经典排序算法及高级字符串索引结构（后缀树/后缀数组）。

每个模块均将**理论不变量说明**、**算法核心实现**、**详细注释演进**与**自我验证 Demo** 整合于单一源文件中。

---

## 🛠️ 构建与编译指南

### 1. CMake 批量构建（推荐）

项目根目录提供 `CMakeLists.txt`，为每个 `.cpp` 文件创建独立的可执行目标，并注册 CTest 单元测试。

```bash
# 生成构建文件
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# 编译所有目标
cmake --build build --parallel

# 运行自动化测试
ctest --test-dir build --output-on-failure
```

### 2. Windows MinGW64 环境说明

在 Windows 环境下，若安装了多套 GCC / Clang 工具链（如 `D:\MinGW64` 与 WinLibs UCRT / LLVM），为了避免头文件与 DLL 运行库冲突（如 `0xc0000139` 入口点错误），请在终端窗口中指定正确的编译器路径：

#### 使用 GCC (MinGW64):
```powershell
$env:Path = "C:\Users\Huo\AppData\Local\Microsoft\WinGet\Packages\BrechtSanders.WinLibs.POSIX.UCRT.LLVM_Microsoft.WinGet.Source_8wekyb3d8bbwe\mingw64\bin;$env:Path"
g++ -std=c++17 -Wall -Wextra -Wpedantic sort.cpp -o sort.exe
.\sort.exe
```

#### 使用 Clang++:
```powershell
clang++ -std=c++17 -Wall -Wextra -Wpedantic redblacktree.cpp -o redblacktree.exe
.\redblacktree.exe
```

---

## 🗺️ 内容地图与算法索引

| 分类 | 源文件 | 核心数据结构 / 算法 | 关键技术与不变量 |
| :--- | :--- | :--- | :--- |
| **理论基础** | `complexity_adt.cpp` | 渐进符号 (O, Ω, Θ)、ADT | 主定理 (Master Theorem)、均摊分析 (势能法/核算法) |
| **线性结构** | `Linked list.cpp` | 单向链表、双向链表 | 快慢指针、头尾插法、链表反转、节点删除 |
| **受限结构** | `linear_structures.cpp` | 顺序栈、链栈、循环队列、双端队列 | 满/空边界条件、最小栈 O(1) 检索、双栈模拟队列 |
| **模式匹配** | `string_match.cpp` | 朴素匹配、KMP 算法 | 前缀函数 $\pi[i]$ 推导、`next` 与 `nextval` 数组优化 |
| **二叉搜索树**| `binary_tree.cpp` | BST、顺序统计树 (Order Statistic) | 中序遍历有序性、三类节点删除逻辑、Rank 查找 |
| **堆与队列** | `heap.cpp` | 最小堆、最大堆、左式堆 (Leftist Heap) | 上浮 `siftUp` / 下沉 `siftDown`、零路径长度 NPL、Top-K |
| **平衡 BST** | `avl_tree.cpp` | AVL 树 | 平衡因子 $BF \in \{-1,0,1\}$、LL/RR/LR/RL 旋转调节 |
| **平衡 BST** | `redblacktree.cpp` | 红黑树 (Red-Black Tree) | 5 大红黑性质、黑高一致性、插入/删除变色与旋转 |
| **自适应 BST**| `splaytree.cpp` | 伸展树 (Splay Tree) | 单旋转 (Zig/Zag)、双旋转 (Zig-Zig/Zig-Zag)、均摊 O(log n) |
| **多路搜索树**| `multiwaytree.cpp` | B 树 (B-Tree) | 阶数 $M$ 下限约束、节点分裂 (Split)、借位与合并 |
| **磁盘索引** | `bplustree.cpp` | B+ 树 | 路由键 (Routing Keys)、叶子节点双向链表、范围查询 |
| **空间索引** | `kdtree.cpp` | KD 树 (K-Dimensional Tree) | 维度轮换切分 (Cycle Dimension)、KNN 最近邻搜索、范围剪枝 |
| **图论算法** | `graph.cpp` | BFS, DFS, Dijkstra, Floyd, Prim, Kruskal, 拓扑排序, AOE 关键路径 | 邻接表/矩阵、松弛操作 (Relaxation)、入度统计、最早/最迟发生时间 |
| **哈希表** | `hashing.cpp` | 拉链法、开放定址法、双重散列、FKS 完美哈希 | 负载因子 $\alpha$、二次探测法、两级哈希无冲突查找 |
| **不相交集** | `disjointset.cpp` | 并查集 (Disjoint Set Union) | 路径压缩 (Path Compression)、按秩合并 (Union by Rank)、阿克曼反函数 $\alpha(n)$ |
| **排序算法** | `sort.cpp` | 10 大比较与非比较排序 (含 TimSort 思想) | 稳定性、原址性、划分 Pivot 选择、小子数组插入排序优化 |
| **高级字符串**| `suffixarray.cpp` | 后缀数组 (Suffix Array), LCP | 倍增算法 $O(n \log n)$、Kasai $O(n)$ LCP 数组计算 |
| **高级字符串**| `suffixtree.cpp` | Ukkonen 后缀树 | 隐式后缀树、Active Point 轮换、Suffix Link 快速跳转 |

---

## 📊 数据结构时空复杂度速查

| 数据结构 | 平均查找 | 最坏查找 | 平均插入 | 最坏插入 | 平均删除 | 最坏删除 | 额外空间 |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **动态数组 (`vector`)** | $O(1)$ 下标 | $O(1)$ 下标 | $O(1)$ 均摊 | $O(n)$ 扩容 | $O(n)$ | $O(n)$ | $O(n)$ |
| **单/双向链表** | $O(n)$ | $O(n)$ | $O(1)^*$ | $O(1)^*$ | $O(1)^*$ | $O(1)^*$ | $O(n)$ |
| **二叉搜索树 (BST)** | $O(\log n)$ | $O(n)$ | $O(\log n)$ | $O(n)$ | $O(\log n)$ | $O(n)$ | $O(n)$ |
| **AVL 树** | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(n)$ |
| **红黑树** | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(n)$ |
| **伸展树 (Splay)** | $O(\log n)$ 均摊 | $O(n)$ | $O(\log n)$ 均摊 | $O(n)$ | $O(\log n)$ 均摊 | $O(n)$ | $O(n)$ |
| **B/B+ 树** | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ | $O(n)$ |
| **哈希表 (拉链法)** | $O(1)$ | $O(n)$ | $O(1)$ | $O(n)$ | $O(1)$ | $O(n)$ | $O(n)$ |
| **二叉堆 (Heap)** | $O(1)$ 堆顶 | $O(n)$ 任意 | $O(\log n)$ | $O(\log n)$ | $O(\log n)$ 堆顶 | $O(\log n)$ | $O(n)$ |
| **并查集 (DSU)** | $O(\alpha(n))$ | $O(\alpha(n))$ | $O(\alpha(n))$ | $O(\alpha(n))$ | N/A | N/A | $O(n)$ |

> $*$ 注：链表插入删除为 $O(1)$ 的前提是已经获取到目标节点的指针/引用。

---

## 🎯 建议学习路线

```
[第一阶段：基础结构与算法工具]
 complexity_adt.cpp -> Linked list.cpp -> linear_structures.cpp -> string_match.cpp
  │
  ├─> 掌握渐进分析、递归主定理与均摊分析
  ├─> 熟练掌握指针操作、边界处理与单双向链表反转
  └─> 理解受限线性表逻辑与 KMP 模式匹配前缀函数

[第二阶段：树形结构与排序原理]
 binary_tree.cpp -> heap.cpp -> sort.cpp
  │
  ├─> 掌握 BST 遍历、查找与三类节点删除逻辑
  ├─> 掌握二叉堆堆化（Heapify）与 Top-K 优先队列
  └─> 深入比较排序与非比较排序的底层移动与划分代价

[第三阶段：平衡搜索树与空间索引]
 avl_tree.cpp -> redblacktree.cpp -> splaytree.cpp -> multiwaytree.cpp -> bplustree.cpp -> kdtree.cpp
  │
  ├─> 对比 AVL 严格平衡与红黑树宽松平衡的旋转开销
  ├─> 理解 Splay 伸展树局部性原理与 B/B+ 树磁盘页分配
  └─> 掌握 KD 树空间切分与 KNN 剪枝搜索

[第四阶段：图论、哈希与字符串高级结构]
 graph.cpp -> hashing.cpp -> disjointset.cpp -> suffixarray.cpp -> suffixtree.cpp
  │
  ├─> 掌握最短路、最小生成树、拓扑排序与关键路径
  ├─> 理解哈希冲突解决、FKS 完美哈希与并查集路径压缩
  └─> 掌握后缀数组 SA/LCP 与 Ukkonen 后缀树线性建树
```

---

## 📚 详细教学文档

项目配有完整的理论教学与工程分析文档：
- **📚 模块化分章节文档（GitHub 按需阅读）**：[docs/ 目录](docs/README.md)
- **📄 Markdown 完整单页版**：[数据结构教学文档.md](数据结构教学文档.md)
- **📘 出版物排版 PDF 版**：[数据结构教学文档.pdf](数据结构教学文档.pdf)

配套文档结合了 CLRS《算法导论》、Sedgewick《Algorithms》与 Mark Allen Weiss《Data Structures and Algorithm Analysis》的技术路线，包含了公式推导、数据结构不变量证明、内存演化分析与工业级应用实例。

