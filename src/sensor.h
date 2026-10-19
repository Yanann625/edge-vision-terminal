#ifndef SENSOR_H
#define SENSOR_H

#include "ringbuf.h"

/*
 * I2C 传感器采集（光照 / 温湿度），周期采样。
 * 这是 W1 先跑通的部分——复用你 MCU 实习的 I2C 经验，快速建立正反馈。
 * 采样结果封装为 frame_t(kind=1) 推入 ringbuf。
 */
typedef struct {
    const char  *i2c_dev;  /* 如 /dev/i2c-1 */
    int          addr;     /* 从机地址 */
    int          interval_ms;
    ringbuf_t   *out;
    volatile int *running;
} sensor_ctx_t;

int  sensor_init(sensor_ctx_t *s);
/* 读一次传感器，输出光照(lux)/温度(℃)/湿度(%)。返回 0 成功。 */
int  sensor_read(sensor_ctx_t *s, double *lux, double *temp, double *humi);
/* 周期采集线程入口 */
void *sensor_thread(void *arg);

#endif /* SENSOR_H */
