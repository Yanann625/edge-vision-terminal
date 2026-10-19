#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H

#include <stddef.h>

/*
 * 图像处理：自己用 C 写，不用 OpenCV（i.MX6ULL 上 OpenCV 太重）。
 * W3 先实现基础版本，W4 针对性优化（查表法 / 降采样 / ROI / 缓冲复用）。
 */

/* YUYV(packed) -> 灰度。整数化系数 Gray = (77*Y + 150*U + 29*V) >> 8
 * YUYV 每 4 字节表示 2 像素：[Y0 U Y1 V] */
void yuyv_to_gray(const unsigned char *src, unsigned char *dst,
                  unsigned int width, unsigned int height);

/* 帧差法移动侦测：返回运动像素比例 [0,1] */
float frame_diff(const unsigned char *prev, const unsigned char *cur,
                 unsigned int n, int threshold);

/* 亮度异常检测：返回 1 表示异常（过暗/过亮） */
int brightness_check(const unsigned char *gray, unsigned int n,
                    int low_th, int high_th);

/* W4 优化：预计算 Y/U/V 贡献表，把乘法换成查表 */
void build_yuv_lut(int lut[256][3]); /* 输出 3 张 0..255 的查表 */

#endif /* IMAGE_PROC_H */
