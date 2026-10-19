#ifndef RINGBUF_H
#define RINGBUF_H

#include <pthread.h>
#include <stddef.h>

/*
 * 跨线程传递的"一帧"数据。
 * 摄像头灰度帧、传感器采样包都用这同一个结构承载，
 * 这样采集线程 / 处理线程 / 上报线程能用同一套环形缓冲解耦。
 */
typedef struct {
    unsigned char *data;       /* 像素或采样数据缓冲（生产方分配，消费方复用/释放） */
    size_t         len;        /* data 的字节数 */
    unsigned int   width;      /* 宽（像素），传感器帧可忽略 */
    unsigned int   height;     /* 高（像素），传感器帧可忽略 */
    int            kind;       /* 0=图像帧 1=传感器包，处理线程据此分流 */
    long long      seq;        /* 帧序号，用于丢帧检测 */
    long long      ts_capture; /* 采集完成时间戳(ns, CLOCK_MONOTONIC) */
} frame_t;

typedef struct {
    frame_t      *buf;        /* 环形数组，capacity 个元素 */
    int           capacity;   /* 必须为 2 的幂 */
    int           head;       /* 下一个出队位置 */
    int           tail;       /* 下一个入队位置 */
    pthread_mutex_t lock;
    pthread_cond_t  not_empty;
    pthread_cond_t  not_full;
    /* 统计，便于 README / benchmark 展示 */
    unsigned long   produced;
    unsigned long   consumed;
    unsigned long   dropped;   /* 满时按策略丢弃的帧数 */
} ringbuf_t;

/*
 * 容量取 2 的幂，用位运算代替取模：
 *   下一位置 = (idx + 1) & (capacity - 1)
 * 比 % capacity 快，且不会有除法。
 */
#define RING_CAPACITY_MIN  4
#define RING_NEXT(idx, cap)  (((idx) + 1) & ((cap) - 1))

int  ringbuf_init(ringbuf_t *rb, int capacity);
void ringbuf_destroy(ringbuf_t *rb);
/* 入队：满时阻塞等待 not_full（实时场景可改成"丢弃最旧帧"策略） */
int  ringbuf_push(ringbuf_t *rb, const frame_t *f);
/* 出队：空时阻塞等待 not_empty */
int  ringbuf_pop(ringbuf_t *rb, frame_t *f);
void ringbuf_stats(const ringbuf_t *rb, unsigned long *prod,
                   unsigned long *cons, unsigned long *drop);

#endif /* RINGBUF_H */
