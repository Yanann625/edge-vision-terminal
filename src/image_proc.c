#include "image_proc.h"

void yuyv_to_gray(const unsigned char *src, unsigned char *dst,
                  unsigned int width, unsigned int height)
{
    unsigned int total = width * height; /* 像素数 */
    unsigned int i;
    for (i = 0; i < total; i += 2) {
        int y0 = src[i * 2 + 0];
        int u  = src[i * 2 + 1];
        int y1 = src[i * 2 + 2];
        int v  = src[i * 2 + 3];
        dst[i]     = (unsigned char)((77 * y0 + 150 * u + 29 * v) >> 8);
        dst[i + 1] = (unsigned char)((77 * y1 + 150 * u + 29 * v) >> 8);
    }
}

float frame_diff(const unsigned char *prev, const unsigned char *cur,
                 unsigned int n, int threshold)
{
    unsigned int i, moved = 0;
    for (i = 0; i < n; i++) {
        int d = cur[i] - prev[i];
        if (d < 0) d = -d;
        if (d > threshold) moved++;
    }
    return (float)moved / (float)n;
}

int brightness_check(const unsigned char *gray, unsigned int n,
                     int low_th, int high_th)
{
    if (n == 0) return 0;
    unsigned long sum = 0;
    unsigned int i;
    for (i = 0; i < n; i++) sum += gray[i];
    long avg = (long)(sum / n);
    return (avg < low_th || avg > high_th) ? 1 : 0;
}

void build_yuv_lut(int lut[256][3])
{
    int i;
    for (i = 0; i < 256; i++) {
        lut[i][0] = (77 * i) >> 8; /* Y 贡献 */
        lut[i][1] = (150 * i) >> 8; /* U 贡献 */
        lut[i][2] = (29 * i) >> 8; /* V 贡献 */
    }
}
