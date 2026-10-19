#ifndef PERF_H
#define PERF_H

#include <time.h>

/*
 * 延迟测量与性能统计——所有优化的前提是先能测。
 * 分段打点：采集开始/完成 -> 处理完成 -> 上报完成。
 * 统计均值 / P95 / P99 / 最大值 / 抖动。
 */

typedef struct {
    struct timespec t_capture_start;
    struct timespec t_capture_done;  /* 采集完成 -> 入队 */
    struct timespec t_proc_done;     /* 处理完成 */
    struct timespec t_report_done;   /* 上报完成 */
} perf_stage_t;

long long ts_ns(const struct timespec *ts);              /* 转纳秒 */
long long stage_us(const struct timespec *a,
                   const struct timespec *b);            /* 阶段耗时(us) */

/* 滑动窗口统计器 */
typedef struct {
    long long *samples;
    int        cap, n, head;
    long long  sum;
} perf_stat_t;

void perf_stat_init(perf_stat_t *s, int cap);
void perf_stat_add(perf_stat_t *s, long long us);
/* 输出均值/P95/P99/max（us） */
void perf_stat_report(const perf_stat_t *s, long long *mean,
                      long long *p95, long long *p99, long long *max);

/* CPU 占用：读 /proc/self/stat 的 utime+stime，两次采样求差 */
double cpu_usage_percent(void);
/* 内存：读 /proc/self/status 的 VmRSS(KB) */
long   mem_rss_kb(void);

#endif /* PERF_H */
