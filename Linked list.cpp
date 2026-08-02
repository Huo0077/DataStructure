// ============================================================
//        链表合集：单向链表 + 双向链表
// ============================================================
//
// 本文件涵盖：
//   Part 1: 单向链表（Singly Linked List）—— 最简单的链表
//   Part 2: 双向链表（Doubly Linked List）—— 带前驱指针
//
// ============================================================

#include<iostream>
using namespace std;

// 【阅读地图】
//   先读 SinglyLinkedList 的 insertHead / remove / reverse，再读双向链表。
//   核心不变量：head 为 nullptr，或指向第一个节点；从 head 沿 next 能恰好访问每个节点一次。
//   每次改链都遵守“先保存仍要访问的指针，再重接链接，最后释放节点”的顺序。

// ============================================================
//   Part 1: 单向链表（Singly Linked List）
// ============================================================
//
// 【核心概念】
//   链表是最基础的非连续存储结构。每个节点只存数据 + 指向下一个的指针。
//   单向链表只能向后走，不能往回走。
//
//   head → [data|next] → [data|next] → [data|next] → nullptr
//
// 【节点定义】
//   data — 存储的数据（此处为 int）
//   next — 指向下一个节点的指针，尾节点 next = nullptr
//
// 【关键操作的时间复杂度】
//   头插  O(1) — 改一下 new node 的 next 指向，再更新 head
//   尾插  O(n) — 需要先遍历到尾部
//   删除  O(n) — 需要知道前驱节点（单向链表没有 prev 指针）
//   查找  O(n) — 需要从头遍历
//
// 【单向 vs 双向】
//   单向链表省空间（少一个指针），但删除时必须维护 prev 指针来找到前驱。
//   双向链表多一个 prev 指针，但删除和反向遍历更方便。
//
// 【为什么需要链表？】
//   数组：插入/删除中间元素需要搬移后续所有元素 → O(n)
//   链表：插入/删除只需改指针 → O(1)（如果已知位置）
//
//   本质区别：数组是连续存储（随机访问快），链表是离散存储（插入删除快）。
//
// ============================================================

class SinglyNode
{
public:
    int data;
    SinglyNode* next;
    SinglyNode(int d) : data(d), next(nullptr) {}
};

class SinglyLinkedList
{
private:
    SinglyNode* head;

public:
    SinglyLinkedList() : head(nullptr) {}
    SinglyLinkedList(int n) { head = new SinglyNode(n); }

    ~SinglyLinkedList()
    {
        while (head)
        {
            SinglyNode* tmp = head;
            head = head->next;
            delete tmp;
        }
    }

    // 头插法 O(1)：新节点放到链表最前面
    // 逆序插入 → 输出与插入顺序相反
    // 例：依次头插 1,2,3 → 输出 3 2 1
    void insertHead(int val)
    {
        SinglyNode* node = new SinglyNode(val);
        node->next = head;
        head = node;
    }

    // 尾插法 O(n)：新节点放到链表末尾
    // 顺序插入 → 输出与插入顺序一致
    void insertTail(int val)
    {
        SinglyNode* node = new SinglyNode(val);
        if (!head)
        {
            head = node;
            return;
        }
        SinglyNode* cur = head;
        while (cur->next)
            cur = cur->next;
        cur->next = node;
    }

    // 按值删除 O(n)：删除第一个值为 val 的节点
    // 关键：维护 prev 指针以修复链表
    void remove(int val)
    {
        if (!head) { cout << "list is empty" << endl; return; }

        // 删除的是头节点 → 直接推进 head
        if (head->data == val)
        {
            SinglyNode* tmp = head;
            head = head->next;
            delete tmp;
            return;
        }

        // 找前驱节点
        SinglyNode* cur = head;
        while (cur->next)
        {
            if (cur->next->data == val)
            {
                SinglyNode* tmp = cur->next;
                cur->next = cur->next->next;  // 跳过被删节点
                delete tmp;
                return;
            }
            cur = cur->next;
        }
        cout << "value not found" << endl;
    }

    // 查找 O(n)
    bool search(int val)
    {
        SinglyNode* cur = head;
        while (cur)
        {
            if (cur->data == val) return true;
            cur = cur->next;
        }
        return false;
    }

    // 统计节点数
    int count()
    {
        int n = 0;
        SinglyNode* cur = head;
        while (cur) { n++; cur = cur->next; }
        return n;
    }

    // 反转链表（迭代法）
    // 三个指针：prev/curr/next，遍历时逐个逆转 next 方向
    // 例：1→2→3  →  3→2→1
    void reverse()
    {
        SinglyNode *prev = nullptr, *curr = head, *next = nullptr;
        while (curr)
        {
            next = curr->next;   // 暂存后继
            curr->next = prev;   // 反转指向
            prev = curr;         // prev 前进
            curr = next;         // curr 前进
        }
        head = prev;  // 原尾节点变新头
    }

    void display()
    {
        SinglyNode* cur = head;
        while (cur)
        {
            cout << cur->data << " ";
            cur = cur->next;
        }
        cout << endl;
    }
};

// ============================================================
//   Part 2: 双向链表（Doubly Linked List）
// ============================================================
//
// 【核心概念】
//   每个节点有两个指针：prev（前驱）和 next（后继）。
//   相比单向链表，多了一倍的指针开销，但换来了以下好处：
//     - 可以在 O(1) 时间删除任意节点（已知节点指针时）
//     - 可以双向遍历
//     - 删除操作不需要额外的前驱指针遍历
//
// 【头节点 first】
//   first 指向链表的第一个节点，first->prev 始终为 nullptr。
//   first == nullptr 表示空链表。
//
// 【支持的操作】
//   Count()          — 统计节点个数，O(n)
//   AscendingInsert  — 升序插入，O(n)
//   DescendingInsert — 降序插入，O(n)
//   Insert           — 尾插法，O(n)（可通过维护尾指针优化到 O(1)）
//   Delete           — 按值删除第一个匹配节点，O(n)
//   Search           — 按值查找，O(n)
//   Display          — 正向遍历打印，O(n)
//
// 【与单向链表的比较】
//   单向链表：Delete 需要额外的前驱指针 + 从头遍历
//   双向链表：Delete 直接用 current->prev 即可找到前驱
//
//   AscendingInsert/DescendingInsert 利用了双向链表的 prev 指针，
//   在找到插入位置后可以方便地在当前节点之前插入。
//
// ============================================================

class Node
{
public:
    int data;
    Node* next;
    Node* prev;
    Node(int d) : data(d), next(nullptr), prev(nullptr) {}
};

class Linkedlist
{
private:
    Node* first;  // 头节点指针，空链表时为 nullptr

public:
    Linkedlist() : first(nullptr) {}

    // 创建一个包含初始值的链表
    Linkedlist(int n) { first = new Node(n); }

    // 析构：遍历释放所有节点
    ~Linkedlist()
    {
        Node* current = first;
        while (current)
        {
            Node* tmp = current;
            current = current->next;
            delete tmp;
        }
    }

    // 统计链表中的节点个数
    int Count()
    {
        int result = 0;
        Node* current = first;
        while (current)
        {
            result++;
            current = current->next;
        }
        return result;
    }

    // 升序插入：遍历找到第一个比 val 大的节点，插在它前面
    // 如果所有节点都 ≤ val，则插入到末尾
    void AscendingInsert(int val)
    {
        Node* newNode = new Node(val);
        if (!first)
        {
            first = newNode;
            return;
        }
        Node* current = first;
        while (current)
        {
            if (current->data <= val)
            {
                if (current->next)
                {
                    current = current->next;  // 继续找更大的
                }
                else
                {
                    // 已到链表尾部，val 是当前最大值，插入末尾
                    current->next = newNode;
                    newNode->prev = current;
                    return;
                }
            }
            else
            {
                // current->data > val，在 current 之前插入
                Node* pre = current->prev;
                newNode->next = current;
                newNode->prev = pre;
                current->prev = newNode;
                if (pre)
                {
                    pre->next = newNode;
                }
                else
                {
                    first = newNode;  // 插入到头节点之前，更新 first
                }
                return;
            }
        }
    }

    // 降序插入：遍历找到第一个比 val 小的节点，插在它前面
    // 逻辑与升序插入对称，只是比较方向相反
    void DescendingInsert(int val)
    {
        Node* newNode = new Node(val);
        if (!first)
        {
            first = newNode;
            return;
        }
        Node* current = first;
        while (current)
        {
            if (current->data >= val)
            {
                if (current->next)
                {
                    current = current->next;
                }
                else
                {
                    current->next = newNode;
                    newNode->prev = current;
                    return;
                }
            }
            else
            {
                Node* pre = current->prev;
                newNode->next = current;
                newNode->prev = pre;
                current->prev = newNode;
                if (pre)
                {
                    pre->next = newNode;
                }
                else
                {
                    first = newNode;
                }
                return;
            }
        }
    }

    // 尾插法：遍历到链表末尾，在尾部插入新节点
    // 时间复杂度 O(n)，可维护 tail 指针优化到 O(1)
    void Insert(int val)
    {
        Node* newnode = new Node(val);
        if (!first)
        {
            first = newnode;
            return;
        }
        Node* current = first;
        while (current)
        {
            if (current->next)
            {
                current = current->next;
            }
            else
            {
                current->next = newnode;
                newnode->prev = current;
                break;
            }
        }
    }

    // 按值删除：找到第一个值为 val 的节点并删除
    void Delete(int val)
    {
        if (!first)
        {
            cout << "list is empty" << endl;
            return;
        }
        Node* current = first;
        while (current)
        {
            if (current->data == val)
            {
                // 调整前驱的 next 指针
                if (current->prev)
                {
                    current->prev->next = current->next;
                }
                else
                {
                    first = current->next;  // 删除的是头节点
                }
                // 调整后继的 prev 指针
                if (current->next)
                {
                    current->next->prev = current->prev;
                }
                delete current;
                return;
            }
            current = current->next;
        }
        cout << "value not found" << endl;
    }

    // 查找值为 val 的节点是否存在
    bool Search(int val)
    {
        Node* current = first;
        while (current)
        {
            if (current->data == val)
            {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    // 正向遍历打印
    void Display()
    {
        Node* current = first;
        while (current)
        {
            cout << current->data << " ";
            current = current->next;
        }
        cout << endl;
    }
};

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║    链 表 综 合 测 试            ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    // ==================== Part 1: 单向链表 ====================
    cout << "\n===== Part 1: 单向链表 =====" << endl;

    SinglyLinkedList sl;
    cout << "\n头插 1,2,3 (逆序): ";
    sl.insertHead(1);
    sl.insertHead(2);
    sl.insertHead(3);
    sl.display();  // 3 2 1
    cout << "节点数=" << sl.count() << " (expect 3)" << endl;

    cout << "尾插 4,5: ";
    sl.insertTail(4);
    sl.insertTail(5);
    sl.display();  // 3 2 1 4 5

    cout << "search(1)=" << sl.search(1) << " (expect 1)" << endl;
    cout << "search(9)=" << sl.search(9) << " (expect 0)" << endl;

    cout << "反转: ";
    sl.reverse();
    sl.display();  // 5 4 1 2 3

    cout << "删除4: ";
    sl.remove(4);
    sl.display();  // 5 1 2 3

    cout << "删除头5: ";
    sl.remove(5);
    sl.display();  // 1 2 3

    // ==================== Part 2: 双向链表 ====================
    cout << "\n===== Part 2: 双向链表 =====" << endl;

    Linkedlist ll;

    cout << "\n升序插入 5,2,8,1,3: ";
    ll.AscendingInsert(5);
    ll.AscendingInsert(2);
    ll.AscendingInsert(8);
    ll.AscendingInsert(1);
    ll.AscendingInsert(3);
    ll.Display();  // 1 2 3 5 8
    cout << "节点数=" << ll.Count() << " (expect 5)" << endl;

    cout << "search(3)=" << ll.Search(3) << " (expect 1)" << endl;
    cout << "search(9)=" << ll.Search(9) << " (expect 0)" << endl;

    cout << "删除1(头): "; ll.Delete(1); ll.Display();
    cout << "删除8(尾): "; ll.Delete(8); ll.Display();
    cout << "删除3(中): "; ll.Delete(3); ll.Display();
    cout << "节点数=" << ll.Count() << " (expect 2)" << endl;

    cout << "降序插入10,6: ";
    ll.DescendingInsert(10);
    ll.DescendingInsert(6);
    ll.Display();  // 10 6 5 2

    cout << "\n所有测试完成！" << endl;
    return 0;
}
