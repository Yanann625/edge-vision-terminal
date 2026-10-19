#!/usr/bin/env python3
"""
perf_plot.py —— 性能数据可视化

把 edge-vision 导出的 perf 采样（CSV）画成图，便于放进 README / benchmark.md。

输入 CSV 格式（由 perf.c 在板端/PC 端 dump，每行一帧，单位 us）：
    seq,cap_us,proc_us,report_us,e2e_us
例如：
    1,1200,8500,300,10000
    2,1180,8600,310,10090

用法：
    python3 perf_plot.py perf_before.csv --label before
    python3 perf_plot.py perf_after.csv  --label after --save e2e.png

依赖：pip install matplotlib pandas
"""
import argparse
import sys
import matplotlib
matplotlib.use("Agg")  # 无显示环境也能出图
import matplotlib.pyplot as plt
import pandas as pd


def load(path):
    try:
        return pd.read_csv(path)
    except Exception as e:  # noqa
        print(f"[perf_plot] 无法读取 {path}: {e}", file=sys.stderr)
        sys.exit(1)


def plot(df, label, save=None, show=False):
    fig, ax = plt.subplots(1, 2, figsize=(12, 4.5))

    # 左：端到端延迟时序
    ax[0].plot(df["seq"], df["e2e_us"], label=label, alpha=0.8)
    ax[0].set_title(f"端到端延迟时序 ({label})")
    ax[0].set_xlabel("帧序号")
    ax[0].set_ylabel("延迟 (us)")
    ax[0].grid(True, alpha=0.3)

    # 右：各阶段占比堆叠
    stages = ["cap_us", "proc_us", "report_us"]
    bottom = pd.Series(0, index=df.index)
    colors = ["#2b5ce6", "#0d9488", "#d97706"]
    for s, c in zip(stages, colors):
        ax[1].plot(df["seq"], df[s], label=s, color=c, alpha=0.7)
        bottom += df[s]
    ax[1].set_title(f"各阶段耗时 ({label})")
    ax[1].set_xlabel("帧序号")
    ax[1].set_ylabel("耗时 (us)")
    ax[1].legend()
    ax[1].grid(True, alpha=0.3)

    mean = df["e2e_us"].mean()
    p95 = df["e2e_us"].quantile(0.95)
    mx = df["e2e_us"].max()
    fig.suptitle(f"{label}: 均值={mean:.0f}us  P95={p95:.0f}us  最大={mx:.0f}us")

    if save:
        fig.savefig(save, dpi=120, bbox_inches="tight")
        print(f"[perf_plot] 已保存 {save}")
    if show:
        plt.show()


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("csv", help="perf 采样 CSV")
    ap.add_argument("--label", default="data")
    ap.add_argument("--save", help="保存图片路径")
    ap.add_argument("--show", action="store_true")
    args = ap.parse_args()
    df = load(args.csv)
    plot(df, args.label, save=args.save, show=args.show)


if __name__ == "__main__":
    main()
