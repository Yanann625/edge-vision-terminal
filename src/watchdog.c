#include "watchdog.h"
#include <stdio.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

int watchdog_open(int timeout_sec)
{
    int fd = open("/dev/watchdog", O_WRONLY);
    if (fd < 0) {
        fprintf(stderr, "[wd] 打开 /dev/watchdog 失败（PC 上正常，板端启用）\n");
        return -1;
    }
    ioctl(fd, WDIOC_SETTIMEOUT, &timeout_sec);
    return fd;
}

void watchdog_feed(int fd)
{
    ioctl(fd, WDIOC_KEEPALIVE, 0);
}

void watchdog_close(int fd)
{
    /* MAGIC 关闭避免硬件复位：write "V" 再 close */
    if (fd >= 0) {
        write(fd, "V", 1);
        close(fd);
    }
}

static volatile int *g_running;

static void on_signal(int sig)
{
    (void)sig;
    if (g_running) *g_running = 0;
}

void signal_setup(volatile int *running)
{
    g_running = running;
    struct sigaction sa;
    sa.sa_handler = on_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT,  &sa, NULL);
}

void graceful_shutdown(void)
{
    /* TODO: STREAMOFF / munmap / flush / close，详见 watchdog.h 注释 */
}
