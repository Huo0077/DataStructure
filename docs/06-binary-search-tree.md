[<- 第 5 章　二叉树基础与遍历](05-binary-tree-basics.md) | [课程目录](README.md) | [第 7 章　堆与优先队列 ->](07-heap-and-priority-queue.md)

> 所属模块：第四部分：树与二叉树  
> 对应实现：binary_tree.cpp

## 本章导航

- [6.1 二叉搜索树（BST）的定义与全局不变量](#c06-6-1)
- [6.2 BST 基础操作：查找、插入与前驱/后继定位](#c06-6-2)
  - [6.2.1 递归与迭代查找算法](#c06-6-2-1)
  - [6.2.2 插入操作与父子链接挂载不变量](#c06-6-2-2)
  - [6.2.3 前驱（Predecessor）与后继（Successor）的定位原理](#c06-6-2-3)
- [6.3 核心难点：BST 节点删除的三种情况与图解](#c06-6-3)
- [6.4 顺序倾斜与退化危机：为什么需要平衡树？](#c06-6-4)
- [6.5 BST 合法性验证的两种经典算法（中序遍历 vs 区间收缩）](#c06-6-5)
- [6.6 进阶实战：顺序统计树（Order Statistic Tree）查第 k 小](#c06-6-6)
- [本章小结](#c06-summary)
- [思考题与经典大厂面试题](#c06-exercises)

---

<a id="chapter-06"></a>
# 第 6 章　二叉搜索树

> 难度：★★☆☆☆ | 代码：[binary_tree.cpp](https://github.com/Huo0077/DataStructure/blob/main/binary_tree.cpp) BST 部分

**前置知识**：[第 5 章](05-binary-tree-basics.md) 二叉树遍历（中序遍历在 BST 下为有序序列），[第 1 章](01-complexity-adt.md) 树高相关的渐进复杂度。

**学习目标**：
1. 理解 BST 的全局不变量，彻底消除“只比对直接孩子”的局部性误区；
2. 掌握查找、插入、前驱/后继查找的指针逻辑；
3. 掌握删除节点的三大分支，透彻理解为什么用后继替换不会破坏 BST 性质；
4. 掌握利用子树规模（`size`）增强二叉树实现 \(O(\log n)\) 的 Rank 查询。

---

<a id="c06-6-1"></a>
## 6.1 二叉搜索树（BST）的定义与全局不变量

二叉搜索树（Binary Search Tree, BST），又称二叉查找树或二叉排序树。其核心定义如下：

> **BST 全局不变量**：  
> 设 \(x\) 为树中的任意节点。  
> - 若 \(y\) 是 \(x\) **左子树**中的任意节点，则必有 \(y.\text{key} < x.\text{key}\)；  
> - 若 \(z\) 是 \(x\) **右子树**中的任意节点，则必有 \(z.\text{key} > x.\text{key}\)。

```text
               10
             /    \
            5      15
           / \    /  \
          2   8  12  20
```

> ⚠️ **高频致命认知陷阱**：  
> 初学者常误以为性质只是“左孩子 < 根 < 右孩子”。  
> **反例**：如果 15 的左孩子是 12，而 12 的右孩子是 11（\(11 > 10\) 成立，\(11 < 12\) 错误；但如果 5 的右孩子是 8，8 的右孩子是 11，局部看 \(8 < 11\)，但 11 处于 10 的左子树中，破坏了全局不变量！）。

**数学推论**：  
**对任何 BST 进行中序遍历（LNR），得到的必定是严格单调递增的序列！**

---

<a id="c06-6-2"></a>
## 6.2 BST 基础操作：查找、插入与前驱/后继定位

<a id="c06-6-2-1"></a>
### 6.2.1 递归与迭代查找算法

查找过程类似于在树形结构上执行“二分查找”：
- 从根节点出发，若目标值等于当前节点，查找成功；
- 若目标值小于当前节点，转向左子树；
- 若目标值大于当前节点，转向右子树；
- 遇到 `nullptr` 则说明目标不存在。

```cpp
// 迭代版本：空间 O(1)，无函数调用栈开销
TreeNode* searchBST(TreeNode* root, int val) {
    TreeNode* curr = root;
    while (curr != nullptr && curr->val != val) {
        if (val < curr->val) curr = curr->left;
        else curr = curr->right;
    }
    return curr;
}
```

<a id="c06-6-2-2"></a>
### 6.2.2 插入操作与父子链接挂载不变量

新元素在 BST 中的插入位置**永远是某个叶子节点的空指针域**。

```cpp
TreeNode* insertBST(TreeNode* node, int val) {
    if (node == nullptr) return new TreeNode(val);
    if (val < node->val) {
        node->left = insertBST(node->left, val);
    } else if (val > node->val) {
        node->right = insertBST(node->right, val);
    }
    return node; // 返回当前节点以维护父子链接
}
```

> **关键代码细节**：  
> 必须使用 `node->left = insertBST(...)` 接收递归返回值！当到达 `nullptr` 实例化新节点时，返回值会直接赋给其父节点的子指针。如果不接收返回值，新节点虽被 `new` 出来，但与整棵树处于断连状态！

<a id="c06-6-2-3"></a>
### 6.2.3 前驱（Predecessor）与后继（Successor）的定位原理

在中序遍历序列中，排在节点 \(u\) 前面的称为其**直接前驱**，排在后面的称为其**直接后继**。

- **节点 \(u\) 拥有右子树**：  
  其后继必定是**右子树中的最小值**（从 `u->right` 出发，一路沿 `left` 指针走到尽头）。
- **节点 \(u\) 没有右子树**：  
  其后继必定是**某个祖先节点**——沿父指针向上回溯，直到找到第一个“以当前子树作为其左孩子”的祖先！

---

<a id="c06-6-3"></a>
## 6.3 核心难点：BST 节点删除的三种情况与图解

删除是 BST 所有操作中逻辑最精妙的部分。设要删除的节点为 \(Z\)：

### 情况 1：\(Z\) 为叶子节点（无左右孩子）
直接释放 \(Z\) 的内存，并令其父节点指向它的指针置为 `nullptr`。

### 情况 2：\(Z\) 仅有一个孩子（只有左孩子或只有右孩子）
让 \(Z\) 的唯一孩子直接取代 \(Z\) 的位置（其父节点跨过 \(Z\)，直接与孙子相连），然后释放 \(Z\)。

### 情况 3：\(Z\) 同时拥有两个孩子（左右子树均非空）
不能直接删除 \(Z\)，否则整棵树将分裂为互不相连的碎片！  
**后继替换法**：
1. 在 \(Z\) 的右子树中寻找其**直接后继 \(Y\)**（即右子树中的最小节点）；
2. 将 \(Y.\text{val}\) 拷贝覆盖到 \(Z.\text{val}\)；
3. 转而**递归删除节点 \(Y\)**！

```text
目标：删除节点 [10]
          10                      12  (用后继 12 覆盖 10)
        /    \                  /    \
       5      15       ───>    5      15
             /  \                    /  \
           [12]  20                nullptr 20  (原 12 被删去)
             \
             13
```

> **为什么后继 \(Y\) 一定容易删除？**  
> 因为 \(Y\) 是右子树中的最小值，**\(Y\) 绝对不可能拥有左孩子**！因此，删除 \(Y\) 必定退化为【情况 1】（\(Y\) 是叶子）或【情况 2】（\(Y\) 只有右孩子），绝不会再度触发复杂分支！

```cpp
TreeNode* deleteBST(TreeNode* root, int val) {
    if (!root) return nullptr;
    
    if (val < root->val) {
        root->left = deleteBST(root->left, val);
    } else if (val > root->val) {
        root->right = deleteBST(root->right, val);
    } else {
        // 找到了待删除节点 root
        if (!root->left) {
            TreeNode* temp = root->right;
            delete root;
            return temp; // 情况 1 & 2
        } else if (!root->right) {
            TreeNode* temp = root->left;
            delete root;
            return temp; // 情况 2
        }
        // 情况 3: 左右孩子俱在
        TreeNode* succ = root->right;
        while (succ->left) succ = succ->left; // 寻找后继
        root->val = succ->val;                // 覆盖值
        root->right = deleteBST(root->right, succ->val); // 递归删除后继
    }
    return root;
}
```

---

<a id="c06-6-4"></a>
## 6.4 顺序倾斜与退化危机：为什么需要平衡树？

BST 的操作复杂度受制于树的高度 \(h\)。
- **最好情况**：节点分布均匀，树高 \(h = \lfloor \log_2 n \rfloor\)，所有操作均为 \(O(\log n)\)；
- **最坏情况（灾难）**：如果按照有序序列 `[1, 2, 3, 4, 5]` 依次插入，树将完全退化为单链表！树高 \(h = n\)，查找与插入退化为悲惨的 \(O(n)\)！

```text
插入有序数据: 1 -> 2 -> 3 -> 4 -> 5
1
 \
  2
   \
    3
     \
      4
       \
        5   <=== 逻辑上是树，物理上是单链表！
```

**工程启示**：基础 BST 无法抵御极端输入，必须引入**自动平衡机制**——这就引出了下一章的 AVL 树、红黑树与伸展树！

---

<a id="c06-6-5"></a>
## 6.5 BST 合法性验证的两种经典算法（中序遍历 vs 区间收缩）

### 算法一：中序遍历递增法
维护一个指针 `long long* prevVal`，中序遍历整棵树，若当前节点值 \(\le\) `prevVal`，则判定非法。

### 算法二：区间收缩法（自顶向下传递约束）
每个节点的值必须严格落在开区间 \((low, high)\) 之内。
- 进入左子树时，更新上界：\((low, root.\text{val})\)；
- 进入右子树时，更新下界：\((root.\text{val}, high)\)。

```cpp
bool validate(TreeNode* node, long long low, long long high) {
    if (!node) return true;
    if (node->val <= low || node->val >= high) return false;
    return validate(node->left, low, node->val) && 
           validate(node->right, node->val, high);
}
```

---

<a id="c06-6-6"></a>
## 6.6 进阶实战：顺序统计树（Order Statistic Tree）查第 k 小

普通的 BST 仅支持按键值查找。若想在 \(O(\log n)\) 内支持：
1. `select(k)`：查询树中第 \(k\) 小的元素；
2. `rank(x)`：查询键为 \(x\) 的元素在全树中的排名。

**增强设计**：在每个节点中增加字段 `int size`，表示**以该节点为根的子树所包含的总节点数**。
\[
node.\text{size} = (node.\text{left} ? node.\text{left}.\text{size} : 0) + (node.\text{right} ? node.\text{right}.\text{size} : 0) + 1
\]

```text
查找第 k 小的核心分治逻辑:
设左子树大小为 leftSize = node->left ? node->left->size : 0;
- 若 k == leftSize + 1: 当前节点恰好就是第 k 小!
- 若 k <= leftSize:     第 k 小必定落在左子树中，递归进左子树查找第 k 小;
- 若 k > leftSize + 1:  第 k 小落在右子树中，递归进右子树查找第 (k - leftSize - 1) 小!
```

---

<a id="c06-summary"></a>
## 本章小结

1. **BST 的灵魂在于中序单调性**：任意节点的整棵左子树都小于它，整棵右子树都大于它。
2. **后继替换化繁为简**：双孩子删除通过借调后继节点，将原本棘手的结构重构巧妙降维成单孩子删除。
3. **树高的致命脆弱性**：普通 BST 无法自愈倾斜退化，催生了平衡树家族的繁荣。
4. **数据结构增强技术**：通过维护局部统计量（如子树 `size`），可以让树形结构以对数时间承载高阶统计功能。

---

<a id="c06-exercises"></a>
## 思考题与经典大厂面试题

1. **BST 转双向链表**：如何不创建任何新节点，仅通过调整树中各节点的 `left`（作为 `prev`）和 `right`（作为 `next`）指针，将一棵 BST 就地（In-place）转换为一个有序的双向循环链表？
2. **BST 中两数之和**：给定一棵 BST 和一个目标和 \(K\)，如何利用类似双指针碰撞的思想，以 \(O(n)\) 时间复杂度、\(O(\log n)\) 空间复杂度判断树中是否存在两节点之和等于 \(K\)？
3. **退化树的重平衡**：如果一棵 BST 已经不幸退化成了链表，如何在 \(O(n)\) 时间、\(O(n)\) 空间内将其重构为一棵完全平衡的 BST？
