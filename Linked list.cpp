// ============================================================
//              双向链表（Doubly Linked List）
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

#include<iostream>
using namespace std;

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
    cout << "║   双 向 链 表 测 试              ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    Linkedlist ll;

    // 升序插入
    cout << "\n┌─ 升序插入 5,2,8,1,3 ──────────┐" << endl;
    ll.AscendingInsert(5);
    ll.AscendingInsert(2);
    ll.AscendingInsert(8);
    ll.AscendingInsert(1);
    ll.AscendingInsert(3);
    ll.Display();  // expect: 1 2 3 5 8
    cout << "节点数=" << ll.Count() << " (expect 5)" << endl;

    // 查找
    cout << "\n┌─ 查找测试 ────────────────────┐" << endl;
    cout << "Search(3)=" << ll.Search(3) << " (expect 1)" << endl;
    cout << "Search(9)=" << ll.Search(9) << " (expect 0)" << endl;

    // 删除
    cout << "\n┌─ 删除测试 ────────────────────┐" << endl;
    ll.Delete(1);  // 删头
    cout << "删除1后: "; ll.Display();
    ll.Delete(8);  // 删尾
    cout << "删除8后: "; ll.Display();
    ll.Delete(3);  // 删中间
    cout << "删除3后: "; ll.Display();
    cout << "节点数=" << ll.Count() << " (expect 2)" << endl;

    // 降序插入
    cout << "\n┌─ 降序插入 10,6 ───────────────┐" << endl;
    ll.DescendingInsert(10);
    ll.DescendingInsert(6);
    ll.Display();  // expect: 10 6 5 2

    cout << "\n所有测试完成！" << endl;
    return 0;
}
