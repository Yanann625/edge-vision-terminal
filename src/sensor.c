#include "sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* TODO(W1): 打开 /dev/i2c-1，ioctl(I2C_SLAVE, addr)，按传感器手册读寄存器。
 * 光照(BH1750) / 温湿度(BME280 / AHT20) 的读法规格书里写明。
 */

int sensor_init(sensor_ctx_t *s)
{
    (void)s;
    fprintf(stderr, "[sensor] TODO: open %s @0x%02x (W1 实现)\n",
            s->i2c_dev, s->addr);
    return -1;
}

int sensor_read(sensor_ctx_t *s, double *lux, double *temp, double *humi)
{
    (void)s;
    if (lux)  *lux = 0.0;
    if (temp) *temp = 0.0;
    if (humi) *humi = 0.0;
    return -1;
}

void *sensor_thread(void *arg)
{
    sensor_ctx_t *s = (sensor_ctx_t *)arg;
    fprintf(stderr, "[sensor] thread stub (W1 实现)\n");
    while (*s->running) {
        /* TODO: sensor_read -> 构造 frame_t(kind=1) -> ringbuf_push(s->out, ...) */
        usleep(s->interval_ms * 1000);
    }
    return NULL;
}
