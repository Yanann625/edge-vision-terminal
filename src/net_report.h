#ifndef NET_REPORT_H
#define NET_REPORT_H

#include "ringbuf.h"
#include "pipeline.h"

/*
 * 网络上报：TCP socket 上报 JSON 结果。
 * - 指数退避重连：1, 2, 4, 8, 16, 32 秒
 * - 断网期间数据写入本地文件队列，恢复后按序补传（不丢数据）
 */
typedef struct {
    const char *host;
    int         port;
    int         max_retry_delay; /* 退避上限秒，默认 32 */
} net_cfg_t;

/* 建立 TCP 连接，失败按指数退避重试，直到成功或超上限 */
int  net_connect(const net_cfg_t *cfg);
/* 上报一条 JSON 字符串；返回 0 成功 */
int  net_report_json(int sock, const char *json);
/* 上报线程入口：从 pipeline->proc2net 取处理结果并上报 */
void *report_thread(void *arg);

#endif /* NET_REPORT_H */
