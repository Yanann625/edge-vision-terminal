#!/usr/bin/env python3
"""
quantize_exp.py —— PC 端 INT8 量化实验（选做·AI 加分，规格书 ⑧）

流程：PyTorch 训练/加载模型 -> 导出 ONNX -> INT8 量化 -> 对比精度损失与推理速度。
目标：用数据回答"为什么这个模型跑不动 i.MX6ULL"。

依赖（建议建 venv）：
    pip install torch onnx onnxruntime onnxruntime-extensions numpy

本脚本为实验骨架，关键步骤以 TODO 标注，按需替换为你自己的模型与校准集。
"""
import time
import numpy as np


def export_onnx(model, dummy_input, onnx_path="model.onnx"):
    """TODO: 用 torch.onnx.export 导出 ONNX。"""
    print(f"[quant] TODO: export {onnx_path}")
    raise NotImplementedError


def quantize_int8(onnx_path="model.onnx", calib_data=None, out="model_int8.onnx"):
    """TODO: 用 onnxruntime 量化工具做 INT8 量化。
    需要校准集(calib_data)确定激活值范围。
    """
    print(f"[quant] TODO: INT8 quantize {onnx_path} -> {out}")
    raise NotImplementedError


def benchmark(onnx_path, input_shape, n=100):
    """在 PC 上测推理延迟，作为对比基线（注意：板端 ARM 会慢很多）。"""
    try:
        import onnxruntime as ort
    except ImportError:
        print("[quant] 未安装 onnxruntime，跳过基准")
        return None
    sess = ort.InferenceSession(onnx_path, providers=["CPUExecutionProvider"])
    in_name = sess.get_inputs()[0].name
    x = np.random.randn(*input_shape).astype(np.float32)
    # warmup
    for _ in range(5):
        sess.run(None, {in_name: x})
    t0 = time.perf_counter()
    for _ in range(n):
        sess.run(None, {in_name: x})
    avg_ms = (time.perf_counter() - t0) / n * 1000
    print(f"[quant] {onnx_path}: 平均推理 {avg_ms:.2f} ms/张 (PC CPU, n={n})")
    return avg_ms


def main():
    print("=== INT8 量化实验骨架 (W5 选做) ===")
    print("1) 导出 ONNX  2) INT8 量化  3) 精度对比  4) 速度对比")
    print("TODO: 填入你的模型、校准集与评价指标。")
    # benchmark("model.onnx", (1, 3, 224, 224))  # 取消注释可先跑 FP32 基线


if __name__ == "__main__":
    main()
