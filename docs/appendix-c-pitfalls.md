[← 附录 B：常见面试题与练习](appendix-b-exercises.md) | [课程目录](README.md) | 已到末页 →

> 补充材料：review, selection, and self-check material.

## 本章导航

- [指针陷阱](#appendix-c-topic-01)
- [递归陷阱](#appendix-c-topic-02)
- [边界陷阱](#appendix-c-topic-03)
- [算法选择陷阱](#appendix-c-topic-04)
- [性能陷阱](#appendix-c-topic-05)

---

<a id="appendix-c"></a>
# 附录 C：常见陷阱与注意事项

<a id="appendix-c-topic-01"></a>
## 指针陷阱
```
1. delete 后未置 nullptr → 野指针，后续使用崩溃
2. new 后忘记 delete → 内存泄漏
3. 默认拷贝构造只复制指针 → 两个对象共享一棵树 → double free
   → 自定义深拷贝 or = delete
4. 递归删除树 → 必须后序遍历（先子后父）
```

<a id="appendix-c-topic-02"></a>
## 递归陷阱
```
1. 忘记 base case → 无限递归爆栈
2. 递归返回值不接收 → BST 删除/插入时丢失父子链接
3. 深度过大 → 栈溢出（改用迭代 or 尾递归）
4. 重复计算 → 使用备忘录（记忆化搜索）
```

<a id="appendix-c-topic-03"></a>
## 边界陷阱
```
1. 取 (i-1)/2 前确保 i>0
2. pop()/top() 前先检查 !IsEmpty()
3. 循环队列：(rear+1)%cap == front 判满（牺牲一个位置）
4. int vs size_t：有符号和无符号混用导致意外行为
5. 链表操作时注意处理空链表和单节点链表的特殊情况
```

<a id="appendix-c-topic-04"></a>
## 算法选择陷阱
```
1. 稠密图上用 Kruskal 不用 Prim O(V²) → 忽略 Prim 在稠密图上的优势
2. 需要范围查询时用哈希表 → 哈希表不支持有序遍历
3. 小数据 (n<50) 上用快速排序 → 插入排序常数更小
4. 数据已排好序时 BST 逐个插入 → 退化为链表 O(n²)
5. 对流式数据用归并排序 → 归并排序需要看到全部数据
```

<a id="appendix-c-topic-05"></a>
## 性能陷阱
```
1. 哈希表最坏 O(n)：选择好的哈希函数 + 控制负载因子
2. BST 退化成链表：使用自平衡变体（AVL/红黑树）
3. 递归建树 O(n²)：每次取中位数确保 O(n log n)
4. 大对象传值：传引用避免 O(n) 拷贝
```

---

> 本文档基于 18 个可编译运行的 C++ 文件编写，按经典教材体系编排，从绪论到高级字符串，递进讲解。每个章节配备前置知识、内容详解、本章小结和思考题，适合自学和教学参考。
>
> 学习建议：从头到尾按顺序阅读，每学完一章尝试回答思考题。代码实现细节请参阅各章标注的 .cpp 源文件。

---

[← 附录 B：常见面试题与练习](appendix-b-exercises.md) | [课程目录](README.md) | 已到末页 →
