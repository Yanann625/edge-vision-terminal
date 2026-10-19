#include "pipeline.h"
#include "v4l2_capture.h"
#include "sensor.h"
#include "image_proc.h"
#include "net_report.h"
#include "perf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

app_config_t g_cfg = {
    .v4l2   = { .device = "/dev/video0", .width = 640, .height = 480,
                .pixelformat = 0 },  /* 0 由代码填 V4L2_PIX_FMT_YUYV */
    .sensor = { .i2c_dev = "/dev/i2c-1", .addr = 0x23, .interval_ms = 1000 },
    .cap2proc_cap = 8,
    .proc2net_cap = 8,
};

/* 处理线程：从 cap2proc 取帧，做 YUYV->灰度 + 帧差/亮度检测，
 * 结果(含延迟打点)推入 proc2net。 */
static void *process_thread(void *arg)
{
    pipeline_t *p = (pipeline_t *)arg;
    frame_t f;
    while (p->running) {
        ringbuf_pop(&p->cap2proc, &f);
        /* TODO(W3): image_proc 处理 + perf 打点 -> 构造结果 frame_t(kind=...) 推入 proc2net */
        (void)f;
    }
    return NULL;
}

/* 采集线程：并行跑 V4L2 与 I2C 传感器（可再拆两个线程，先合一个占位） */
static void *capture_thread(void *arg)
{
    pipeline_t *p = (pipeline_t *)arg;
    /* TODO(W1/W3): sensor_thread 与 v4l2_capture_loop 分别把帧推入 cap2proc */
    (void)p;
    return NULL;
}

int pipeline_init(pipeline_t *p)
{
    memset(p, 0, sizeof(*p));
    if (ringbuf_init(&p->cap2proc, g_cfg.cap2proc_cap) != 0) return -1;
    if (ringbuf_init(&p->proc2net, g_cfg.proc2net_cap) != 0) return -1;
    return 0;
}

int pipeline_start(pipeline_t *p)
{
    p->running = 1;
    /* TODO: 初始化 sensor_ctx.out = &p->cap2proc 后创建线程 */
    if (pthread_create(&p->tid_cap,  NULL, capture_thread, p) != 0) return -1;
    if (pthread_create(&p->tid_proc, NULL, process_thread, p) != 0) return -1;
    if (pthread_create(&p->tid_net,  NULL, report_thread,  p) != 0) return -1;
    return 0;
}

void pipeline_stop(pipeline_t *p)
{
    p->running = 0;
    /* 唤醒可能因缓冲空/满而阻塞的线程 */
    /* TODO: pthread_cond_broadcast 各 ringbuf 的条件变量 */
}

void pipeline_join(pipeline_t *p)
{
    if (p->tid_cap)  pthread_join(p->tid_cap,  NULL);
    if (p->tid_proc) pthread_join(p->tid_proc, NULL);
    if (p->tid_net)  pthread_join(p->tid_net,  NULL);
    ringbuf_destroy(&p->cap2proc);
    ringbuf_destroy(&p->proc2net);
}
