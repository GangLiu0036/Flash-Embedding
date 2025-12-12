**项目名称：** Flash-Embedding —— 基于C++加速的高性能向量化推理与评测系统
**技术栈：** C++11/14/17/20, Python (FastAPI, PyTest), OpenMP, SIMD (AVX2), ONNX Runtime,  Docker

**项目描述**： 针对传统 Python 推理服务在高并发场景下的 GIL 瓶颈与高延迟问题，设计并实现了一个 C++ Native Extension 架构的高性能向量化推理系统。项目采用 C++ 14 重构核心计算链路，通过 Pybind11 提供 Python 接口，实现了比原生 PyTorch 方案 xxx 的 QPS 提升。

**核心职责：**
**C++ 底层优化与内存管理 (AI Infra)**：

- 基于 Pybind11 实现了 Numpy 到 C++ 的 Zero-Copy (零拷贝) 数据传输机制，避免了高频推理场景下百万级向量传输的内存拷贝开销。
- 采用 Strategy Pattern (策略模式) 设计算子库，支持在运行时自动检测 CPU 指令集并切换至 AVX2 (SIMD) 加速内核，将余弦相似度计算延迟降低 40%（根据实测数据修改）。
- 利用 RAII 机制与智能指针管理推理引擎生命周期，彻底杜绝了 C++ 扩展中的内存泄漏风险。

**模型推理与微服务架构 (System Design)**：

- 集成 ONNX Runtime (C++ API) 作为推理后端，通过 OpenMP 实现算子级并行，突破了 Python GIL 对多核 CPU 的限制。
- 搭建基于 FastAPI + Docker 的微服务容器，封装了“Tokenization -> Inference -> Pooling”的完整 Pipeline，支持动态 Batching 处理。

**全链路自动化测试 (Engineering Excellence)**：

- 构建 A/B Test 性能基准平台，输出纯 Python 版与 C++ 加速版的 Latency/Throughput 对比热力图。
- 设计 精度对齐测试 (Consistency Test)，确保 C++ 算子与 PyTorch 原生算子的计算误差严格控制在 xxx 以内（Float32 精度）。
- 编写 Locust 压测脚本模拟高并发流量，验证了系统在持续高负载下的稳定性。

注：xxx 表示性能量化指标的占位字符串，项目完成后填入具体数值
