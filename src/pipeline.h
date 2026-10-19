#ifndef PIPELINE_H
#define PIPELINE_H

#include <pthread.h>
#include "ringbuf.h"
#include "v4l2_capture.h"
#include "sensor.h"

/*
 * 三线程 pipeline 编排：
 *   采集线程(capture_thread) -> cap2proc 环形缓冲
 *   处理线程(process_thread)  -> proc2net 环形缓冲
 *   上报线程(report_thread)
 * 采集线程内部再细分 USB 摄像头(V4L2) 与 I2C 传感器两条来源。
 */
typedef struct {
    ringbuf_t      cap2proc;     /* 采集 -> 处理 */
    ringbuf_t      proc2net;     /* 处理 -> 上报 */
    pthread_t      tid_cap;
    pthread_t      tid_proc;
    pthread_t      tid_net;
    volatile int   running;
} pipeline_t;

int  pipeline_init(pipeline_t *p);
int  pipeline_start(pipeline_t *p);
void pipeline_stop(pipeline_t *p);
void pipeline_join(pipeline_t *p);

/* 供各线程引用的全局配置在此集中（默认值见 pipeline.c） */
typedef struct {
    v4l2_cfg_t  v4l2;
    sensor_ctx_t sensor;
    int         cap2proc_cap;   /* 环形缓冲容量（2 的幂） */
    int         proc2net_cap;
} app_config_t;

extern app_config_t g_cfg;

#endif /* PIPELINE_H */
