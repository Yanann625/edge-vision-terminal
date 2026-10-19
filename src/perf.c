#include "perf.h"
#include <stdio.h>
#include <stdlib.h>

long long ts_ns(const struct timespec *ts)
{
    return (long long)ts->tv_sec * 1000000000LL + ts->tv_nsec;
}

long long stage_us(const struct timespec *a, const struct timespec *b)
{
    return (ts_ns(b) - ts_ns(a)) / 1000LL;
}

void perf_stat_init(perf_stat_t *s, int cap)
{
    s->cap = cap > 0 ? cap : 1024;
    s->samples = (long long *)calloc(s->cap, sizeof(long long));
    s->n = s->head = 0;
    s->sum = 0;
}

void perf_stat_add(perf_stat_t *s, long long us)
{
    if (s->n < s->cap) {
        s->samples[s->n++] = us;
        s->sum += us;
    } else {
        s->sum -= s->samples[s->head];
        s->samples[s->head] = us;
        s->sum += us;
        s->head = (s->head + 1) % s->cap;
    }
}

static int cmp_ll(const void *a, const void *b)
{
    long long x = *(const long long *)a, y = *(const long long *)b;
    return (x > y) - (x < y);
}

void perf_stat_report(const perf_stat_t *s, long long *mean,
                      long long *p95, long long *p99, long long *max)
{
    if (s->n == 0) {
        if (mean) *mean = 0; if (p95) *p95 = 0;
        if (p99) *p99 = 0;  if (max) *max = 0;
        return;
    }
    long long *sorted = (long long *)malloc(sizeof(long long) * s->n);
    int i, k;
    for (i = 0, k = s->head; i < s->n; i++, k = (k + 1) % s->cap)
        sorted[i] = s->samples[k];
    qsort(sorted, s->n, sizeof(long long), cmp_ll);

    if (mean) *mean = s->sum / s->n;
    if (p95)  *p95  = sorted[(int)(0.95 * (s->n - 1))];
    if (p99)  *p99  = sorted[(int)(0.99 * (s->n - 1))];
    if (max)  *max  = sorted[s->n - 1];
    free(sorted);
}

double cpu_usage_percent(void) { return 0.0; } /* TODO: 读 /proc/self/stat */
long   mem_rss_kb(void)        { return 0;   } /* TODO: 读 /proc/self/status */
