#ifndef V4L2_CAPTURE_H
#define V4L2_CAPTURE_H

#include "ringbuf.h"

/*
 * V4L2 摄像头采集模块（USB 摄像头，YUYV 格式，mmap 零拷贝）。
 * 关键流程：open -> QUERYCAP -> S_FMT -> REQBUFS -> QUERYBUF -> mmap
 *           -> QBUF -> STREAMON -> (select 等帧 -> DQBUF -> 处理 -> QBUF)
 */
typedef struct {
    const char  *device;      /* 默认 /dev/video0 */
    unsigned int width;
    unsigned int height;
    unsigned int pixelformat; /* 默认 V4L2_PIX_FMT_YUYV */
} v4l2_cfg_t;

typedef struct {
    int          fd;
    void        *mmap_base;   /* mmap 映射起始地址（多缓冲连续存放） */
    size_t       mmap_len;
    struct v4l2_buffer *buffers; /* 每个内核缓冲的元数据 */
    unsigned int n_buffers;
} v4l2_cap_t;

int  v4l2_open(v4l2_cap_t *c, const v4l2_cfg_t *cfg);
int  v4l2_start(v4l2_cap_t *c);
/*
 * 采集循环（在采集线程内调用）。
 * 每拿到一帧：用 mmap 地址构造 frame_t 推入 out 环形缓冲。
 * 注意：推入的是"当前内核缓冲的快照"，用完需 QBUF 归还。
 */
int  v4l2_capture_loop(v4l2_cap_t *c, ringbuf_t *out, volatile int *running);
void v4l2_stop(v4l2_cap_t *c);
void v4l2_close(v4l2_cap_t *c);

#endif /* V4L2_CAPTURE_H */
