// ============================================================
//                    插入排序（Insertion Sort）
// ============================================================
//
// 【核心思想】
//   将数组分为"已排序"和"未排序"两部分。每次从未排序部分取第一个
//   元素，在已排序部分中找到它的正确位置并插入。就像打牌时整理
//   手牌——拿到新牌后，从右向左找它应该在的位置，然后插进去。
//
// 【算法步骤】
//   1. 从第 2 个元素（索引 1）开始，认为索引 0 已经有序
//   2. 取出当前元素 key = arr[i]
//   3. 从 i-1 向左扫描，将比 key 大的元素都右移一格
//   4. 将 key 放入空出的位置
//
// 【时间复杂度】
//   最好（已有序）: O(n)     —— 内层 while 从不执行
//   最坏（逆序）:   O(n²)    —— 每次都要移到最前面
//   平均:          O(n²)
//
// 【稳定性】稳定 —— 相等元素不会交换位置
//
// 【适用场景】
//   - 数据量小（n < 50）时比快排/归并更快（常数因子小）
//   - 数据基本有序时几乎是 O(n)
//   - 实际中常用于混合排序（如 TimSort 对小数组用插入排序）
//
// 【与冒泡/选择排序的区别】
//   冒泡：两两比较交换，每轮把最大元素"浮"到最后
//   选择：每轮选最小元素放到前面
//   插入：把当前元素插入前面已排序部分的正确位置
//   三者都是 O(n²)，但插入排序通常更快（交换次数更少）
// ============================================================

#include<iostream>
#include<vector>
#include<algorithm>
#include<utility>
#include<queue>
using namespace std;

// 【阅读地图】
//   不要按文件顺序背诵 20 多种排序。建议按“局部有序、分治、堆、键域、外部排序”分组比较。
//   阅读每个算法时都记录四件事：循环不变量、稳定性、额外空间、触发最坏情况的输入。
//   稳定性只在相等键的相对次序上定义；快速、堆和通常的选择排序不稳定。

void insertion_sort(vector<int>& arr)
{
    int n = arr.size();
    // 从第二个元素开始，左边 arr[0..i-1] 已有序，右边 arr[i..n-1] 待处理
    for (int i = 1; i < n; i++)
    {
        int key = arr[i];     // 当前待插入的元素（"新牌"）
        int j = i - 1;
        // 从右向左扫描已排序部分，将比 key 大的元素逐个右移
        while (j >= 0 && arr[j] > key)
        {
            arr[j + 1] = arr[j];  // 右移，给 key 腾位置
            j--;
        }
        arr[j + 1] = key;         // 插入到正确位置
    }
}


// ============================================================
//              原始冒泡排序（Bubble Sort）—— 升序
// ============================================================
//
// 【核心思想】
//   每轮从左到右比较相邻元素，如果"前 > 后"就交换，
//   一轮结束后最大元素就像气泡一样"浮"到了最右端。
//   n-1 轮后数组升序排列。
//
// 【痛点】
//   即使数组已经有序，原始冒泡仍然傻傻跑完所有 n(n-1)/2 次比较。
//   最好情况也是 O(n²)，毫无"感知有序"的能力。
//
// ============================================================
void bubble_sort(vector<int>& arr)
{
    int n = arr.size();
    for (int i = 0; i < n; i++)
    {
        // 每轮最后 i 个元素已经排好，缩小右边界
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])         // 前 > 后 → 交换（升序）
            {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// ============================================================
//       方案一：提前停机 —— 布尔标志位（Flagged Bubble Sort）
// ============================================================
//
// 【优化思路】
//   如果某轮遍历中没有任何元素交换位置，说明数组已经完全有序。
//   此时直接 break 退出所有循环，后面的轮次全都可以省掉。
//   就像排队时老师看了一眼说"已经整齐了"，就不需要重新排了。
//
// 【物理直觉】
//   问一句"这一趟有没有人换过座位？"
//   没有 → 已经有序 → 收工！
//
// 【效果】
//   最好情况（已有序）：第 1 轮跑一遍发现无需交换 → 直接退出 → O(n)
//   最坏情况（完全逆序）：仍然 O(n²)，但每轮都要交换到底
//
// 【实现细节】
//   swapped 在每轮开始时设为 false，发生交换时置为 true，
//   轮末检查 swapped，若为 false 则提前终止。
//
// ============================================================
void bubble_sort_v1_flagged(vector<int>& arr)
{
    int n = arr.size();
    for (int i = 0; i < n; i++)
    {
        bool swapped = false;  // 本轮是否发生过交换
        //   ↑ 每轮开始时，假设"已经有序"

        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr[j], arr[j + 1]);
                swapped = true;  // 发生了交换 → 说明还不完全有序
            }
        }

        // 如果整轮都没有交换 → 已完全有序，后面的轮次直接跳过
        if (!swapped)
        {
            break;
        }
    }
}

// ============================================================
//    方案二：缩小边界 —— 记录最后一次交换位置
//    （Right-Bound Shrinking Bubble Sort）
// ============================================================
//
// 【优化思路】
//   方案一的 swapped 只回答"有没有交换"（是/否），但更精细的问题是：
//   "最后一次交换发生在哪个位置？"
//
//   这个位置之后的所有元素，必然已经有序！因为它们之间再也没
//   发生过交换——说明从该位置到数组末尾，所有相邻元素都已满足
//   前 ≤ 后的升序关系。
//
//   因此，下一次循环的右边界可以直接收缩到"最后一次交换的位置"，
//   而非固定的 n - i - 1。
//
// 【物理直觉】
//   在一次巡视中，最后一次发生交换的位置就是"混乱区"与"有序区"
//   的分界线。边界右侧 → 绝对有序，以后都不用看了。
//
// 【效果】
//   当数组后半部分已经有序时，大幅减少无效比较次数。
//   例如：[3,1,4,5,6,7,8] → 第一次遍历到最后交换位置 = 1，
//   下一轮右边界直接缩到 1，后面 [4,5,6,7,8] 全部跳过。
//
// 【与方案一的区别】
//   方案一：只能判断"全有序 / 不全有序"
//   方案二：能精确定位"有序开始的位置"
//   方案二包含方案一的逻辑——若 lastSwapPos = 0（没交换过），自动退出
//
// ============================================================
void bubble_sort_v2_bound(vector<int>& arr)
{
    int n = arr.size();
    int rightBound = n - 1;       // 初始右边界 = 数组末尾

    for (int i = 0; i < n; i++)
    {
        int lastSwapPos = 0;      // 本轮最后一次交换的位置
        //                    ↑ 初始化为 0，若保持 0 说明没交换过

        for (int j = 0; j < rightBound; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr[j], arr[j + 1]);
                lastSwapPos = j;  // 记录最后一次交换的位置
                //              ↑ 这个位置之后已经有序了！
            }
        }

        // 下一轮的右边界直接收缩到最后发生交换的位置
        rightBound = lastSwapPos;

        // 如果右边界为 0 → 没有发生任何交换 → 全部有序
        if (rightBound == 0)
        {
            break;
        }
    }
}

// ============================================================
//         方案三：双向巡视 —— 鸡尾酒排序
//    （Cocktail Shaker Sort / Bidirectional Bubble Sort）
// ============================================================
//
// 【优化思路】
//   传统冒泡有一个严重的不对称问题——"兔子与乌龟"：
//
//   - 大元素（兔子）：向右移动极快，一趟就能从最左跑到最右
//   - 小元素（乌龟）：向左移动极慢，每次只能挪 1 格！
//
//   例如 [9,2,3,4,5,6,1] 升序排列：
//     第一趟从左→右：9 像兔子一样跳到最右边 → [2,3,4,5,6,1,9]
//     第二趟从左→右：6 跳到 1 右边 → [2,3,4,5,1,6,9]
//     ... 可怜的 1 每次只能向左爬 1 格，需要 n-2 趟才能到最左！
//
//   解决方案：别只往一个方向走，来回扫！
//     奇数趟：从左→右，把最大值推到右边
//     偶数趟：从右→左，把最小值拉到左边
//     来回震荡，像调酒师摇雪克杯一样，乌龟和兔子都能照顾到。
//
// 【效果】
//   完美解决"乌龟"数据拖慢的问题，外层总趟数大幅减少。
//   例如上面的例子，第二次从右→左时 1 直接跳到最左边：
//     [2,3,4,5,1,6,9] 右→左 → [1,2,3,4,5,6,9] 两趟搞定！
//
// ============================================================
void bubble_sort_v3_cocktail(vector<int>& arr)
{
    int n = arr.size();
    int left = 0;          // 左边界
    int right = n - 1;     // 右边界

    while (left < right)
    {
        // -------- 第一趟：从左 → 右（把最大值推到右边）--------
        int lastSwapRight = left;  // 初始化为 left，表示"本轮未发生交换"
        for (int j = left; j < right; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr[j], arr[j + 1]);
                lastSwapRight = j;  // 记录最后一次交换的位置
            }
        }
        // 若没有发生交换 → 区间内已全部有序
        if (lastSwapRight == left)
        {
            break;
        }
        right = lastSwapRight;   // 右边界收缩到最后交换位置

        // -------- 第二趟：从右 → 左（把最小值拉到左边）--------
        int lastSwapLeft = right;  // 初始化为 right，表示"本轮未发生交换"
        for (int j = right; j > left; j--)
        {
            if (arr[j - 1] > arr[j])
            {
                swap(arr[j - 1], arr[j]); // 较小值向左移动
                lastSwapLeft = j;          // 记录最后一次交换的位置
            }
        }
        // 若没有发生交换 → 区间内已全部有序
        if (lastSwapLeft == right)
        {
            break;
        }
        left = lastSwapLeft;     // 左边界收缩到最后交换位置
    }
}

// ============================================================
//    方案四：终极完全体 —— 双向冒泡 + 双向边界收缩
//    （Optimized Cocktail Sort with Dual Boundary Shrinking）
// ============================================================
//
// 【优化思路】
//   方案三虽然来回走，但左边界收缩还不够精细——从右往左走时，
//   也应当精确记录"最后一次交换的位置"来更新左边界。
//
//   方案四 = 方案二 + 方案三 的完全融合：
//     - 从左→右：记录 lastSwapRight → 更新右边界
//     - 从右→左：记录 lastSwapLeft  → 更新左边界
//     左右两个边界像钳子一样，从两端向中间"混乱区"迅速收缩。
//
// 【物理直觉】
//   两个边界像两把手术钳，精准地夹住无序区域：
//     左钳子（leftBound）： 往右收缩 → 左边已经有序
//     右钳子（rightBound）：往左收缩 → 右边已经有序
//     中间是唯一的"需要处理的无序区"
//   每轮两把钳子同时向中间逼近，直到夹紧（left ≥ right）。
//
// 【效果】
//   这是冒泡排序在 O(n²) 家族中能达到的性能巅峰。
//   - 同时解决"兔子与乌龟"的不对称问题
//   - 两边边界都做精准收缩，避免一切无效比较
//   - 对于大部分已有序的数组，复杂度接近 O(n)
//
// 【复杂度】
//   最好（已有序）：双向各走一次就收工 → O(n)
//   最坏（逆序）：仍然 O(n²)，但常数因子比原始冒泡小得多
//
// ============================================================
void bubble_sort_v4_ultimate(vector<int>& arr)
{
    int n = arr.size();
    int leftBound = 0;              // 左边界（已排序区）
    int rightBound = n - 1;         // 右边界（已排序区）

    while (leftBound < rightBound)
    {
        // 本轮记录的最后交换位置
        // 初始化为各自的边界值，表示"本轮未发生任何交换"
        int lastSwapRight = leftBound;   // 左→右：未交换则保持 leftBound
        int lastSwapLeft = rightBound;   // 右→左：未交换则保持 rightBound

        // -------- 第一趟：从左 → 右（把最大值推到右边）--------
        for (int j = leftBound; j < rightBound; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr[j], arr[j + 1]);
                lastSwapRight = j;  // 记录本轮最后的交换位置
            }
        }

        // 如果 lastSwapRight 没变 → 没有发生任何交换 → 区间已有序！
        if (lastSwapRight == leftBound)
        {
            break;
        }

        // 右边界收缩到本轮最后一次发生交换的位置
        rightBound = lastSwapRight;

        // -------- 第二趟：从右 → 左（把最小值拉到左边）--------
        for (int j = rightBound; j > leftBound; j--)
        {
            if (arr[j - 1] > arr[j])          // 左 > 右 → 交换
            {
                swap(arr[j - 1], arr[j]);     // 较小值向左移动
                lastSwapLeft = j;             // 记录本轮最后的交换位置
            }
        }

        // 如果 lastSwapLeft 没变 → 没有发生任何交换 → 区间已有序！
        if (lastSwapLeft == rightBound)
        {
            break;
        }

        // 左边界收缩到本轮最后一次发生交换的位置
        leftBound = lastSwapLeft;
    }
}

// ============================================================
//
//                 快速排序（Quick Sort）系列
//
// ============================================================
//
// 【核心思想 —— 分治法（Divide and Conquer）】
//   1. 选一个"基准"（Pivot / 老大）
//   2. 分区（Partition）：小于基准的放左边，大于基准的放右边
//   3. 对左右两个子数组递归进行同样操作
//   4. 递归到底（只剩 1 个元素）时，整个数组就有序了
//
//   核心公式：QuickSort(arr) = Partition + QuickSort(左) + QuickSort(右)
//
// 【基础版快排的致命弱点】
//   如果总是选最右（或最左）元素当 pivot：
//     - 已有序数组 → 每次都分出 0:n-1 → 退化为 O(n²)
//     - 完全逆序数组 → 同样退化
//   下面三个优化方案逐一攻破这些弱点。
//
// 【复杂度】
//   基础版：平均 O(n log n)，最坏 O(n²)
//   优化后：平均 O(n log n)，最坏几乎不会出现
//   空间：递归栈 O(log n)（优化后）
//
// ============================================================

// ----------------------------------------------------------
// 区间插入排序辅助函数：对 arr[left..right] 做插入排序
// 用于方案二的小区间切换
// ----------------------------------------------------------
void insertionSortRange(vector<int>& arr, int left, int right)
{
    for (int i = left + 1; i <= right; i++)
    {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key)
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// ============================================================
//              基础版快速排序（Basic Quick Sort）
// ============================================================
//
// 最简单的实现：总是选 arr[right] 作为 pivot，然后双向扫描分区。
//
// 【分区流程（Hoare 变体的简化版）】
//   pivot = arr[right]
//   i 从左边找 ≥ pivot 的元素
//   j 从右边找 ≤ pivot 的元素
//   找到后交换，i 和 j 交错时停止
//
// 【痛点演示】arr = [1,2,3,4,5,6,7,8]（已有序）
//   选 arr[7]=8 为 pivot：
//     分区结果：左边 [1,2,3,4,5,6,7]，右边 []（空！）
//     递归左 7 个，右 0 个 → 极度不平衡 → O(n²)
//
// ============================================================
int basicPartition(vector<int>& arr, int left, int right)
{
    int pivot = arr[right];              // 选最右为基准
    int i = left;                        // i：从左扫描，找 ≥ pivot 的

    for (int j = left; j < right; j++)   // j：从左扫描到 right-1
    {
        if (arr[j] < pivot)              // 比基准小 → 换到左边去
        {
            swap(arr[i], arr[j]);
            i++;                         // i 左边全是 < pivot 的
        }
    }
    swap(arr[i], arr[right]);            // 把 pivot 放到正确位置
    return i;                            // 返回 pivot 的最终位置
}

void quickSort_basic(vector<int>& arr, int left, int right)
{
    if (left >= right)
    {
        return;
    }
    int pivotIndex = basicPartition(arr, left, right);
    quickSort_basic(arr, left, pivotIndex - 1);   // 递归左半
    quickSort_basic(arr, pivotIndex + 1, right);  // 递归右半
}

void quickSort_basic(vector<int>& arr)
{
    quickSort_basic(arr, 0, arr.size() - 1);
}


// ============================================================
//      方案一：三数取中法（Median-of-Three Pivot Selection）
// ============================================================
//
// 【优化思路】
//   不再"死板地选最右边当老大"，而是取三个位置的元素：
//     arr[left]（最左）、arr[mid]（中间）、arr[right]（最右）
//   选它们三个的中位数（不最大也不最小的那个）作为 pivot。
//
// 【为什么选中位数？】
//   中位数保证：左边至少有一部分比它小，右边至少有一部分比它大。
//   这样就几乎不会出现"一边全空"的极端情况。
//
// 【物理直觉】
//   从队伍的最左边、中间、最右边各拉一个人出来比身高，
//   选中间身高的当队长（pivot），然后把它换到最右边去。
//   这样选出的队长大概率是"中等水平"，不会太极端。
//
// 【效果】
//   已有序数组：选中间值 → 左右各约一半 → O(n log n) ✓
//   对随机数组也有轻微提升（避免偶尔选到极值）
//
// ============================================================

// 三数取中：比较 left, mid, right 三个位置，把中位数换到 arr[right]
// 这样后续可以直接用标准 Lomuto partition（pivot 始终在 arr[right]）
void medianOfThree(vector<int>& arr, int left, int right)
{
    int mid = left + (right - left) / 2;

    // 三次比较后，arr[mid] 的值就是 left/mid/right 三者的中位数
    if (arr[left] > arr[mid])    swap(arr[left], arr[mid]);
    if (arr[left] > arr[right])  swap(arr[left], arr[right]);
    if (arr[mid]  > arr[right])  swap(arr[mid],  arr[right]);

    // 把中位数换到 arr[right]，作为后续 partition 的 pivot
    swap(arr[mid], arr[right]);
}

int partition_v1_median(vector<int>& arr, int left, int right)
{
    medianOfThree(arr, left, right);  // 中位数已放到 arr[right]
    int pivot = arr[right];

    int i = left;
    for (int j = left; j < right; j++)
    {
        if (arr[j] < pivot)
        {
            swap(arr[i], arr[j]);
            i++;
        }
    }
    swap(arr[i], arr[right]);  // pivot 归位
    return i;
}

void quickSort_v1_median(vector<int>& arr, int left, int right)
{
    if (left >= right)
    {
        return;
    }
    int pi = partition_v1_median(arr, left, right);
    quickSort_v1_median(arr, left, pi - 1);
    quickSort_v1_median(arr, pi + 1, right);
}

void quickSort_v1_median(vector<int>& arr)
{
    quickSort_v1_median(arr, 0, arr.size() - 1);
}


// ============================================================
//    方案二：小区间切换插入排序（Insertion Sort Cutoff）
// ============================================================
//
// 【优化思路】
//   快排递归到底层时，子数组非常小（比如 10 个元素以内）。
//   此时函数调用的开销（压栈/弹栈）反而比排序本身还大！
//   而且插入排序对"基本有序"的小数组极快（常数因子小）。
//
// 【物理直觉】
//   大部队用快排（分治效率高），
//   小队（≤10人）直接用插入排序（简单高效），
//   杀鸡不用牛刀！
//
//   "递归树底层有大量叶子节点，砍掉它们能省一半开销。"
//
// 【效果】
//   砍掉递归树底层的大量节点，整体性能提升约 10%~15%。
//   经验阈值通常在 5~20 之间，这里选 16。
//
// ============================================================

const int INSERTION_CUTOFF = 16;  // 小数组切换阈值

int partition_v2_cutoff(vector<int>& arr, int left, int right)
{
    int pivot = arr[right];
    int i = left;
    for (int j = left; j < right; j++)
    {
        if (arr[j] < pivot)
        {
            swap(arr[i], arr[j]);
            i++;
        }
    }
    swap(arr[i], arr[right]);
    return i;
}

void quickSort_v2_cutoff(vector<int>& arr, int left, int right)
{
    // 小区间切换：如果元素数 ≤ 阈值，直接用插入排序
    if (right - left + 1 <= INSERTION_CUTOFF)
    {
        insertionSortRange(arr, left, right);
        return;
    }

    int pi = partition_v2_cutoff(arr, left, right);
    quickSort_v2_cutoff(arr, left, pi - 1);
    quickSort_v2_cutoff(arr, pi + 1, right);
}

void quickSort_v2_cutoff(vector<int>& arr)
{
    quickSort_v2_cutoff(arr, 0, arr.size() - 1);
}


// ============================================================
//   方案三：三路划分 —— 荷兰国旗问题
//   (3-Way Partitioning / Dutch National Flag Problem)
// ============================================================
//
// 【优化思路】
//   基础快排只分"小于 pivot"和"大于 pivot"两拨。
//   如果数组中有大量重复元素（如 [4,2,4,4,1,4,4,7]），
//   等于 pivot 的元素会被反复搬来搬去，白白浪费时间。
//
//   三路划分把数组分成三拨：
//     左区：< pivot   （lt 标记右边界）
//     中区：== pivot  （全部聚在中间，不需要再递归！）
//     右区：> pivot   （gt 标记左边界）
//
//   下一次递归时，只需要处理"左区"和"右区"，中间庞大的一坨
//   直接跳过，对于重复元素多的场景速度飞升！
//
// 【为什么叫"荷兰国旗问题"？】
//   荷兰国旗由红、白、蓝三色条纹组成。
//   图灵奖得主 Dijkstra 提出这个算法，
//   把数组分成三块（小/等/大）→ 就像荷兰国旗的三色条纹。
//   只用一次遍历 + 三个指针就能完成，不需额外数组。
//
// 【三指针详解】
//   lt —— "小于区"的右边界（less than）
//        arr[left .. lt-1] 全是 < pivot 的元素
//
//   gt —— "大于区"的左边界（greater than）
//        arr[gt+1 .. right] 全是 > pivot 的元素
//
//   i  —— 侦察兵 / 扫描指针
//        arr[lt .. i-1] 全是 == pivot 的元素
//        arr[i .. gt] 是待扫描区域
//
// 【算法流程（一次遍历）】
//   while (i <= gt)  // 侦察兵还没撞上大于区边界
//   {
//       if arr[i] < pivot:
//           swap(arr[lt], arr[i]);  // 塞到小于区末尾
//           lt++; i++;              // 小于区扩张，侦察兵前进
//
//       else if arr[i] > pivot:
//           swap(arr[gt], arr[i]);  // 丢到大于区开头
//           gt--;                   // 大于区扩张，i 不动！
//           //    ↑ i 不动是因为换过来的新元素还没检查过
//
//       else:  // arr[i] == pivot
//           i++;  // 等于区自然扩张，侦察兵继续向前
//   }
//
// 【为什么 swap(arr[gt], arr[i]) 后 i 不能前进？】
//   换过来的 arr[gt] 是"未扫描区"的元素，它的值是未知的！
//   需要下一轮循环对新的 arr[i] 重新判断。
//   而 swap(arr[lt], arr[i]) 后 i 可以前进，因为换过来的一定是
//   等于区里的元素（已经在之前被 i 扫描并判定为 == pivot 了）。
//
// 【效果】
//   - 所有等于 pivot 的元素一次性归位，后续不参与递归
//   - 大量重复元素的数组：接近 O(n)
//   - 例如 [4,2,4,4,1,4,4,7] 选 pivot=4：
//     三路划分后 → 左区[2,1] 中区[4,4,4,4,4] 右区[7]
//     只需递归 [2,1] 和 [7]！
//
// ============================================================

// 三路划分：返回 pair{lt, gt}，区间 [lt, gt] 内都是 == pivot
pair<int, int> threeWayPartition(vector<int>& arr, int left, int right)
{
    int pivot = arr[right];  // 基准值
    int lt = left;           // 小于区的右边界
    int gt = right;          // 大于区的左边界
    int i = left;            // 侦察兵

    while (i <= gt)  // 侦察兵没撞上大于区就继续
    {
        if (arr[i] < pivot)
        {
            swap(arr[lt], arr[i]);  // 塞到小于区
            lt++;                   // 小于区扩张
            i++;                    // 侦察兵前进
        }
        else if (arr[i] > pivot)
        {
            swap(arr[gt], arr[i]);  // 丢到大于区
            gt--;                   // 大于区扩张，i 不动！
        }
        else  // arr[i] == pivot
        {
            i++;  // 等于区自动扩张
        }
    }
    return {lt, gt};  // [lt, gt] = 等于区间，不需要再排序
}

void quickSort_v3_threeway(vector<int>& arr, int left, int right)
{
    if (left >= right)
    {
        return;
    }

    pair<int, int> p = threeWayPartition(arr, left, right);
    int lt = p.first;
    int gt = p.second;

    // 只需要递归小于区和大于区，等于区 [lt, gt] 已经全在正确位置
    quickSort_v3_threeway(arr, left, lt - 1);
    quickSort_v3_threeway(arr, gt + 1, right);
}

void quickSort_v3_threeway(vector<int>& arr)
{
    quickSort_v3_threeway(arr, 0, arr.size() - 1);
}


// ============================================================
//       方案四：终极完全体 —— 三合一优化快排
//       （Ultimate Quick Sort: 三数取中 + 小区间切换 + 三路划分）
// ============================================================
//
// 【优化思路】
//   集大成者！把三种优化全部融入：
//     1. 三数取中选 pivot → 永久避免 O(n²) 退化
//     2. 小区间切换插入排序 → 砍掉递归底层海量开销
//     3. 三路划分 → 重复元素直接聚中跳过
//
// 【效果】
//   这是通用排序算法在工程实践中的巅峰形态！
//   事实上，Java 的 Arrays.sort()（基本类型）、C++ 的
//   std::sort()（introsort）都综合了这些优化思想。
//
//   - 随机数组：     O(n log n)，常数因子极小
//   - 已有序数组：   O(n)（三数取中 + 小区间插入排序）
//   - 大量重复元素： 接近 O(n)（三路划分跳过重复）
//   - 任何数据分布：  稳定高效，没有明显弱点
//
// ============================================================

void quickSort_v4_ultimate(vector<int>& arr, int left, int right)
{
    // -------- 优化 1：小区间切换插入排序 --------
    if (right - left + 1 <= INSERTION_CUTOFF)
    {
        insertionSortRange(arr, left, right);
        return;
    }

    // -------- 优化 2：三数取中选 pivot --------
    medianOfThree(arr, left, right);  // 中位数已放到 arr[right]
    int pivot = arr[right];

    // -------- 优化 3：三路划分 --------
    int lt = left;
    int gt = right;
    int i = left;

    while (i <= gt)
    {
        if (arr[i] < pivot)
        {
            swap(arr[lt], arr[i]);
            lt++;
            i++;
        }
        else if (arr[i] > pivot)
        {
            swap(arr[gt], arr[i]);
            gt--;
        }
        else
        {
            i++;
        }
    }

    // 中间 [lt, gt] 全等于 pivot，跳过！
    quickSort_v4_ultimate(arr, left, lt - 1);
    quickSort_v4_ultimate(arr, gt + 1, right);
}

void quickSort_v4_ultimate(vector<int>& arr)
{
    quickSort_v4_ultimate(arr, 0, arr.size() - 1);
}

// ============================================================
//
//                 归并排序（Merge Sort）系列
//
// ============================================================
//
// 【核心思想 —— 分治法（Divide and Conquer）】
//   1. 分（Divide）：把数组从中间切成两半
//   2. 治（Conquer）：递归地对左右两半分别排序
//   3. 合（Combine）：把两个有序子数组合并成一个有序数组
//
//   核心公式：MergeSort(arr) = Merge(MergeSort(左), MergeSort(右))
//
//   与快排的分治对比：
//     快排：分在"治"之前（先分区再递归）→ 自顶向下
//     归并：分为先（先切半递归），合才是核心 → 自底向上回溯
//
// 【归并排序的优势】
//   1. 稳定排序：相等元素保持原始顺序（快排不稳定！）
//   2. 保证 O(n log n)：不存在退化到 O(n²) 的情况
//   3. 适合链表排序：链表合并不需要额外空间
//   4. 适合外部排序：处理海量数据（磁盘文件排序）
//
// 【归并排序的劣势】
//   1. 需要 O(n) 额外空间（临时数组）
//   2. 递归版本有栈开销
//
// 【合并过程（Merge）详解 —— 双指针法】
//
//   左右两半都已有序，用两个指针 i、j 分别指向两半的起始位置：
//
//     left:  [2, 5, 8]     right: [1, 3, 9]
//             ↑                     ↑
//             i                     j
//
//   比较 arr[i] 和 arr[j]，较小的放入临时数组，对应指针前进：
//
//     步骤1: 2>1 → 放 1, j++    temp=[1]            i→2, j→3
//     步骤2: 2<3 → 放 2, i++    temp=[1,2]          i→5, j→3
//     步骤3: 5>3 → 放 3, j++    temp=[1,2,3]        i→5, j→9
//     步骤4: 5<9 → 放 5, i++    temp=[1,2,3,5]      i→8, j→9
//     步骤5: 8<9 → 放 8, i++    temp=[1,2,3,5,8]    i 越界
//     步骤6: i 越界，把右半剩余 [9] 全部追加
//           temp=[1,2,3,5,8,9]
//
//   最后把 temp 拷回原数组。O(n) 时间，O(n) 空间。
//
// 【复杂度】
//   时间：T(n) = 2·T(n/2) + O(n) → O(n log n)，所有情况一致
//   空间：O(n)（临时数组）+ O(log n)（递归栈）
//   稳定：是（合并时相等元素保持原序）
//
// ============================================================

// ----------------------------------------------------------
// 合并两个有序子数组 arr[left..mid] 和 arr[mid+1..right]
// 使用临时数组 temp，合并后拷回 arr[left..right]
// ----------------------------------------------------------
void merge(vector<int>& arr, int left, int mid, int right)
{
    // 临时数组存放合并结果
    vector<int> temp(right - left + 1);

    int i = left;       // 左半指针：arr[left..mid]
    int j = mid + 1;    // 右半指针：arr[mid+1..right]
    int k = 0;          // temp 指针

    // 双指针归并：每次取较小的放入 temp
    while (i <= mid && j <= right)
    {
        if (arr[i] <= arr[j])  // 用 <= 保证稳定性（左半元素优先）
        {
            temp[k++] = arr[i++];
        }
        else
        {
            temp[k++] = arr[j++];
        }
    }

    // 左半有剩余 → 直接追加（右半已空）
    while (i <= mid)
    {
        temp[k++] = arr[i++];
    }

    // 右半有剩余 → 直接追加（左半已空）
    while (j <= right)
    {
        temp[k++] = arr[j++];
    }

    // 把合并好的 temp 拷回原数组
    for (size_t t = 0; t < temp.size(); t++)
    {
        arr[left + static_cast<int>(t)] = temp[t];
    }
}

// ============================================================
//              基础版归并排序（自顶向下递归）
// ============================================================
//
// 纯粹的递归分治，代码最简洁，但递归到底层开销大。
//
// ============================================================
void mergeSort_basic(vector<int>& arr, int left, int right)
{
    if (left >= right)
    {
        return;  // 只剩 0 或 1 个元素，自然有序
    }

    int mid = left + (right - left) / 2;  // 防止 (left+right) 溢出

    mergeSort_basic(arr, left, mid);       // 递归排序左半
    mergeSort_basic(arr, mid + 1, right);  // 递归排序右半
    merge(arr, left, mid, right);          // 合并两个有序半区
}

void mergeSort_basic(vector<int>& arr)
{
    mergeSort_basic(arr, 0, arr.size() - 1);
}

// ============================================================
//
//                   桶排序（Bucket Sort）
//
// ============================================================
//
// 【核心思想】
//   把数据分散到若干个"桶"里，每个桶单独排序（通常用插入排序，
//   因为桶内数据量小），最后把所有桶按顺序拼接起来。
//
//   公式：BucketSort(arr) = 分桶 + Σ 桶内排序 + 拼接
//
// 【物理直觉】
//   你是一年级老师，要把全班 50 份试卷按分数排序。
//   你不会直接 50 份一起排，而是：
//     1. 在桌上摆 10 个盒子（桶）：0-9分、10-19分、...、90-100分
//     2. 把每份试卷扔进对应盒子（分桶，O(n)）
//     3. 每个盒子内部单独排序（桶内排序，每桶数据很少）
//     4. 从低分到高分，把盒子里的试卷依次摞好（拼接，O(n)）
//
// 【时间复杂度】
//   设 n 个元素，m 个桶：
//     分桶：     O(n)          —— 每个元素计算一次桶编号
//     桶内排序： O(m × (n/m)²) —— 每个桶约 n/m 个元素，插入排序
//     拼接：     O(n)          —— 依次收集
//   总计：O(n + n²/m)
//     当 m ≈ n 时（桶数接近元素数）→ 接近 O(n)！
//
//   最好：  O(n)      数据均匀分布，每个桶 1~2 个元素
//   最坏：  O(n²)     所有数据挤进同一个桶（退化为插入排序）
//   平均：  O(n)      数据较均匀时
//
// 【与计数排序/基数排序的关系】
//   三者都是"非比较排序"，理论上可以突破 O(n log n) 下界：
//     计数排序：每个值一个桶（桶数 = 值域范围）
//     基数排序：按数位分桶，多轮处理
//     桶排序：  按值域区间分桶，桶内用比较排序
//   桶排序最灵活——它不是严格比较，也不要求值域连续。
//
// 【适用场景】
//   - 数据均匀分布在某个区间（如 [0, 1) 浮点数）
//   - 数据量大但值域可控
//   - 外部排序（数据分布在多个文件，各自排序后再归并）
//
// 【稳定性】稳定 —— 桶内用稳定排序（如插入排序），桶间按序拼接
//
// 【空间复杂度】O(n + m) —— n 个元素 + m 个桶
//
// ============================================================

void bucketSort(vector<int>& arr)
{
    int n = arr.size();
    if (n <= 1)
    {
        return;
    }

    // ---------- 第一步：找到值域范围 [minVal, maxVal] ----------
    int minVal = arr[0];
    int maxVal = arr[0];
    for (int i = 1; i < n; i++)
    {
        if (arr[i] < minVal)
        {
            minVal = arr[i];
        }
        if (arr[i] > maxVal)
        {
            maxVal = arr[i];
        }
    }

    // 如果所有元素都相同（minVal == maxVal），已经有序
    if (minVal == maxVal)
    {
        return;
    }

    // ---------- 第二步：创建桶 ----------
    // 桶的数量通常选 n，使得平均每个桶 1 个元素，接近 O(n)
    int bucketCount = n;
    // vector<vector<int>> 是 C++11 语法（需要空格避免 >> 被解析为右移）
    vector<vector<int> > buckets(bucketCount);

    // ---------- 第三步：把元素放进桶 ----------
    // 关键：如何计算元素属于哪个桶？
    //
    //   值域跨度 = maxVal - minVal
    //   每个桶覆盖的区间大小 = (maxVal - minVal + 1) / bucketCount
    //
    //   映射公式：
    //     bucketIndex = (val - minVal) * bucketCount / (maxVal - minVal + 1)
    //
    //   举例：range = [0, 100], n = 10 个桶
    //     val=5  → bucketIndex = (5-0) × 10 / 101 = 0  → 桶 0
    //     val=50 → bucketIndex = (50-0) × 10 / 101 = 4 → 桶 4
    //     val=99 → bucketIndex = (99-0) × 10 / 101 = 9 → 桶 9
    //
    for (int i = 0; i < n; i++)
    {
        int bucketIndex = (arr[i] - minVal) * bucketCount / (maxVal - minVal + 1);
        //                ─────────────────────────────────────────────────
        //                将值域 [minVal, maxVal] 线性映射到 [0, bucketCount-1]
        buckets[bucketIndex].push_back(arr[i]);
    }

    // ---------- 第四步：桶内排序 ----------
    // 每个桶用插入排序——桶内元素少，插入排序 O(n²) 的常数因子最小
    for (int i = 0; i < bucketCount; i++)
    {
        if (!buckets[i].empty())
        {
            insertion_sort(buckets[i]);  // 复用前面写的插入排序
        }
    }

    // ---------- 第五步：把桶按顺序拼接回原数组 ----------
    // 桶 0 → 桶 1 → ... → 桶 (bucketCount-1)，依次收集
    int index = 0;
    for (int i = 0; i < bucketCount; i++)
    {
        for (int j = 0; j < (int)buckets[i].size(); j++)
        {
            arr[index++] = buckets[i][j];
        }
    }
}

// ============================================================
//       严格 O(n) 桶排序 —— 经典教科书版本（浮点 [0, 1)）
//   （Strict O(n) Bucket Sort — Classic Textbook Version）
// ============================================================
//
// 【为什么这个版本能做到严格 O(n)？】
//   前提假设：输入为 n 个均匀分布在 [0, 1) 区间的浮点数。
//
//   在这个假设下，桶的数量 = n，桶 i 覆盖的区间为 [i/n, (i+1)/n)。
//   元素 x 放入桶的公式：bucketIndex = floor(n * x)
//
//   由于均匀分布，每个桶期望的元素数 = 1。
//   期望上每个桶只有常数个元素 → 桶内插入排序 O(1) × n 个桶 = O(n)。
//
//   严格来说这是"期望 O(n)"，但在均匀假设下，以极高概率：
//     最大桶大小 = O(log n / log log n)
//   因此实际运行时间严格接近 O(n)。
//
// 【与普通桶排序的核心区别】
//                             普通版                    严格 O(n) 版
//   输入类型                   任意整数                   浮点数 ∈ [0, 1)
//   桶数                        n 个                      n 个
//   分桶公式   (val-min)*n/(max-min+1)                   floor(n * val)
//   需要扫描找 min/max？        需要                       不需要！
//   桶大小稳定性                取决于数据分布              均匀分布下每个桶≈1
//   最坏情况                    全进一个桶 → O(n²)         依赖均匀假设
//
// 【为什么需要 [0, 1) 均匀假设？】
//   如果数据不满足假设（比如全挤在 [0, 0.1)），所有元素会进
//   前几个桶，桶内插入排序退化为 O(n²)。所以严格 O(n) 的
//   前提是"数据均匀分布"——这个假设是算法的一部分声明。
//
// 【整数数据怎么办？】
//   分两步：
//     1. 找到 min/max，把每个整数 x 归一化到 [0, 1)：
//        normalized = (double)(x - min) / (max - min + 1)
//     2. 对归一化后的浮点数组调用 strictBucketSort
//   见下方的 strictBucketSort_int 封装。
//
// 【空间复杂度】O(n)（n 个桶 + n 个元素的存储）
//
// ============================================================

void strictBucketSort(vector<double>& arr)
{
    int n = arr.size();
    if (n <= 1)
    {
        return;
    }

    // ---------- 第一步：创建 n 个桶 ----------
    // 桶 i 负责区间 [i/n, (i+1)/n)
    // 注意：输入的 arr 值必须在 [0, 1) 范围内
    vector<vector<double> > buckets(n);

    // ---------- 第二步：把每个元素放入对应桶 ----------
    // 公式：bucketIndex = floor(n * x)
    //
    // 推导：
    //   值域 [0, 1) 被等分为 n 个区间，每个宽度 = 1/n
    //   区间 i 的范围：[i/n, (i+1)/n)
    //   元素 x 属于区间 i 当且仅当 i/n ≤ x < (i+1)/n
    //   两边乘 n：i ≤ n·x < i+1
    //   即：i = floor(n·x)
    //
    // 举例（n = 10）：
    //   x = 0.23 → n*x = 2.3 → floor = 2 → 桶 2（区间 [0.2, 0.3)）
    //   x = 0.87 → n*x = 8.7 → floor = 8 → 桶 8（区间 [0.8, 0.9)）
    //   x = 0.99 → n*x = 9.9 → floor = 9 → 桶 9（区间 [0.9, 1.0)）
    //   x = 0.00 → n*x = 0.0 → floor = 0 → 桶 0（区间 [0.0, 0.1)）
    //
    for (int i = 0; i < n; i++)
    {
        int bucketIndex = (int)(n * arr[i]);  // floor(n * x)
        // 边界保护：x 理论上 ∈ [0,1)，但浮点误差可能导致 n*x == n
        if (bucketIndex >= n)
        {
            bucketIndex = n - 1;
        }
        buckets[bucketIndex].push_back(arr[i]);
    }

    // ---------- 第三步：桶内排序 ----------
    // 均匀分布下每个桶期望只有 1 个元素 → 插入排序 O(1)
    // 即使某个桶稍大（如 3~5 个），常数依然极小
    for (int i = 0; i < n; i++)
    {
        if (buckets[i].size() > 1)
        {
            // 手动插入排序（size 很小，无需调用函数）
            vector<double>& bucket = buckets[i];
            int sz = bucket.size();
            for (int k = 1; k < sz; k++)
            {
                double key = bucket[k];
                int j = k - 1;
                while (j >= 0 && bucket[j] > key)
                {
                    bucket[j + 1] = bucket[j];
                    j--;
                }
                bucket[j + 1] = key;
            }
        }
    }

    // ---------- 第四步：按桶顺序拼接 ----------
    int index = 0;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < (int)buckets[i].size(); j++)
        {
            arr[index++] = buckets[i][j];
        }
    }
}

// ============================================================
//  严格 O(n) 桶排序 —— 整数版本封装
//  内部将整数归一化到 [0,1)，调用上面 double 版本
// ============================================================
void strictBucketSort_int(vector<int>& arr)
{
    int n = arr.size();
    if (n <= 1)
    {
        return;
    }

    // 找最值
    int minVal = arr[0];
    int maxVal = arr[0];
    for (int i = 1; i < n; i++)
    {
        if (arr[i] < minVal) minVal = arr[i];
        if (arr[i] > maxVal) maxVal = arr[i];
    }
    if (minVal == maxVal) return;

    // 归一化到 [0, 1)
    vector<double> normalized(n);
    double range = maxVal - minVal + 1.0;  // +1 保证 maxVal 被映射到 < 1
    for (int i = 0; i < n; i++)
    {
        normalized[i] = (arr[i] - minVal) / range;
    }

    // 调用 strict O(n) 版本
    strictBucketSort(normalized);

    // 反归一化回整数
    for (int i = 0; i < n; i++)
    {
        arr[i] = minVal + (int)(normalized[i] * range + 0.5);
        //                                            ↑ 四舍五入，防止浮点截断
    }
}

// ============================================================
//
//                 基数排序（Radix Sort）—— LSD 版
//
// ============================================================
//
// 【核心思想 —— 逐位排序，从低位到高位】
//   不直接比较两个元素的大小，而是按"数位"逐轮排序：
//     第 1 轮：按个位排序
//     第 2 轮：按十位排序
//     第 3 轮：按百位排序
//     ...
//     最高位最后一轮
//
//   每轮内部用"计数排序"（稳定），保证同一数位相等时，
//   保留上一轮的顺序——这就是"基数排序正确性的关键"。
//
// 【为什么从低位（LSD）开始？】
//   LSD = Least Significant Digit
//
//   高位（MSD）是主导因素，但低位决定了"平局"时的顺序。
//   如果先排高位再排低位，低位的排序会打乱高位已排好的顺序。
//   先排低位再排高位：高位排序时，计数排序的稳定性保证
//   高位相同的元素保持低位轮次的顺序 → 最终全局有序！
//
//   类比：按"年月日"排序日期
//     LSD：先按日排 → 按月排 → 按年排 ✓（高位最终决定，低位是 tie-breaker）
//     MSD：先按年排 → 按月排 → 按日排 也能工作，但需要递归分治
//
// 【为什么每轮必须用稳定排序？】
//   假设已按个位排好：  [21, 31, 42, 52]
//   现在按十位排：21(2), 31(3), 42(4), 52(5) —— 十位各不相同，随便排
//   但如果十位相同：   [21, 31, 42, 52] → 十位排
//     unstable sort:  可能把 31 排到 21 前面 → 个位顺序丢失！
//     stable sort:    21 一定在 31 前面 → 个位顺序保留 ✓
//
//   结论：基数排序的正确性完全依赖每轮稳定排序的稳定性。
//
// 【计数排序作为稳定子程序】
//   选计数排序的原因：
//     1. O(n + k) 时间，k = 基数（10 进制 k=10，常数！）
//     2. 天然稳定——通过前缀和 + 倒序遍历保证
//     3. 不需要比较，只需知道当前数位的值
//
// 【时间复杂度】
//   设 d 轮（d = 最大数的位数），每轮 O(n + k)，其中 k = 10（进制）
//   T(n) = O(d × (n + k)) = O(d × n)（因为 k=10 是常数）
//
//   对于 32 位整数，d ≤ 10（10 进制最多 10 位）→ O(10n) = O(n)
//
//   最好/最坏/平均：全是 O(d × n)，与数据顺序完全无关！
//   这也是"非比较排序"可以突破 O(n log n) 的原因。
//
// 【稳定性】稳定 —— 每轮用稳定排序，整体稳定
//
// 【空间复杂度】O(n + k) —— 计数数组 (size k) + 临时输出数组 (size n)
//
// 【局限性】
//   1. 只能排整数（或可映射为整数的类型）
//   2. 需要额外 O(n) 空间
//   3. 如果 d 很大（如 64 位整数 10 进制），轮数过多
//      解决方案：用更大的基数（如基 256，一次处理 8 位，4 轮搞定 32 位）
//
// 【适用场景】
//   - 整数排序，数据量大但位数有限（如电话号码、身份证号）
//   - 需要稳定排序的场景
//   - 排序 key 长度固定的情况
//
// 【与桶排序/计数排序的区别】
//   计数排序：每个值是一个"桶"，桶数 = 值域大小（值域大时不可行）
//   桶排序：  按值域区间分桶，桶内用比较排序
//   基数排序：按数位分桶，多轮处理，桶数 = 基数（常数）
//   基数排序 = 计数排序的多轮应用，每轮只取一位数字
//
// ============================================================

// ----------------------------------------------------------
// 按指定位数做计数排序（稳定）
// exp = 1  → 按个位排序
// exp = 10 → 按十位排序
// exp = 100→ 按百位排序
// ...
// ----------------------------------------------------------
void countingSortByDigit(vector<int>& arr, int exp)
{
    int n = arr.size();
    vector<int> output(n);   // 临时输出数组
    int count[10] = {0};     // 计数数组：每位只有 0~9 共 10 种可能

    // ---- 第一步：统计当前位每个数字出现的次数 ----
    for (int i = 0; i < n; i++)
    {
        int digit = (arr[i] / exp) % 10;  // 提取当前位的数字
        //          ─────────────────────
        //          例：arr[i]=329, exp=10 → (329/10)%10 = 32%10 = 2 ← 十位
        count[digit]++;
    }

    // ---- 第二步：前缀和 → 确定每个数字在输出数组中的"最后位置 + 1" ----
    // count[d] 变为：当前位等于 d 的元素在 output 中应该被放到哪里
    for (int d = 1; d < 10; d++)
    {
        count[d] += count[d - 1];
    }

    // ---- 第三步：倒序遍历原数组，放入 output ----
    // 为什么倒序？
    //   前缀和给出的是"最后一个元素的位置 + 1"，
    //   倒序遍历保证：原数组中靠后的同 digit 元素被放到靠后的位置
    //   → 维持了上一轮的相对顺序（稳定性！）
    for (int i = n - 1; i >= 0; i--)
    {
        int digit = (arr[i] / exp) % 10;
        count[digit]--;                      // 该 digit 的"下一个可用位置"
        output[count[digit]] = arr[i];       // 放入
    }

    // ---- 第四步：拷回原数组 ----
    for (int i = 0; i < n; i++)
    {
        arr[i] = output[i];
    }
}

void radixSort(vector<int>& arr)
{
    int n = arr.size();
    if (n <= 1)
    {
        return;
    }

    // 找到最大值，确定需要多少轮
    int maxVal = arr[0];
    for (int i = 1; i < n; i++)
    {
        if (arr[i] > maxVal)
        {
            maxVal = arr[i];
        }
    }

    // 从个位开始，逐位做计数排序
    // 每一轮：exp = 1, 10, 100, 1000, ...
    // 当 maxVal / exp == 0 时，说明已处理完所有有效位
    for (int exp = 1; maxVal / exp > 0; exp *= 10)
    {
        countingSortByDigit(arr, exp);
    }
}

// ============================================================
//  基数排序 —— 处理负数版本
// ============================================================
//
// 上面的 LSD 基数排序只处理非负整数。
// 要处理负数，有多种方案：
//
//   方案一（简单）：把所有数 + offset 变为非负，排序后再减回去
//   方案二（优雅）：分正负两组，分别排序后拼接（负数取绝对值后反序）
//
//   这里采用方案一，更直观：
//     1. 找最小值 minVal
//     2. 所有元素减去 minVal（变为 ≥ 0）
//     3. 正常基数排序
//     4. 所有元素加回 minVal
//
// ============================================================
void radixSort_signed(vector<int>& arr)
{
    int n = arr.size();
    if (n <= 1)
    {
        return;
    }

    int minVal = arr[0];
    int maxVal = arr[0];
    for (int i = 1; i < n; i++)
    {
        if (arr[i] < minVal) minVal = arr[i];
        if (arr[i] > maxVal) maxVal = arr[i];
    }

    // 将所有元素偏移为非负数
    for (int i = 0; i < n; i++)
    {
        arr[i] -= minVal;
    }
    maxVal -= minVal;  // 最大值也做相应偏移

    // 标准基数排序
    for (int exp = 1; maxVal / exp > 0; exp *= 10)
    {
        countingSortByDigit(arr, exp);
    }

    // 还原
    for (int i = 0; i < n; i++)
    {
        arr[i] += minVal;
    }
}

// ============================================================
//
//               希尔排序（Shell Sort）
//
// ============================================================
//
// 【核心思想 —— 带"间隔"的插入排序】
//   插入排序有个致命弱点：小元素在最右端时，每次只能左移 1 格（乌龟）。
//   希尔排序的解决方案：先大步跳远，再小步精细化！
//
//   把数组按"间隔 gap"分成若干子序列，对每个子序列做插入排序。
//   gap 从大逐渐变小（如 n/2 → n/4 → ... → 1），最后一轮 gap=1
//   就是标准插入排序——但这时的数组已经"基本有序"，插入排序接近 O(n)！
//
// 【物理直觉 —— 大步挪远，小步调近】
//
//   gap = 5（间隔 5）：
//     索引: 0  1  2  3  4  5  6  7  8  9
//     元素: 9  8  3  6  1  7  2  5  4  0
//              ───┬───    ───┬───    ───┬───
//           子序列1: arr[0]=9, arr[5]=7, arr[10不存在]
//                    对 [9, 7] 做插入排序 → [7, 9]
//
//           子序列2: arr[1]=8, arr[6]=2
//                    对 [8, 2] 做插入排序 → [2, 8]
//           ...依此类推
//
//     结果: 7  2  3  6  0  9  8  5  4  1
//     (9 从索引 0 跳到 5，0 从索引 9 跳到 4——大步移动！)
//
//   gap = 2（间隔 2）：
//     索引: 0  1  2  3  4  5  6  7  8  9
//     元素: 7  2  3  6  0  9  8  5  4  1
//             ───┬───┬───┬───┬───    → 子序列1: arr[0,2,4,6,8] = [7,3,0,8,4]
//                ───┬───┬───┬───┬─── → 子序列2: arr[1,3,5,7,9] = [2,6,9,5,1]
//
//   gap = 1（标准插入排序）：
//     此时数组已接近有序，插入排序基本是 O(n)！
//
// 【为什么比直接插入排序快？】
//   1. 大 gap 让元素能跨越大段距离移动，快速消除"逆序对"
//   2. 每一轮 gap 缩小后，数组已经比上一轮更有序
//   3. 最后一轮 gap=1 时，插入排序面对的几乎是已有序数组
//
// 【时间复杂度 —— 依 gap 序列而定】
//
//   希尔原始序列 (n/2, n/4, ..., 1) : 最坏 O(n²)
//     → 若 n=2^k，奇数轮只比较偶数位，偶数轮才合拢，会退化
//
//   Hibbard 序列 (1, 3, 7, 15, ..., 2^k-1): 最坏 O(n^(3/2))
//     → gap 全部互质，避免"重复比较相同模式"
//
//   Knuth 序列 (1, 4, 13, 40, ..., (3^k-1)/2) : 最坏 O(n^(3/2))
//     → 实践中最常用的序列，经验性能很好
//
//   Sedgewick 序列 (1, 5, 19, 41, 109, ...) : 最坏 O(n^(4/3))
//     → 目前已知最优的序列之一
//
//   实现中选用 Hibbard 序列，gap 之间互质是关键。
//
// 【稳定性】不稳定 —— 相隔 gap 的元素交换时可能跨越相同元素
//   例：[2^a, 1, 2^b]，gap=2 时 2^a 和 1 交换 →
//       [1, 2^a, 2^b] — 两个 2 的相对位置可能改变
//
// 【空间复杂度】O(1) —— 原地排序，不需要额外数组
//
// 【适用场景】
//   - 中等规模数据的嵌入式系统（没有额外空间用归并）
//   - 不需要稳定性的场景
//   - C/C++ 的 qsort 在部分实现中退化为希尔排序（数据量不大时）
//
// ============================================================

void shellSort(vector<int>& arr)
{
    int n = arr.size();
    if (n <= 1)
    {
        return;
    }

    // ---- 第一步：生成 Hibbard 序列的初始 gap ----
    // Hibbard: 1, 3, 7, 15, 31, ..., 2^k - 1
    // 选择不超过 n 的最大 Hibbard 数作为初始 gap
    int gap = 1;
    while (gap < n / 3)  // 也接受除以 2 的变体
    {
        gap = gap * 3 + 1;  // Knuth 序列: 1, 4, 13, 40, 121, ...
        //      ─────────
        //      这是生成公式：next_gap = gap * 3 + 1
        //      等价于 (3^k - 1) / 2
        //      例如：1→4, 4→13, 13→40, 40→121
    }

    // ---- 第二步：外层循环——逐步缩小 gap ----
    while (gap >= 1)
    {
        // ---- 内层：对当前 gap 做"间隔插入排序" ----
        // 这就是插入排序！只是把"前一个元素"换成了"前 gap 个元素"
        //
        // 标准插入排序:
        //   for i=1..n-1: key=arr[i]; j=i-1; while j>=0 && arr[j]>key: arr[j+1]=arr[j]; j--
        //
        // 间隔插入排序:
        //   for i=gap..n-1: key=arr[i]; j=i-gap; while j>=0 && arr[j]>key: arr[j+gap]=arr[j]; j-=gap
        //
        // "1" 全部替换为 "gap"！
        for (int i = gap; i < n; i++)
        {
            int key = arr[i];     // 待插入的元素
            int j = i - gap;      // 比较对象是前 gap 个位置的元素

            // 在该子序列中向左找 key 的正确位置，边找边右移
            while (j >= 0 && arr[j] > key)
            {
                arr[j + gap] = arr[j];  // 右移 gap 格（而非 1 格！）
                j -= gap;               // 向前跳 gap 格
            }
            arr[j + gap] = key;         // 放入正确位置
        }

        gap /= 3;  // Knuth 序列的缩进公式（等价于 gap = (gap-1)/3 的整数近似）
        //       ↑ 和 gen 公式对称: gen=gap*3+1, shrink=gap/3
    }
}

// ============================================================
//  希尔排序 —— Shell 原始序列版（对比用）
//  gap: n/2, n/4, ..., 1
//  最坏 O(n²)，但实现最简单，便于理解
// ============================================================
void shellSort_original(vector<int>& arr)
{
    int n = arr.size();
    if (n <= 1)
    {
        return;
    }

    // Shell 原始：gap 从 n/2 开始，每次折半
    for (int gap = n / 2; gap >= 1; gap /= 2)
    {
        // 间隔插入排序，同上
        for (int i = gap; i < n; i++)
        {
            int key = arr[i];
            int j = i - gap;
            while (j >= 0 && arr[j] > key)
            {
                arr[j + gap] = arr[j];
                j -= gap;
            }
            arr[j + gap] = key;
        }
    }
}

// ============================================================
//               堆排序（Heap Sort）
// ============================================================
//
// 【核心思想 —— 利用堆的性质排序】
//   堆排序使用最大堆（或最小堆）来排序。
//   1. 建堆（heapify）：将数组转成一个最大堆，O(n)
//   2. 排序：反复取堆顶（最大元素），放到数组末尾，O(n log n)
//
//   最大堆性质：arr[parent(i)] ≥ arr[i]
//
// 【建堆 —— 从最后一个非叶节点开始向下调整】
//
//   数组: [4, 10, 3, 5, 1]
//   索引:  0   1  2  3  4
//
//   完全二叉树:
//         4(0)
//        /    ╲
//    10(1)    3(2)
//     /  ╲
//  5(3) 1(4)
//
//   最后一个非叶节点索引 = n/2 - 1 = 5/2 - 1 = 1（即 arr[1]=10）
//   从索引 1 到 0，逐个做 siftDown（下沉），调整子树为最大堆。
//
//   siftDown 的过程（以索引 0 为例）：
//     比较父节点与左右子节点，如果子节点更大就交换，
//     然后递归调整被交换的子节点子树。
//
// 【排序过程】
//
//   已建好最大堆: [10, 5, 3, 4, 1]（arr[0] = 10 最大）
//
//   第 1 轮：交换 arr[0] 和 arr[4] → [1, 5, 3, 4, 10]
//           arr[4] 已排好，堆范围缩小为 [0..3]
//           siftDown(arr[0]=1) → [5, 4, 3, 1, 10]
//
//   第 2 轮：交换 arr[0] 和 arr[3] → [1, 4, 3, 5, 10]
//           siftDown(arr[0]=1) → [4, 1, 3, 5, 10]
//
//   第 3 轮：交换 arr[0] 和 arr[2] → [3, 1, 4, 5, 10]
//           siftDown(arr[0]=3) → [3, 1, 4, 5, 10]
//
//   第 4 轮：交换 arr[0] 和 arr[1] → [1, 3, 4, 5, 10]
//
//   最终： [1, 3, 4, 5, 10]  ✓ 有序
//
// 【时间复杂度】
//   建堆：   O(n)      （不是 O(n log n)！数学可证明更紧确）
//   排序：   O(n log n)（n-1 次交换，每次 O(log n) siftDown）
//   总体：   O(n log n)
//
// 【空间复杂度】O(1) —— 原地排序，仅需常数临时变量
//
// 【稳定性】不稳定 —— 交换堆顶时可能打乱相同元素的相对顺序
//   例：[2^a, 1, 2^b] → 建堆 → [2^a, 1, 2^b] → 交换 →
//       [2^b, 1, 2^a] — 两个 2 的相对位置变了
//
// 【与归并/快速排序的比较】
//
//   ┌──────────┬──────────┬──────────┬──────────┐
//   │          │ 堆排序    │ 归并排序  │ 快速排序  │
//   ├──────────┼──────────┼──────────┼──────────┤
//   │ 时间     │ O(n log n)│ O(n log n)│ O(n²)最坏 │
//   │ 空间     │ O(1)     │ O(n)     │ O(log n) │
//   │ 稳定     │ 否       │ 是       │ 否       │
//   │ 缓存友好  │ 差       │ 好       │ 好       │
//   │ 适用场景  │ 嵌入式    │ 稳定需求  │ 通用最快  │
//   └──────────┴──────────┴──────────┴──────────┘
//
//   堆排序的"缓存友好性差"是因为 siftDown 跳着访问数组
//   （parent→left child→grandchild），不利于 CPU 缓存预取。
//
//   因此在实际工程中，快速排序通常更快（缓存局部性好），
//   但堆排序保证了真正的 O(n log n) 最坏情况。
//
// 【堆排序的变体 —— 内省排序（Introsort）】
//   C++ 的 std::sort 和 .NET 的 Array.Sort 使用 Introsort：
//     1. 先用快速排序
//     2. 如果递归深度 > 2*log n，切换到堆排序（避免快排退化）
//     3. 小区间用插入排序收尾
//   这样结合了三者的优点：快排的平均速度 + 堆排的最坏保证 + 插入的小数组效率。
//
// ============================================================

// ----------------------------------------------------------
// siftDown（下沉）：将节点 i 在堆范围 [0, heapSize-1] 内下沉
// 保持最大堆性质
// ----------------------------------------------------------
void heapSiftDown(vector<int>& arr, int i, int heapSize)
{
    while (true)
    {
        int largest = i;               // 假设当前节点最大
        int left = 2 * i + 1;          // 左子
        int right = 2 * i + 2;         // 右子

        // 在三个节点中找出最大的
        if (left < heapSize && arr[left] > arr[largest])
        {
            largest = left;
        }
        if (right < heapSize && arr[right] > arr[largest])
        {
            largest = right;
        }

        if (largest == i)
        {
            break;  // 已经满足堆序，停止
        }

        // 交换并继续下沉
        swap(arr[i], arr[largest]);
        i = largest;
    }
}

// ----------------------------------------------------------
// 堆排序主函数
// ----------------------------------------------------------
void heapSort(vector<int>& arr)
{
    int n = arr.size();
    if (n <= 1) return;

    // ---------- 第一步：建堆 O(n) ----------
    // 从最后一个非叶节点开始，逐个下沉
    // 为什么从 n/2-1 开始？因为 n/2 到 n-1 都是叶节点，无需下沉
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapSiftDown(arr, i, n);
    }

    // ---------- 第二步：排序 O(n log n) ----------
    // 反复将堆顶（最大元素）移到数组末尾
    for (int heapSize = n - 1; heapSize > 0; heapSize--)
    {
        // 交换堆顶和堆的最后一个元素
        swap(arr[0], arr[heapSize]);
        // arr[heapSize..n-1] 是已排序区
        // 堆范围缩小，修复堆顶
        heapSiftDown(arr, 0, heapSize);
    }
}

// ============================================================
//             计数排序（Counting Sort）
// ============================================================
//
// 【核心思想 —— 数数而非比较】
//   计数排序不是比较排序！它通过统计每个值出现的次数来排序。
//   这使得它能突破 O(n log n) 的比较排序下界，达到 O(n + k)。
//
//   其中 k = maxVal - minVal + 1（值域范围）。
//
// 【算法流程】
//   1. 找到最大值和最小值，确定值域范围 k
//   2. 创建计数数组 count[k]，统计每个值出现的次数
//   3. 计算前缀和（累加计数），确定每个值在输出数组中的位置
//   4. 从右向左遍历原数组（保证稳定性），
//      根据 count[value] 将元素放到正确位置
//
// 【为什么从右向左遍历保证稳定性？】
//   前缀和数组存储的是"有多少个元素 ≤ 当前值"。
//   从右向左处理时，相同值的最后一个元素放在最后位置，
//   倒数第二个放在倒数第二位置，依次类推 → 稳定！
//
//   如果从左向右处理，相同值的顺序会反转 → 不稳定。
//
// 【时间复杂度】
//   最好/最坏/平均：O(n + k)
//   当 k = O(n) 时，总体 O(n) → 线性时间！
//   当 k >> n 时（如值域是 [0, 10^9]），时间和空间都不可接受。
//
// 【空间复杂度】O(n + k) —— 输出数组 + 计数数组
//
// 【稳定性】稳定（从右向左遍历保证）
//
// 【适用场景】
//   - 值域小且密集（如考试成绩 0~100、年龄 0~150）
//   - 需要稳定排序时（作为基数排序的子程序）
//   - 整数排序
//
// 【不适合的场景】
//   - 值域很大（如 32 位整数的全范围）
//   - 非整数数据（浮点数、字符串等）
//   - 数据稀疏（值域大但只有少量元素）
//
// 【计数排序 vs 桶排序】
//   计数排序：每个值是一个"桶"，桶数 = 值域大小
//   桶排序：  按区间分桶，每个桶可以装多个不同的值
//   因此计数排序可以看作桶排序的一个特例（桶大小 = 1）。
//
// ============================================================

void countingSort(vector<int>& arr)
{
    int n = arr.size();
    if (n <= 1) return;

    // ---------- 第一步：找值域范围 ----------
    int minVal = arr[0], maxVal = arr[0];
    for (int i = 1; i < n; i++)
    {
        if (arr[i] < minVal) minVal = arr[i];
        if (arr[i] > maxVal) maxVal = arr[i];
    }

    int range = maxVal - minVal + 1;

    // ---------- 第二步：统计每个值的出现次数 ----------
    vector<int> count(range, 0);
    for (int i = 0; i < n; i++)
    {
        count[arr[i] - minVal]++;
    }

    // ---------- 第三步：计算前缀和 ----------
    // count[v] = 值 ≤ v 的元素个数
    // 即 count[v] 表示值 v 在输出数组中的"最后一个位置 + 1"
    for (int i = 1; i < range; i++)
    {
        count[i] += count[i - 1];
    }

    // ---------- 第四步：构建输出数组（从右向左，保证稳定性）----------
    vector<int> output(n);
    for (int i = n - 1; i >= 0; i--)    // ← 从右向左！保证稳定性
    {
        int idx = arr[i] - minVal;       // 当前值的计数索引
        output[count[idx] - 1] = arr[i]; // 放到正确位置
        count[idx]--;                    // 该值计数减 1
    }

    // ---------- 第五步：写回原数组 ----------
    for (int i = 0; i < n; i++)
    {
        arr[i] = output[i];
    }
}

// ============================================================
//              外部排序（External Sort）—— 原理讲解
// ============================================================
//
// 【什么是外部排序？】
//   当数据量大到无法全部装入内存时，需要使用外部排序。
//   数据存储在磁盘（外存）上，每次只能读取一部分到内存中处理。
//
//   典型场景：10GB 的日志文件排序，但只有 1GB 可用内存。
//
// 【核心策略 —— 多路归并（K-way Merge）】
//
//   算法分为两个阶段：
//
//   阶段 1：分块排序（Run Generation / Internal Sort）
//     - 将大文件分成若干能装入内存的"块"（chunk/run）
//     - 每个块读入内存，用内部排序（如快速排序）排好序
//     - 将排序后的块写回磁盘（称为一个 sorted run）
//
//     例如：10GB 文件，1GB 内存 → 分成 10 个 sorted runs
//
//     文件: [//////////////////////// 10GB ////////////////////////]
//              ↓                    ↓                    ↓
//           Run1 (1GB)          Run2 (1GB)    ...    Run10 (1GB)
//              ↓ 内部排序          ↓ 内部排序          ↓ 内部排序
//           Run1 sorted         Run2 sorted       Run10 sorted
//
//   阶段 2：多路归并（K-way Merge）
//     - 同时打开 K 个 sorted runs，各读取一小块到内存缓冲区
//     - 使用 最小堆（K 个元素）选择当前最小的元素输出
//     - 输出缓冲区满了就写回磁盘
//     - 某个输入缓冲区空了就从对应的 run 中读下一块
//
//     归并过程示意（K=3）：
//
//       Run1: [2, 5, 9, ...]  → 缓冲区1: [2, 5]
//       Run2: [1, 4, 8, ...]  → 缓冲区2: [1, 4]   最小堆: [1, 2, 5]
//       Run3: [3, 6, 7, ...]  → 缓冲区3: [3, 6]
//                                           ↓ 弹出最小
//                    输出:  [1, 2, 3, ...]
//
// 【复杂度分析】
//
//   阶段 1（分块排序）：
//     块数 = N/M（N=总数据量, M=可用内存）
//     每块排序 O(M log M)
//     总时间：O(N/M × M log M) = O(N log M)
//
//   阶段 2（K 路归并）：
//     每轮归并 O(N log K)（最小堆大小 K）
//     如果一趟归并就能完成：总时间 O(N log M)
//     如果需要多趟（两两归并）：≈ O(N log (N/M))
//
//   总体：约 O(N log N) 的 I/O 量 + O(N log M) 的内部计算
//
// 【优化 —— 置换-选择排序（Replacement Selection）】
//   标准分块排序产生大小固定为 M 的 runs。
//   置换-选择排序可以产生平均 2M 长度的 runs，减少归并趟数。
//
//   方法：使用最小堆，当弹出的元素比前一个输出小（"冻结"），
//   等所有在堆中的"可用"元素都输出后，下一轮再处理冻结的。
//
// 【优化 —— 多趟归并与归并树】
//   如果内存只能同时打开 K 个 runs，但总共有 R > K 个 runs，
//   需要多趟归并：
//
//     第 1 趟：合并前 K 个 runs → 产生更大的 run
//     第 2 趟：合并下一批 K 个 runs → ...
//     ...
//     直到所有 runs 合并为一个。
//
//   总趟数 = ⌈log_K R⌉（类似多路树的层数）
//
// 【外部排序的应用】
//   - 数据库的 ORDER BY（数据超过 sort_buffer_size）
//   - Hadoop/Spark 的 Shuffle 排序阶段
//   - 大文件的去重（sort + unique）
//   - 磁带排序（历史上最早的算法场景之一）
//
// 【注意】
//   外部排序是一个概念框架而非固定的代码实现，因为：
//   1. 涉及文件 I/O，与操作系统相关
//   2. 输入/输出缓冲区大小依赖于硬件特性
//   3. 实际系统（如数据库）会将此逻辑封装在存储引擎中
//
//   下面的代码是一个简化的 K 路归并模拟（数据在内存中，
//   但模拟分块和堆归并的过程，便于理解）。
//
// ============================================================

// ----------------------------------------------------------
// 模拟外部排序（K 路归并核心逻辑）
// 假设数据已经被分成若干有序的 runs
// ----------------------------------------------------------
void kWayMerge(const vector<vector<int>>& runs, vector<int>& output)
{
    int K = (int)runs.size();
    if (K == 0) return;

    // 堆元素：(值, 来自哪个 run, 在该 run 中的位置)
    struct HeapElement
    {
        int value;
        int runIdx;
        int posInRun;
        bool operator<(const HeapElement& other) const
        {
            return value > other.value;  // 最小堆（与默认相反）
        }
    };

    priority_queue<HeapElement> minHeap;

    // 初始化堆：每个 run 的第一个元素入堆
    for (int i = 0; i < K; i++)
    {
        if (!runs[i].empty())
        {
            minHeap.push({runs[i][0], i, 0});
        }
    }

    // 反复从堆中取最小元素
    output.clear();
    while (!minHeap.empty())
    {
        HeapElement top = minHeap.top();
        minHeap.pop();

        output.push_back(top.value);

        // 从同一个 run 中取下一个元素
        int nextPos = top.posInRun + 1;
        if (nextPos < (int)runs[top.runIdx].size())
        {
            minHeap.push({runs[top.runIdx][nextPos], top.runIdx, nextPos});
        }
    }
}

// ----------------------------------------------------------
// 模拟外部排序完整流程
// ----------------------------------------------------------
void externalSortDemo(const vector<int>& data, int memSize,
                       vector<int>& sorted)
{
    int n = (int)data.size();
    if (n == 0) return;

    // ===== 阶段 1：分块排序 =====
    int numRuns = (n + memSize - 1) / memSize;  // 向上取整
    vector<vector<int>> runs(numRuns);

    cout << "  外部排序模拟：总数据 " << n << " 个, 内存可装 "
         << memSize << " 个, 分成 " << numRuns << " 个块" << endl;

    for (int r = 0; r < numRuns; r++)
    {
        int start = r * memSize;
        int end = min(start + memSize, n);

        // 读入一个块
        runs[r].assign(data.begin() + start, data.begin() + end);

        // 内部排序（模拟快速排序）
        sort(runs[r].begin(), runs[r].end());

        cout << "    Run " << r << " sorted: [";
        for (int i = 0; i < min(5, (int)runs[r].size()); i++)
        {
            cout << runs[r][i];
            if (i < min(4, (int)runs[r].size() - 1)) cout << ", ";
        }
        if ((int)runs[r].size() > 5) cout << ", ...";
        cout << "] (" << runs[r].size() << " 个元素)" << endl;
    }

    // ===== 阶段 2：K 路归并 =====
    kWayMerge(runs, sorted);
}

// ============================================================
//   专题：逆序对（Inversion Count）
// ============================================================
//
// 【定义】
//   在一个序列 a[0..n-1] 中，如果 i < j 且 a[i] > a[j]，
//   则 (a[i], a[j]) 构成一个逆序对。
//
//   逆序对数量反映数组的"无序程度"：
//     - 完全升序：逆序对 = 0
//     - 完全降序：逆序对 = n(n-1)/2（最大）
//     - 插入排序的每次交换消除恰好一个逆序对 → 交换次数 = 逆序对数
//
// 【示例】arr = [2, 4, 1, 3]
//   逆序对：(2,1), (4,1), (4,3), (2,1)...
//   详细：2>1(✓), 4>1(✓), 4>3(✓), 2>3(✗), 1>3(✗)
//   答案 = 3 对：{ (2,1), (4,1), (4,3) }
//
// ============================================================

// ---------- 方法 1：归并排序计数 O(n log n) ----------
// 核心思路：归并时，如果左半部分的 a[i] > 右半部分的 a[j]，
// 那么左半部分从 i 到 mid 的所有元素都 > a[j]（因为左右各自已排序）。
// 所以逆序对增加 (mid - i + 1)。
//
// 示例：归并 L=[2,5,8] 和 R=[1,3,6]：
//   L[0]=2 > R[0]=1 → +3 (2,1)(5,1)(8,1)
//   L[0]=2 < R[1]=3 → 正常归并
//   L[1]=5 > R[1]=3 → +2 (5,3)(8,3)
//   继续...

long long mergeAndCount(vector<int>& arr, vector<int>& tmp, int left, int mid, int right)
{
    int i = left;       // 左半起始
    int j = mid + 1;    // 右半起始
    int k = left;       // 临时数组下标
    long long invCount = 0;

    while (i <= mid && j <= right)
    {
        if (arr[i] <= arr[j])
        {
            tmp[k++] = arr[i++];
        }
        else
        {
            // arr[i] > arr[j] → 左半从 i 到 mid 的所有元素都 > arr[j]
            tmp[k++] = arr[j++];
            invCount += (mid - i + 1);
        }
    }

    // 剩余元素直接复制
    while (i <= mid) tmp[k++] = arr[i++];
    while (j <= right) tmp[k++] = arr[j++];

    for (i = left; i <= right; i++)
        arr[i] = tmp[i];

    return invCount;
}

long long mergeSortAndCount(vector<int>& arr, vector<int>& tmp, int left, int right)
{
    long long invCount = 0;
    if (left < right)
    {
        int mid = left + (right - left) / 2;
        invCount += mergeSortAndCount(arr, tmp, left, mid);
        invCount += mergeSortAndCount(arr, tmp, mid + 1, right);
        invCount += mergeAndCount(arr, tmp, left, mid, right);
    }
    return invCount;
}

long long countInversions(vector<int> arr)
{
    vector<int> tmp(arr.size());
    return mergeSortAndCount(arr, tmp, 0, arr.size() - 1);
}

// ---------- 方法 2：暴力法 O(n²)（仅用于小数组验证）----------
long long countInversionsBrute(vector<int>& arr)
{
    long long count = 0;
    for (size_t i = 0; i < arr.size(); i++)
        for (size_t j = i + 1; j < arr.size(); j++)
            if (arr[i] > arr[j])
                count++;
    return count;
}

// ============================================================
//                         测 试 主 函 数
// ============================================================

int main()
{
    cout << "╔══════════════════════════════════╗" << endl;
    cout << "║    排 序 算 法 综 合 测 试       ║" << endl;
    cout << "╚══════════════════════════════════╝" << endl;

    // ---- 插入排序 ----
    cout << "\n┌─ 插入排序 ─────────────────────┐" << endl;
    vector<int> arr1 = {5, 2, 8, 1, 9, 3};
    insertion_sort(arr1);
    cout << "结果: ";
    for (int v : arr1) cout << v << " ";
    cout << "(expect 1 2 3 5 8 9)" << endl;

    // ---- 冒泡排序（鸡尾酒） ----
    cout << "\n┌─ 鸡尾酒排序 ───────────────────┐" << endl;
    vector<int> arr2 = {5, 1, 4, 2, 8};
    bubble_sort_v3_cocktail(arr2);
    cout << "结果: ";
    for (int v : arr2) cout << v << " ";
    cout << "(expect 1 2 4 5 8)" << endl;

    // ---- 快速排序（终极版） ----
    cout << "\n┌─ 快速排序（三数取中+截断+三路）──┐" << endl;
    vector<int> arr3 = {3, 6, 8, 10, 1, 2, 1, 5, 7, 9, 4, 2};
    quickSort_v4_ultimate(arr3);
    cout << "结果: ";
    for (int v : arr3) cout << v << " ";
    cout << "(expect 升序)" << endl;

    // ---- 归并排序 ----
    cout << "\n┌─ 归并排序 ─────────────────────┐" << endl;
    vector<int> arr4 = {38, 27, 43, 3, 9, 82, 10};
    mergeSort_basic(arr4);
    cout << "结果: ";
    for (int v : arr4) cout << v << " ";
    cout << "(expect 3 9 10 27 38 43 82)" << endl;

    // ---- 基数排序 ----
    cout << "\n┌─ 基数排序 ─────────────────────┐" << endl;
    vector<int> arr5 = {170, 45, 75, 90, 802, 24, 2, 66};
    radixSort(arr5);
    cout << "结果: ";
    for (int v : arr5) cout << v << " ";
    cout << "(expect 2 24 45 66 75 90 170 802)" << endl;

    // ---- 堆排序 ----
    cout << "\n┌─ 堆排序 ───────────────────────┐" << endl;
    vector<int> arr6 = {4, 10, 3, 5, 1, 7, 8, 2, 6, 9};
    heapSort(arr6);
    cout << "结果: ";
    for (int v : arr6) cout << v << " ";
    cout << "(expect 1 2 3 4 5 6 7 8 9 10)" << endl;

    // ---- 计数排序 ----
    cout << "\n┌─ 计数排序 ─────────────────────┐" << endl;
    vector<int> arr7 = {4, 2, 2, 8, 3, 3, 1, 5, 0};
    countingSort(arr7);
    cout << "结果: ";
    for (int v : arr7) cout << v << " ";
    cout << "(expect 0 1 2 2 3 3 4 5 8)" << endl;

    // ---- 逆序对计数 ----
    cout << "\n┌─ 逆序对计数 ───────────────────┐" << endl;
    {
        vector<int> invArr = {2, 4, 1, 3};
        cout << "数组: 2 4 1 3" << endl;
        cout << "逆序对(归并法)=" << countInversions(invArr) << " (expect 3)" << endl;
        cout << "逆序对(暴力法)=" << countInversionsBrute(invArr) << " (expect 3)" << endl;

        vector<int> asc = {1, 2, 3, 4, 5};
        cout << "升序数组逆序对=" << countInversions(asc) << " (expect 0)" << endl;

        vector<int> desc = {5, 4, 3, 2, 1};
        cout << "降序数组逆序对=" << countInversions(desc) << " (expect 10)" << endl;
    }

    cout << "\n所有测试完成！" << endl;
    return 0;
}
