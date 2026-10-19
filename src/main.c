/*
 * 边缘视觉采集终端 —— 程序入口
 * 职责：初始化 -> 注册信号处理 -> 启动三线程 pipeline -> 看门狗喂狗 -> 优雅退出
 *
 * 编译： make                (本地 PC 验证)
 *        make CC=arm-linux-gnueabihf-gcc  (交叉编译到 i.MX6ULL)
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pipeline.h"
#include "watchdog.h"
#include "perf.h"

int main(void)
{
    pipeline_t pl;
    int wd = -1;

    signal_setup(&pl.running);   /* 捕获 SIGTERM/SIGINT -> running=0 */

    if (pipeline_init(&pl) != 0) {
        fprintf(stderr, "pipeline init failed\n");
        return 1;
    }
    if (pipeline_start(&pl) != 0) {
        fprintf(stderr, "pipeline start failed\n");
        return 1;
    }

    /* 看门狗：板端启用；PC 上 /dev/watchdog 不存在会自动跳过 */
    wd = watchdog_open(10);
    while (pl.running) {
        if (wd >= 0) watchdog_feed(wd);
        /* TODO(W4): 周期性打印 perf 统计（均值/P95/CPU/内存） */
        sleep(5);
    }

    /* 优雅退出 */
    pipeline_stop(&pl);
    pipeline_join(&pl);
    graceful_shutdown();
    if (wd >= 0) watchdog_close(wd);

    printf("edge-vision terminal stopped.\n");
    return 0;
}
