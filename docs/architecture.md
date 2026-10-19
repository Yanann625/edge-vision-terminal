# 架构说明与数据流

本文档补充 README 中的架构图，给出组件职责、线程模型与关键数据结构。

## 1. 整体数据流

```
          ┌─────────────┐         ┌─────────────┐
  I2C     │  传感器采集   │         │  V4L2 摄像头 │  USB
  总线 ──▶│ sensor.c     │         │ v4l2_cap.c  │◀── 摄像头
          └──────┬──────┘         └──────┬──────┘
                 │  frame_t(kind=1)      │  frame_t(kind=0)
                 └──────────┬────────────┘
                            ▼
                    ┌───────────────────┐
                    │  cap2proc 环形缓冲 │   (互斥锁 + 两个条件变量)
                    └─────────┬─────────┘
                              ▼
                    ┌───────────────────┐
                    │   处理线程         │  image_proc.c
                    │ YUYV→灰度/帧差检测 │  perf.c 分段打点
                    └─────────┬─────────┘
                              ▼
                    ┌───────────────────┐
                    │  proc2net 环形缓冲 │
                    └─────────┬─────────┘
                              ▼
                    ┌───────────────────┐
                    │   上报线程         │  net_report.c
                    │ TCP JSON + 重连    │  断网落本地队列补传
                    └─────────┬─────────┘
                              ▼
                          服务端 / 监控

   主循环：signal_setup → pipeline_start → 周期性 watchdog_feed + 打印 perf
```

## 2. 线程模型

| 线程 | 入口 | 职责 | 产出 |
|------|------|------|------|
| 采集线程 | `capture_thread` | 跑 V4L2 与 I2C 两条采集源，推 `cap2proc` | frame_t |
| 处理线程 | `process_thread` | 格式转换 + 检测 + 延迟打点，推 `proc2net` | 结果帧 |
| 上报线程 | `report_thread` | socket 上报 JSON，断线重连与补传 | 网络 I/O |

单生产者单消费者场景可讨论无锁实现（C11 `_Atomic`），但要能说清内存序风险。

## 3. 关键数据结构

### frame_t（跨线程传递单位）
```c
typedef struct {
    unsigned char *data;
    size_t         len;
    unsigned int   width, height;
    int            kind;     /* 0=图像帧 1=传感器包 */
    long long      seq;
    long long      ts_capture; /* CLOCK_MONOTONIC ns */
} frame_t;
```

### ringbuf_t（2^n 容量 + 位运算）
- `capacity` 取 2 的幂，用 `(idx+1) & (capacity-1)` 代替取模
- 互斥锁 + `not_empty`/`not_full` 两个条件变量
- 满时默认阻塞等待（实时场景可改为丢弃最旧帧）

## 4. 延迟打点分段（perf.c）

```
t_capture_start ─▶ t_capture_done ─▶ t_proc_done ─▶ t_report_done
      采集耗时          处理耗时          上报耗时
```

统计：均值 / P95 / P99 / 最大值 / 抖动；CPU 读 `/proc/self/stat`，内存读 `/proc/self/status` 的 VmRSS。

## 5. 优雅退出顺序

1. `running = 0` 通知各线程
2. `pthread_join` 等待线程结束
3. `VIDIOC_STREAMOFF` 停止采集
4. `munmap` 解除映射、释放缓冲
5. flush 未上报数据到磁盘
6. close 所有 fd，退出
