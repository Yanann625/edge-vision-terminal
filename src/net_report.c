#include "net_report.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int net_connect(const net_cfg_t *cfg)
{
    int retry = 1;
    while (retry <= cfg->max_retry_delay) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        /* TODO: 填充 sockaddr_in，connect；失败关闭并 sleep(retry)，retry *= 2 */
        (void)sock;
        fprintf(stderr, "[net] TODO: connect %s:%d (W5 实现)\n", cfg->host, cfg->port);
        return -1;
    }
    return -1;
}

int net_report_json(int sock, const char *json)
{
    (void)sock; (void)json;
    return -1; /* TODO(W5): send() JSON，处理部分写与断线 */
}

void *report_thread(void *arg)
{
    pipeline_t *p = (pipeline_t *)arg;
    frame_t f;
    while (p->running) {
        ringbuf_pop(&p->proc2net, &f);
        /* TODO(W5): 构造 JSON -> net_connect/net_report_json -> 断网落本地队列补传 */
        (void)f;
    }
    return NULL;
}
