# 📅 开发路线图 (Roadmap)

本项目的开发分为三个核心阶段，旨在逐步构建高性能的推理引擎。

## Phase 0: 项目初始化与基建 (Initialization)
- [x] **项目脚手架**: 构建标准文件结构 (src/include/python/tests)。
- [x] **构建系统**: 配置 CMake + scikit-build-core + Conda 环境。
- [x] **质量保障**: 集成 GTest/Pytest，配置 clang-format 与 pre-commit。
- [x] **CI/CD**: 搭建 GitHub Actions 流水线 (Mamba 环境)。
- [x] **文档体系**: 初始化 README, Roadmap 及技术文档框架。

## Phase 1: 核心算子与 SIMD 加速 (Core Kernels)
- [ ] **SIMD 基础设施**: 实现 `Strategy` 模式，支持 Scalar/AVX2 运行时切换。
- [ ] **内存对齐**: 实现 `AlignedAllocator`，确保 AVX2 `load_ps` 安全。
- [ ] **算子实现**:
  - [ ] 余弦相似度 (Cosine Similarity)
  - [ ] 向量归一化 (L2 Normalize)
- [ ] **基准测试**: C++ GTest 验证精度，Benchmark 对比 Scalar vs AVX2 性能。

## Phase 2: 推理引擎集成 (Inference Engine)
- [ ] **ONNX Runtime 集成**: 封装 ORT C++ API，实现模型加载与推理 Session 管理。
- [ ] **Zero-Copy Binding**: 优化 Pybind11 接口，实现 Python -> C++ 的零拷贝张量传输。
- [ ] **线程安全**: 实现 RAII 封装的 Session Pool，支持多线程并发推理。
- [ ] **端到端测试**: 跑通 Python (Input) -> C++ (Inference) -> Python (Output) 完整链路。

## Phase 3: 服务化与全链路优化 (Service & Pipeline)
- [ ] **FastAPI 服务**: 搭建 HTTP 接口，接入 Tokenizer 和 Embedding 模型。
- [ ] **动态批处理 (Dynamic Batching)**: 实现请求合并策略，提升高并发吞吐量。
- [ ] **性能调优**:
  - [ ] OpenMP 线程绑定与亲和性设置。
  - [ ] 解决 ORT 与 OpenMP 的线程争抢问题。
- [ ] **压力测试**: 使用 Locust 进行全链路压测，输出最终性能报告。
