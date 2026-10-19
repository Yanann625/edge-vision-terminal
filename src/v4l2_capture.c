#include "v4l2_capture.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <time.h>

/* TODO(W3): 实现完整 V4L2 mmap 采集流程。
 *
 * 1. fd = open(device, O_RDWR)
 * 2. ioctl(fd, VIDIOC_QUERYCAP, &cap)
 * 3. fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; fmt.fmt.pix.width/height/pixelformat
 *    ioctl(fd, VIDIOC_S_FMT, &fmt)
 * 4. req.count = N; req.type = ...; req.memory = V4L2_MEMORY_MMAP
 *    ioctl(fd, VIDIOC_REQBUFS, &req)
 * 5. 对每个 i: ioctl(fd, VIDIOC_QUERYBUF, &buf); mmap(..., buf.length, ..., fd, buf.m.offset)
 * 6. ioctl(fd, VIDIOC_QBUF, &buf) 全部入队
 * 7. ioctl(fd, VIDIOC_STREAMON, &type)
 * 8. 循环：select(fd+1,&fds,NULL,NULL,&tv) -> DQBUF -> 构造 frame_t 推入 ringbuf -> QBUF
 */

int v4l2_open(v4l2_cap_t *c, const v4l2_cfg_t *cfg)
{
    (void)c; (void)cfg;
    fprintf(stderr, "[v4l2] TODO: open %s (W3 实现)\n", cfg ? cfg->device : "/dev/video0");
    return -1; /* 未实现：先用占位返回，W3 再填充 */
}

int v4l2_start(v4l2_cap_t *c) { (void)c; return -1; }

int v4l2_capture_loop(v4l2_cap_t *c, ringbuf_t *out, volatile int *running)
{
    (void)c; (void)out; (void)running;
    return -1;
}

void v4l2_stop(v4l2_cap_t *c) { (void)c; }
void v4l2_close(v4l2_cap_t *c) { (void)c; }
