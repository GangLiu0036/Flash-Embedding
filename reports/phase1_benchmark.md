# Phase 1: 核心算子性能验收报告

**验收日期**: 2025-12-14
**环境信息**:
- CPU: AMD R7-9700x 16 Core 5582.3 MHz
- Caches: L1 48K / L2 1024K / L3 32MB
- Compiler: GCC 15.2.0 (Flags: `-O3 -mavx2 -mfma -fno-tree-vectorize`)

## 1. 性能对比概览 (Cosine Similarity)

| 维度 (Dim) | Scalar (ns) | AVX2 (ns) | 加速比 (Speedup) | 备注 |
| :--- | :--- | :--- | :--- | :--- |
| **768** (BERT) | 311 | 64.6 | **4.81x** | 远超初步设定的 3x 目标 |
| **1024** (Large) | 416 | 87.5 | **4.75x** | 性能线性扩展 |
| **4096** (LLM) | 1657 | 363 | **4.56x** | 受限于内存带宽 |
| **769** (非对齐) | 313 | 64.9 | **4.82x** | 尾部处理无额外开销 |

## 2. 结论
1.  **DoD 达成**: 所有测试用例下加速比均超过 4.5x，远超 Roadmap 设定的 3x 阈值。
2.  **边界稳健**: 对于非 8 倍数长度 (769)，AVX2 实现的 Peeling 逻辑工作正常，未见性能回退。
3.  **内存对齐**: `AlignedAllocator` 有效保证了 32 字节对齐，`_mm256_load_ps` 未触发 SegFault。

## 3. 复现命令
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --target cpp_benchmarks
./build/cpp_benchmarks
```

详见原始数据: [benchmarks/phase1_result.csv](../benchmarks/phase1_result.csv)
