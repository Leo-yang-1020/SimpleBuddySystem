//
// Created by 50633 on 2021/8/12.
//

#ifndef __BUDDY2_H__
#define __BUDDY2_H__

#include <stdlib.h>

struct buddy2;
/**
 * 创建伙伴系统，size必须是2的幂
 * @param size
 * @return
 */
struct buddy2* buddy2_new( int size );
/**
 * 销毁
 * @param self
 */
void buddy2_destroy( struct buddy2* self );
/**
 * 伙伴系统分配算法，分配一个适合size的块
 * @param self
 * @param size
 * @return
 */
int buddy2_alloc(struct buddy2* self, int size);
/**
 * 释放对应offset的块，并且实现紧凑
 * @param self
 * @param offset
 */
void buddy2_free(struct buddy2* self, int offset);

int buddy2_size(struct buddy2* self, int offset);
void buddy2_dump(struct buddy2* self);

#endif//__BUDDY2_H__
