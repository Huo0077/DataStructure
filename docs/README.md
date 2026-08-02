# 数据结构与算法 — GitHub 分章学习文档

> 本目录将 65 页 PDF 教材整理为可独立阅读、可稳定链接的 Markdown 页面。每章保留原有的理论、示例、小结、思考题，并直接链接到对应的 C++17 实现。

[返回项目主页](../README.md) | [单页 Markdown 版](../数据结构教学文档.md) | [PDF 打印版](../数据结构教学文档.pdf)

## 如何使用

1. 从第 1 章开始顺序学习；每页都包含前置知识、章节导航、小结、思考题和源码入口。
2. 使用页面首尾的“上一章 / 课程目录 / 下一章”链接进行连续阅读。
3. 每个小节都有稳定的 ASCII 锚点。例如 [`#c04-4-6`](04-string-matching-kmp.md#c04-4-6) 直接定位到 KMP 匹配过程。
4. 需要全文搜索、离线保存或打印时，可使用单页 Markdown 版或 PDF 版。

## 课程路径

### 第一部分：绪论

| 章 | 学习主题 | 对应源码 |
| :---: | --- | --- |
| 01 | [复杂度分析与 ADT](01-complexity-adt.md) | [`complexity_adt.cpp`](../complexity_adt.cpp) |

### 第二部分：线性结构

| 章 | 学习主题 | 对应源码 |
| :---: | --- | --- |
| 02 | [线性表](02-linear-list.md) | [`Linked list.cpp`](../Linked%20list.cpp) |
| 03 | [栈与队列](03-stacks-and-queues.md) | [`linear_structures.cpp`](../linear_structures.cpp) |

### 第三部分：串

| 章 | 学习主题 | 对应源码 |
| :---: | --- | --- |
| 04 | [字符串匹配（KMP）](04-string-matching-kmp.md) | [`string_match.cpp`](../string_match.cpp) |

### 第四部分：树与二叉树

| 章 | 学习主题 | 对应源码 |
| :---: | --- | --- |
| 05 | [二叉树基础与遍历](05-binary-tree-basics.md) | [`binary_tree.cpp`](../binary_tree.cpp) |
| 06 | [二叉搜索树](06-binary-search-tree.md) | [`binary_tree.cpp`](../binary_tree.cpp) |
| 07 | [堆与优先队列](07-heap-and-priority-queue.md) | [`heap.cpp`](../heap.cpp) |
| 08 | [平衡二叉树](08-balanced-binary-search-trees.md) | [`avl_tree.cpp`](../avl_tree.cpp) / [`redblacktree.cpp`](../redblacktree.cpp) / [`splaytree.cpp`](../splaytree.cpp) |
| 09 | [多路搜索树](09-multiway-search-trees.md) | [`multiwaytree.cpp`](../multiwaytree.cpp) / [`bplustree.cpp`](../bplustree.cpp) |
| 10 | [多维搜索树（KD 树）](10-kd-tree.md) | [`kdtree.cpp`](../kdtree.cpp) |

### 第五部分：图

| 章 | 学习主题 | 对应源码 |
| :---: | --- | --- |
| 11 | [图论算法](11-graph-algorithms.md) | [`graph.cpp`](../graph.cpp) |

### 第六部分：查找

| 章 | 学习主题 | 对应源码 |
| :---: | --- | --- |
| 12 | [哈希表](12-hash-tables.md) | [`hashing.cpp`](../hashing.cpp) |
| 13 | [并查集](13-disjoint-set-union.md) | [`disjointset.cpp`](../disjointset.cpp) |

### 第七部分：排序

| 章 | 学习主题 | 对应源码 |
| :---: | --- | --- |
| 14 | [排序算法](14-sorting-algorithms.md) | [`sort.cpp`](../sort.cpp) |

### 第八部分：高级字符串

| 章 | 学习主题 | 对应源码 |
| :---: | --- | --- |
| 15 | [后缀树与后缀数组](15-suffix-tree-and-array.md) | [`suffixtree.cpp`](../suffixtree.cpp) / [`suffixarray.cpp`](../suffixarray.cpp) |

## 附录

| 内容 | 用途 |
| --- | --- |
| [附录 A：数据结构选择速查](appendix-a-quick-reference.md) | 按需求选择数据结构与算法 |
| [附录 B：常见面试题与练习](appendix-b-exercises.md) | 课后自测与面试准备 |
| [附录 C：常见陷阱与注意事项](appendix-c-pitfalls.md) | 指针、递归、边界与性能复盘 |

## 仓库构建

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

> 建议学习闭环：读完一章，运行对应的 `.cpp` 演示程序，再完成该章思考题。理论、实现与验证会彼此加固。
