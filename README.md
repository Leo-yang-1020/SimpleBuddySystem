# SimpleBuddySystem
Buddy system的极简实现方式

### Buddy system理论

> 知其然还要知其所以然

Linux引入Buddy system使用必然存在他的必要原因。

Linux基于段页式机制管理物理内存，内存被分割成一个个页框，由多级页表管理。除此之外，由于硬件的约束：

- DMA处理器，只能对RAM的前16MB寻址。
- 32位机器CPU最大寻址空间，只有4GB，对于大容量超过4GB的RAM，无法访问所有的地址空间。

Linux还将物理内存划分为不同的管理区：ZONE_DMA、ZONE_NORMAL、ZONE_HIGHMEM，每个管理区都有自己的描述符，也有自己的页框分配器，示意图如下：

![](https://cdn.jsdelivr.net/gh/Leo-yang-1020/picRepo/img/20210812203120.png)

对于连续页框组的内存分配请求，是由管理区分配器完成，每个管理区的页框分配是通过伙伴系统算法来实现。内核经常请求和释放单个页框，为了提高性能，每个内存管理区，还定义了一个CPU页框高速缓存，包含一些预选分配的页框。

**伙伴系统算法：**内核为分配一组**连续**的页框而建立的一种健壮、高效的分配策略，这种策略缓解了内存碎片的发生。算法的核心思想：是把所有的空闲页框分组为11个块链表，每个块链表分别包含1、2、4、8、16、...、512、1024个连续页框。举个简单的例子，说明算法的工作过程。

**伙伴系统属于连续内存分配的算法，但是在Linux中的应用，并不是针对于整块大内存，而是对于页框进行分配，在性能与效果上，能够减少外部碎片，并且便于快速分配与回收。**

假设需要256个页框的连续内存，算法先在256个页框的链表中，检查是否还有空闲块，如果有就分配出去。如果没有，算法会找到下一个更大的512页框的链表，如果存在空闲块，内核会把512页框分割成两部分，一半用来分配，另一半插入到256页框的链表中。

至此，我们可以看到，Buddy system在Linux中得到使用的原因。



#### Buddy system的定义，分配，释放

这里给出伙伴的概念，满足以下三个条件的称为伙伴：
1）两个块大小相同；
2）两个块地址连续；
3）两个块必须是同一个大块中分离出来的；



分配的情况：

伙伴系统只会分配为2的幂大小的块（如果是页框，即为2的幂数量的页框）

当需要的分区大小为 2^u-1<s<=2^u 时，会将整个块分配给该进程

简单的话来讲，就是分配一块刚刚好的块，即不大，也不小。

释放合并的情况：

当内存块完成释放后，需要进行紧凑。

紧凑进行的条件是：

- 大小相同
- 地址相邻
- 是从原来同一个块分离出来的



### 简易实现伙伴系统

既然是简易实现，我们就不需要考虑太多空间与时间的效应问题，只抓住最核心的需求：有内存，分配内存，回收内存。

伙伴系统的层次图很像一颗满二叉树，因此，我们自然选择了这样的数据结构去实现分配（满二叉树用数组便可以轻松表示，何乐而不为）

 核心结构体定义：

```c
struct buddy2 {
  unsigned size;//表明管理内存的总单元数
  unsigned longest[1]; //完全二叉树的节点标记，表明对应内存块的空闲单位，长度为1方便扩展
};
```



初始化过程：

```c
struct buddy2* buddy2_new( int size ) {//初始化分配器
  struct buddy2* self;
  unsigned node_size;//节点所拥有的内存大小
  int i;

  if (size < 1 || !IS_POWER_OF_2(size))
    return NULL;

  self = (struct buddy2*)ALLOC( 2 * size * sizeof(unsigned));
  self->size = size;
  node_size = size * 2;

  for (i = 0; i < 2 * size - 1; ++i) {//初始化每一个节点
    if (IS_POWER_OF_2(i+1))
      node_size /= 2;
    self->longest[i] = node_size;
    //初始化操作，向表示满二叉树的数组赋值，例子：size为16 则赋值的数组为[16,8,8,4,4,4,4....]
    //表示节点对应的块的大小
  }
  return self;
}
```

最巧妙的就在于longest数组的使用。该数组相当于是满二叉树，每个节点表示了块的大小。

在分配算法和释放算法中我们将详细讲述该数组的使用，该数组的引入可以避免使用**状态机**表示节点状态，简化编程。

分配算法：

```c
/**
 * 寻找到合适的size,返回-1说明无法分配，分配失败，如果分配成功，会返回一个offset，代表分配的块在数组中的位置
 * @param self
 * @param size
 * @return
 */
int buddy2_alloc(struct buddy2* self, int size) {
  unsigned index = 0;//节点的标号(数组下标)
  unsigned node_size;
  unsigned offset = 0;

  if (self==NULL)//无法分配
    return -1;

  if (size <= 0)//分配不合理
    size = 1;
  else if (!IS_POWER_OF_2(size))//不为2的幂时，取比size更大的2的n次幂
    size = fixsize(size);

  if (self->longest[index] < size)//可分配内存不足时，直接返回，否则，一定能找到合适的块
    return -1;

  for(node_size = self->size; node_size != size; node_size /= 2 ) {
    //从最大根节点开始向下寻找合适的节点, node_size表示该层节点所能代表最大块
    //到达层数所能代表的最大块等于要寻找的size的上一层为止  原因：继续找下去已经无意义
    //先找左子树
    if (self->longest[LEFT_LEAF(index)] >= size)
      index = LEFT_LEAF(index);
    else
      index = RIGHT_LEAF(index);//左子树不满足时选择右子树
  }

  self->longest[index] = 0;//将节点标记为已使用
  offset = (index + 1) * node_size - self->size;//计算偏移量
	//向上刷新
  while (index) {
    index = PARENT(index);
    self->longest[index] =
      MAX(self->longest[LEFT_LEAF(index)], self->longest[RIGHT_LEAF(index)]);
    	//刷新父节点；由于块分配，因此从根节点到该节点的块的大小都需要更新
      //注意，这里并不是单纯地减去，而是以块切割。例如第一个节点为16，当切割出大小为2的块时，16会变成8，而非14
  }

  return offset;
  //返回分配的块的offset，便于标志
}
```

释放的算法：

释放块：应该释放对应offset：

从最后的节点开始一直往上找到longest为0的节点，即当初分配块所适配的大小和位置。**我们将longest恢复到原来满状态的值。继续向上回溯，检查是否存在合并的块，依据就是左右子树longest的值相加是否等于原空闲块满状态的大小，如果能够合并，就将父节点longest标记为相加的和**

```c
/**
 * 释放掉数组位置为offset的块
 * @param self
 * @param offset
 */
void buddy2_free(struct buddy2* self, int offset) {
  unsigned node_size, index = 0;
  unsigned left_longest, right_longest;

  assert(self && offset >= 0 && offset < self->size);//判断请求是否出错

  node_size = 1;
  index = offset + self->size - 1;

  for (; self->longest[index] ; index = PARENT(index)) {//向上寻找分配过的节点
    node_size *= 2;
    if (index == 0)//如果节点不存在
      return;
  }

  self->longest[index] = node_size;//恢复内存

  while (index) {
      //compaction算法，释放后的核心算法
      //释放后需要依次向上更新从根到该结点的块
    index = PARENT(index);
    node_size *= 2;

    left_longest = self->longest[LEFT_LEAF(index)];
    right_longest = self->longest[RIGHT_LEAF(index)];

    if (left_longest + right_longest == node_size) //如果可以则合并
      self->longest[index] = node_size;
    else
      self->longest[index] = MAX(left_longest, right_longest);
      //否则块大小保持左右孩子最大的一个

  }
}
```

分配算法的进一步改进：

我们发现，由于优先找左子树的内容，因此，即使右子树的块已经分割好，有更好的块，也会优先去左子树拆大块，因此，分配的算法可以进一步改进：

```c
 for(node_size = self->size; node_size != size; node_size /= 2 ) {
    //从最大根节点开始向下寻找合适的节点, node_size表示该层节点所能代表最大块
    //到达层数所能代表的最大块等于要寻找的size的上一层为止  原因：继续找下去已经无意义
    if (self->longest[LEFT_LEAF(index)] < size) {
            index = RIGHT_LEAF(index);//如果左子树大小不够，可直接转向右子树
        } else {
            if (self->longest[RIGHT_LEAF(index) < self->longest[LEFT_LEAF(index)]]) {
                //如果左子树小于右子树，我们优先选择小的分配（避免拆分大内存块造成外碎片浪费）
                index = LEFT_LEAF(index);
            } else {
                //左子树大于右子树，如果右子树够大，则直接选择右子树
                if (self->longest[RIGHT_LEAF(index)] >= size) {
                    index = RIGHT_LEAF(index);
                } else {
                    index = LEFT_LEAF(index);
                }
            }
        }
 }
```



### ucore中的实现









### 真实Linux下是如何实现伙伴系统的？

#### NUMA

所谓物理内存，就是安装在机器上的，实打实的内存设备（不包括硬件cache），被CPU通过总线访问。在多核系统中，如果物理内存对所有CPU来说没有区别，每个CPU访问内存的方式也一样，则这种体系结构被称为Uniform Memory Access(UMA)。

如果物理内存是分布式的，由多个cell组成（比如每个核有自己的本地内存），那么CPU在访问靠近它的本地内存的时候就比较快，访问其他CPU的内存或者全局内存的时候就比较慢，这种体系结构被称为Non-Uniform Memory Access(NUMA)。

以上是硬件层面上的NUMA，而作为软件层面的Linux，则对NUMA的概念进行了抽象。即便硬件上是一整块连续内存的UMA，Linux也可将其划分为若干的node。同样，即便硬件上是物理内存不连续的NUMA，Linux也可将其视作UMA。

所以，在Linux系统中，你可以基于一个UMA的平台测试NUMA上的应用特性。从另一个角度，UMA就是只有一个node的特殊NUMA，所以两者可以统一用NUMA模型表示。

![](https://cdn.jsdelivr.net/gh/Leo-yang-1020/picRepo/img/20210813151050.png)



在NUMA系统中，当Linux内核收到内存分配的请求时，它会优先从发出请求的CPU本地或邻近的内存node中寻找空闲内存，这种方式被称作local allocation，local allocation能让接下来的内存访问相对底层的物理资源是local的。

每个node由一个或多个zone组成（我们可能经常在各种对虚拟内存和物理内存的描述中迷失，但以后你见到zone，就知道指的是物理内存），每个zone又由若干page frames组成（一般page frame都是指物理页面）。

![img](https://pic3.zhimg.com/80/v2-7219a5a8a8a068b2716eb815440b3b0e_1440w.jpg)

而，这也是Linux核心的数据结构：

Linux中，将访问内存定义为一个内存节点，总体上Linux采取了**节点(node)，域(zone)和页面(page)**三级结构描述物理内存。

虽然内存访问的最小单位是byte或者word，但MMU是以page为单位来查找页表的，page也就成了Linux中内存管理的重要单位。包括换出（swap out）、回收（relcaim）、映射等操作，都是以**page**为粒度的。



#### free_area

关于ZONE，只需要知道属性相同的page会被划分到同一个zone，便于内存管理罢了。buddy system ，也就是zone中管理page分配的算法。

```c
struct zone {
    /* free areas of different sizes */
    struct free_area    free_area[MAX_ORDER];
    ...
}

#define MAX_ORDER 11
#define MAX_ORDER_NR_PAGES (1 << (MAX_ORDER - 1))
```

**MAX_ORDER为11代表着什么呢？**

这将所有的空闲页框分组分为了11块链表-> 例如下标为1 表示可以包含2^1个页面的块。如此类推。

将所有的空闲页框分为11个组，同等大小的串成一个链表，对应到free_area数组中。

![](https://cdn.jsdelivr.net/gh/Leo-yang-1020/picRepo/img/20210813153827.png)

如图所示，数组下标对应着相应大小块的链表

注意，不同大小的块在空间上，不会有重叠。当一个需求为4个连续页面时，检查是否有4个页面的空闲块而快速满足请求。若该链表上（每个结点都是大小为4页面的块）有空闲的块，则分配给用户，否则向下一个级别（order）的链表中查找。若存在（8页面的）空闲块（现处于另外一个级别的链表上），则将该页面块**分裂为两个4页面的块，一块分配给请求者，另外一块加入到4页面的块链表中**。这样可以避免分裂大的空闲块，而此时有可以满足需求的小页面块，从而减少外面碎片。

![100911_1610_Linux2.png?w=620](http://i1.wp.com/ilinuxkernel.com/wp-content/uploads/2011/10/100911_1610_Linux2.png?w=620)

#### 复合页

> 那由多个页构成的blcok该如何表示呢？

如果"order"的值大于0，那么同一个free list中的一个节点上的page frames不只一个，它们构成了一个"**compound pages**"，其中的第一个page frame被称为"**head page**"（如下图蓝色部分所示），其余的page frame被称为"**tail page**"（如下图黄色部分所示）。

![](https://cdn.jsdelivr.net/gh/Leo-yang-1020/picRepo/img/20210813155257.png)

（这里有点扯，内核并没有为描述compound page而单独定义结构体，一个compound page的相关信息（比如order）是「塞」在组成这个compound page的页面的"struct page"中的）



每个"free_area"包含不只一个**free list**，而是一个free list数组（形成二维数组），包含了多种"**migrate type**"，以便将具有相同「迁移类型」的page frame尽可能地分组（同一类型的所有order的free list构成一组"**pageblocks**"）

![img](https://pic1.zhimg.com/80/v2-64fe33c229f845d5a86388f52352e578_1440w.jpg)

简单地理解，迁移类型migrate就是将page再做了一次分组，仅此而已。



### buddy内存释放

> 仅仅简单讲讲，没有展开详细叙述代码

![](https://cdn.jsdelivr.net/gh/Leo-yang-1020/picRepo/img/20210813161103.png)

对于compound page，在__free_pages_ok()中，获得物理页面号"pfn"和"migratetype"，准备好了核心函数__free_one_page()所需的各项参数。

```c
static void __free_pages_ok(struct page *page, unsigned int order)
{
    unsigned long pfn = page_to_pfn(page);
    int migratetype = get_pfnblock_migratetype(page, pfn);

    free_one_page(page_zone(page), page, pfn, order, migratetype);
    ...
}
```

**pfn**用于标记每一个page，一个compound page 就会有多个pfn。

因为一个page和它的buddy在**物理内存上是连续**的，确定"order"之后，就可以根据其中一个的物理页面号，计算出另一个的。比如一个compound page的起始pfn是8，order是1，那么它的其中一个buddy的pfn就是10。

free_one_page则是负责在一个single/compound page释放后，查询相邻的buddy是否是空闲的，如果是就需要合并成一个更大的compound page。合并可能会进行多次，直到不再满足合并的条件为止，在次过程中，"order"的值不断增大。



关于分配和释放这里的讲述并不是非常细致，主要还是对于Linux实现buddySystem的数据结构进行了剖w析。
