#ifndef WATCHDOG_H
#define WATCHDOG_H

/*
 * 系统可靠性：看门狗 + 优雅关机。
 * 大部分应届生项目没有这一层，是你的差异化亮点（对口系统软件/车载岗）。
 */
int  watchdog_open(int timeout_sec);   /* 打开 /dev/watchdog 并设置超时 */
void watchdog_feed(int fd);            /* ioctl(WDIOC_KEEPALIVE) 喂狗 */
void watchdog_close(int fd);           /* 关闭（注意：部分硬件会触发复位） */

/* 注册 SIGTERM/SIGINT 处理，置 running=0 触发优雅退出 */
void signal_setup(volatile int *running);

/* 优雅退出六步（main 退出时调用）：
 * 1. running=0 通知各线程  2. pthread_join 等待
 * 3. VIDIOC_STREAMOFF      4. munmap 释放
 * 5. flush 未上报数据      6. close 所有 fd */
void graceful_shutdown(void);

#endif /* WATCHDOG_H */
