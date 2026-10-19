#include "ringbuf.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int ringbuf_init(ringbuf_t *rb, int capacity)
{
    /* capacity 必须 >= RING_CAPACITY_MIN 且为 2 的幂 */
    if (capacity < RING_CAPACITY_MIN)
        capacity = RING_CAPACITY_MIN;
    if ((capacity & (capacity - 1)) != 0) {
        /* 向上取整到最近的 2 的幂 */
        int c = 1;
        while (c < capacity) c <<= 1;
        capacity = c;
    }

    rb->buf = (frame_t *)calloc(capacity, sizeof(frame_t));
    if (!rb->buf)
        return -1;

    rb->capacity = capacity;
    rb->head = rb->tail = 0;
    rb->produced = rb->consumed = rb->dropped = 0;
    pthread_mutex_init(&rb->lock, NULL);
    pthread_cond_init(&rb->not_empty, NULL);
    pthread_cond_init(&rb->not_full, NULL);
    return 0;
}

void ringbuf_destroy(ringbuf_t *rb)
{
    if (!rb)
        return;
    /* TODO: 释放 buf[i].data 中尚未出队的数据缓冲 */
    free(rb->buf);
    rb->buf = NULL;
    pthread_mutex_destroy(&rb->lock);
    pthread_cond_destroy(&rb->not_empty);
    pthread_cond_destroy(&rb->not_full);
}

int ringbuf_push(ringbuf_t *rb, const frame_t *f)
{
    pthread_mutex_lock(&rb->lock);
    while (((rb->tail - rb->head) & (rb->capacity - 1)) == (rb->capacity - 1)) {
        /* 满：默认阻塞等待。实时场景可改为丢弃最旧帧（TODO）。 */
        pthread_cond_wait(&rb->not_full, &rb->lock);
    }
    rb->buf[rb->tail] = *f;
    rb->tail = RING_NEXT(rb->tail, rb->capacity);
    rb->produced++;
    pthread_cond_signal(&rb->not_empty);
    pthread_mutex_unlock(&rb->lock);
    return 0;
}

int ringbuf_pop(ringbuf_t *rb, frame_t *f)
{
    pthread_mutex_lock(&rb->lock);
    while (rb->head == rb->tail)
        pthread_cond_wait(&rb->not_empty, &rb->lock);
    *f = rb->buf[rb->head];
    /* 出队后把该槽位的 data 指针置空，避免重复释放 */
    rb->buf[rb->head].data = NULL;
    rb->head = RING_NEXT(rb->head, rb->capacity);
    rb->consumed++;
    pthread_cond_signal(&rb->not_full);
    pthread_mutex_unlock(&rb->lock);
    return 0;
}

void ringbuf_stats(const ringbuf_t *rb, unsigned long *prod,
                   unsigned long *cons, unsigned long *drop)
{
    if (prod) *prod = rb->produced;
    if (cons) *cons = rb->consumed;
    if (drop) *drop = rb->dropped;
}
