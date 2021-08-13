//
// Created by  Leo
//
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct buddy2 {
    unsigned size;//表明管理内存的总单元数
    unsigned longest[1]; //完全二叉树的节点标记，表明对应内存块的空闲单位
};

#define LEFT_LEAF(index) ((index) * 2 + 1)//左子树节点的值
#define RIGHT_LEAF(index) ((index) * 2 + 2)//右子树节点的值
#define PARENT(index) ( ((index) + 1) / 2 - 1)//父节点的值

#define IS_POWER_OF_2(x) (!((x)&((x)-1)))//判断x是不是2的幂
#define MAX(a, b) ((a) > (b) ? (a) : (b))//判断a，b大小

#define ALLOC malloc//申请内存
#define FREE free//释放内存

static unsigned fixsize(unsigned size) {//找到恰好合适的内存块，移位操作，知道就行
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    return size + 1;
}

struct buddy2 *buddy2_new(int size) {//初始化分配器
    struct buddy2 *self;
    unsigned node_size;//节点所拥有的内存大小
    int i;

    if (size < 1 || !IS_POWER_OF_2(size))
        return NULL;

    self = (struct buddy2 *) ALLOC(2 * size * sizeof(unsigned));
    self->size = size;
    node_size = size * 2;

    for (i = 0; i < 2 * size - 1; ++i) {//初始化每一个节点
        if (IS_POWER_OF_2(i + 1))
            node_size /= 2;
        self->longest[i] = node_size;
        //初始化操作，向表示满二叉树的数组赋值，例子：size为16 则赋值的数组为[16,8,8,4,4,4,4....]
        //表示节点对应的块的大小
    }
    return self;
}

void buddy2_destroy(struct buddy2 *self) {
    FREE(self);
}

/**
 * 寻找到合适的size,返回-1说明无法分配，分配失败，如果分配成功，会返回一个offset，代表分配的块在数组中的位置
 * @param self
 * @param size
 * @return
 */
int buddy2_alloc(struct buddy2 *self, int size) {
    unsigned index = 0;//节点的标号(数组下标)
    unsigned node_size;
    unsigned offset = 0;

    if (self == NULL)//无法分配
        return -1;

    if (size <= 0)//分配不合理
        size = 1;
    else if (!IS_POWER_OF_2(size))//不为2的幂时，取比size更大的2的n次幂
        size = fixsize(size);

    if (self->longest[index] < size)//可分配内存不足时，直接返回，否则，一定能找到合适的块
        return -1;

    for (node_size = self->size; node_size != size; node_size /= 2) {
        //从最大根节点开始向下寻找合适的节点, node_size表示该层节点所能代表最大块
        //到达层数所能代表的最大块等于要寻找的size的上一层为止  原因：继续找下去已经无意义
        //先找左子树
        if (self->longest[LEFT_LEAF(index)] >= size) {
            index = LEFT_LEAF(index);
        } else {
            index = RIGHT_LEAF(index);
        }
//        if (self->longest[LEFT_LEAF(index)] < size) {
//            index = RIGHT_LEAF(index);//如果左子树大小不够，可直接转向右子树
//        } else {
//            if (self->longest[RIGHT_LEAF(index) < self->longest[LEFT_LEAF(index)]]) {
//                //如果左子树小于右子树，我们优先选择小的分配（避免拆分大内存块造成外碎片浪费）
//                index = LEFT_LEAF(index);
//            } else {
//                //左子树大于右子树，如果右子树够大，则直接选择右子树
//                if (self->longest[RIGHT_LEAF(index)] >= size) {
//                    index = RIGHT_LEAF(index);
//                } else {
//                    index = LEFT_LEAF(index);
//                }
//            }
//        }

    }
    self->longest[index] = 0;//将节点标记为已使用
    offset = (index + 1) * node_size - self->size;//计算偏移量
    //向上刷新
    while (index) {
        index = PARENT(index);
        self->longest[index] =
                MAX(self->longest[LEFT_LEAF(index)], self->longest[RIGHT_LEAF(index)]);
        //刷新父节点；由于块分配，因此从根节点到该节点的块的大小都需要更新
    }

    return offset;
    //返回分配的块的offset，便于标志
}

/**
 * 释放掉数组位置为offset的块
 * @param self
 * @param offset
 */
void buddy2_free(struct buddy2 *self, int offset) {
    unsigned node_size, index = 0;
    unsigned left_longest, right_longest;

    assert(self && offset >= 0 && offset < self->size);//判断请求是否出错

    node_size = 1;
    index = offset + self->size - 1;

    for (; self->longest[index]; index = PARENT(index)) {//向上寻找分配过的节点
        node_size *= 2;
        if (index == 0)//如果节点不存在
            return;
    }

    self->longest[index] = node_size;//恢复内存

    while (index) {
        index = PARENT(index);
        node_size *= 2;

        left_longest = self->longest[LEFT_LEAF(index)];
        right_longest = self->longest[RIGHT_LEAF(index)];

        if (left_longest + right_longest == node_size) //如果可以则合并
            self->longest[index] = node_size;
        else
            self->longest[index] = MAX(left_longest, right_longest);

    }
}

//返回当前节点的内存大小
int buddy2_size(struct buddy2 *self, int offset) {
    unsigned node_size, index = 0;

    assert(self && offset >= 0 && offset < self->size);

    node_size = 1;
    for (index = offset + self->size - 1; self->longest[index]; index = PARENT(index))
        node_size *= 2;

    return node_size;
}

//打印内存状态，反应当前内存占用情况
void buddy2_dump(struct buddy2 *self) {
    char canvas[65];
    int i, j;
    unsigned node_size, offset;

    if (self == NULL) {
        printf("buddy2_dump: (struct buddy2*)self == NULL");
        return;
    }

    if (self->size > 64) {
        printf("buddy2_dump: (struct buddy2*)self is too big to dump");
        return;
    }

    memset(canvas, '_', sizeof(canvas));
    node_size = self->size * 2;

    for (i = 0; i < 2 * self->size - 1; ++i) {
        if (IS_POWER_OF_2(i + 1))
            node_size /= 2;

        if (self->longest[i] == 0) {
            if (i >= self->size - 1) {
                canvas[i - self->size + 1] = '*';
            } else if (self->longest[LEFT_LEAF(i)] && self->longest[RIGHT_LEAF(i)]) {
                offset = (i + 1) * node_size - self->size;

                for (j = offset; j < offset + node_size; ++j)
                    canvas[j] = '*';
            }
        }
    }
    canvas[self->size] = '\0';
    puts(canvas);
}



