[<- 第 15 章　后缀树与后缀数组](15-suffix-tree-and-array.md) | [课程目录](README.md) | [附录 B：常见面试题与练习 ->](appendix-b-exercises.md)

> 补充材料：review, selection, and self-check material.

## 本章导航

- Reference page

---

<a id="appendix-a"></a>
# 附录 A：数据结构选择速查

| 需求 | 推荐结构 | 复杂度 |
|------|----------|--------|
| 快速增删，不需频繁查找 | 链表 | O(1) 插入/删除 |
| 快速查找 + 有序遍历 | 平衡 BST | O(log n) |
| 最快查找（无序） | 哈希表 | O(1) 平均 |
| 优先级队列 | 堆 | O(log n) push/pop |
| 局部性访问 | 伸展树 | 均摊 O(log n) |
| 大量数据存磁盘 | B 树 / B+ 树 | O(log_m n) |
| 判断连通性 | 并查集 | ≈O(1) |
| 多维搜索 | KD 树 | 平均 O(log n) |
| 模式匹配（单次） | KMP | O(n+m) |
| 模式匹配（多次） | 后缀树 / 后缀数组 | O(m) / O(m log n) |
| 范围查询（数据库） | B+ 树 | O(log_m n + k) |
| Top-K 问题 | 最小堆 | O(n log k) |
| 数据流中位数 | 双堆 | 插入 O(log n) |
| 滑动窗口最大值 | 单调队列 | O(n) |
| 区间最值查询 | 稀疏表 / 线段树 | O(1) / O(log n) |
| 大文件排序 | 外部归并 | O(N log N) I/O |

---

[<- 第 15 章　后缀树与后缀数组](15-suffix-tree-and-array.md) | [课程目录](README.md) | [附录 B：常见面试题与练习 ->](appendix-b-exercises.md)
