[<- 第 1 章　复杂度分析与 ADT](01-complexity-adt.md) | [课程目录](README.md) | [第 3 章　栈与队列 ->](03-stacks-and-queues.md)

> 所属模块：第二部分：线性结构  
> 对应实现：Linked list.cpp

## 本章导航

- [2.1 线性表的两种实现](#c02-2-1)
  - [2.1.1 顺序表（数组实现）](#c02-2-1-1)
  - [2.1.2 链表 vs 顺序表 对比](#c02-2-1-2)
- [2.2 单向链表](#c02-2-2)
- [2.3 双向链表](#c02-2-3)
- [2.4 循环链表](#c02-2-4)
- [2.5 链表的核心技巧](#c02-2-5)
  - [快慢指针 (Floyd's Tortoise and Hare)](#c02-topic-01)
  - [哨兵节点 (Dummy Node)](#c02-topic-02)
- [本章小结](#c02-summary)
- [思考题](#c02-exercises)

---

<a id="chapter-02"></a>
# 第 2 章　线性表

> 难度：★☆☆☆☆ | 代码：[Linked list.cpp](https://github.com/Huo0077/DataStructure/blob/main/Linked%20list.cpp)

**前置知识**：[第 1 章](01-complexity-adt.md) 复杂度分析（理解 O(1) vs O(n) 操作的含义，为选择顺序表还是链表提供判断依据）。

**学习目标**：理解顺序存储与链式存储的本质区别，掌握单向/双向/循环链表的操作和核心技巧。

<a id="c02-2-1"></a>
## 2.1 线性表的两种实现

<a id="c02-2-1-1"></a>
### 2.1.1 顺序表（数组实现）

```cpp
int data[MAX_SIZE];  // 存储空间
int length;          // 当前元素个数（≤ MAX_SIZE）
```

| 操作 | 复杂度 | 实现要点 |
|------|--------|----------|
| 随机访问 | O(1) | 直接下标寻址 |
| 尾部插入 | O(1) | `data[length++] = val` |
| 中间插入 | O(n) | 插入位置后的所有元素后移一位 |
| 删除 | O(n) | 删除位置后的所有元素前移一位 |
| 查找 | O(n) | 顺序扫描；若有序可用二分 O(log n) |

**关键概念：逻辑长度 ≠ 物理容量**——length 告诉程序"当前用了多少"，容量告诉程序"还能装多少"。这是贯穿整个课程的基础概念（栈、队列、堆、哈希表都有类似的"满"判断）。

<a id="c02-2-1-2"></a>
### 2.1.2 链表 vs 顺序表 对比

| | 顺序表（数组） | 链表 |
|------|------|------|
| 存储方式 | 连续内存 | 节点 + 指针 |
| 随机访问 | O(1) | O(n) |
| 插入/删除 | O(n)（需要移动） | O(1)（已有位置引用时） |
| 缓存友好 | 是（连续内存） | 否（内存分散） |
| 空间开销 | 无额外指针 | 每节点 1-2 个指针 |
| 动态扩容 | 需要复制 | 天然支持 |
| **适用场景** | 频繁随机访问、尾部操作 | 频繁头部/中间增删 |

**选择原则**：随机访问多 → 数组；插入删除多 → 链表；两者都频繁 → 平衡树（见第 6-8 章）。

<a id="c02-2-2"></a>
## 2.2 单向链表

每个节点包含 `data` 和 `next` 指针。

```
head → [10|next] → [20|next] → [30|next] → nullptr
```

| 操作 | 复杂度 | 实现要点 |
|------|--------|----------|
| 头插 | O(1) | `newNode->next = head; head = newNode` |
| 尾插 | O(n) | 需遍历到末尾（可维护 tail 指针优化到 O(1)） |
| 删除 | O(n) | 需找到**前驱**节点来修正链接 |
| 反转 | O(n) | 三指针法：prev, curr, next |

**反转算法（三指针法）**：

```
prev = nullptr, curr = head
while curr != nullptr:
    next = curr->next     // 暂存下一个节点
    curr->next = prev     // 翻转：当前指向前一个
    prev = curr           // 前移
    curr = next           // 前移
head = prev               // prev 指向原链表的末尾 = 新头

示例: 1→2→3→null → null←1←2←3
```

<a id="c02-2-3"></a>
## 2.3 双向链表

每个节点有 `prev` 和 `next` 两个指针。

```
nullptr ← [prev|10|next] ↔ [prev|20|next] ↔ [prev|30|next] → nullptr
```

优势：删除给定节点只需 O(1)，因为可以直接通过 prev 找到前驱。

```
// 双向链表删除节点 node 只需两行：
node->prev->next = node->next;
node->next->prev = node->prev;
// 对比单向链表：需要从 head 遍历找到 node 的前驱 → O(n)
```

<a id="c02-2-4"></a>
## 2.4 循环链表

尾节点指向头节点，形成环。经典应用：**约瑟夫问题**——n 个人围成一圈，每次数到第 m 个人出局。

```
n=5, m=3:
  1→2→3→4→5→(回到1)
  第1轮数到3 → 3出局
  第2轮从4开始数3 → 1出局
  ... 直到只剩一人
```

<a id="c02-2-5"></a>
## 2.5 链表的核心技巧

<a id="c02-topic-01"></a>
### 快慢指针 (Floyd's Tortoise and Hare)

```
slow 每次走 1 步，fast 每次走 2 步：
  检测环:     slow 和 fast 相遇 → 有环
  找环入口:   相遇后 slow 回 head，两者每次走 1 步，再次相遇点 = 环入口
  找中点:     fast 到末尾时 slow 恰好在中间
```

**为什么 fast 和 slow 一定相遇？** fast 每次比 slow 多走 1 步。如果有环，fast 进入环后会逐步"追上"slow，相当于跑圈套圈。由于差距每次缩小 1，必然相遇。

<a id="c02-topic-02"></a>
### 哨兵节点 (Dummy Node)

在链表头前加一个虚拟节点，统一处理空链表和头节点修改的边界情况。

```cpp
// 合并两个有序链表 —— 用哨兵简化代码
ListNode* merge(ListNode* a, ListNode* b) {
    ListNode dummy(0);
    ListNode* tail = &dummy;
    while (a && b) {
        if (a->data < b->data) { tail->next = a; a = a->next; }
        else                   { tail->next = b; b = b->next; }
        tail = tail->next;
    }
    tail->next = a ? a : b;
    return dummy.next;  // 哨兵的下一个才是真正头节点
}
```

---

<a id="c02-summary"></a>
## 本章小结
- 线性表的两种存储方式（顺序 vs 链式）各有适用场景，没有绝对的优劣
- 链表的指针操作是后续树结构的基础——左/右指针本质上就是链表的 next 指针推广
- 三指针反转、快慢指针、哨兵节点是链表操作的三大核心技巧
- 理解"为什么双向链表删除是 O(1) 而单向链表不是"——这揭示了一个深层道理：**结构的冗余（额外指针）换取操作的效率**

<a id="c02-exercises"></a>
## 思考题
1. 递归反转单链表：写出递归实现，并追踪 `1→2→3→null` 的调用栈。迭代和递归版本的空间复杂度分别是多少？
2. 证明 Floyd 判环算法中，slow 和 fast 必定在 slow 走完第一圈之前相遇。
3. 设计一个支持以下操作的结构：`getMiddle()` 返回链表中点（O(1)），`push(x)` 在头部插入（O(1)），`pop()` 移除头部（O(1)）。

---

[<- 第 1 章　复杂度分析与 ADT](01-complexity-adt.md) | [课程目录](README.md) | [第 3 章　栈与队列 ->](03-stacks-and-queues.md)
