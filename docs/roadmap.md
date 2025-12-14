# 📅 开发路线图 (Roadmap)

本项目的开发分为四个核心阶段，旨在构建工业级高性能推理引擎。每个阶段均设定了严格的、可复现的验收标准 (Definition of Done)。

**⚠️ 全生命周期强制产物**:
所有 Phase 结束时，必须提交以下产物至仓库，否则视为未完成：
1. **验收报告**: `reports/phaseX_*.md` (包含环境规格、复现命令、结果表格)。
2. **原始数据**: `benchmarks/*.csv` (测试原始数据)。
3. **可视化图表**: `figs/phaseX_*.png` (关键性能对比图)。

---

## Phase 0: 项目初始化与基建 (Initialization)
- [x] **项目脚手架**: 构建标准文件结构 (src/include/python/tests)。
- [x] **构建系统**: 配置 CMake + scikit-build-core + Conda 环境。
- [x] **质量保障**: 集成 GTest/Pytest，配置 clang-format 与 pre-commit。
- [x] **CI/CD**: 搭建 GitHub Actions 流水线 (Mamba 环境)。
- [x] **文档体系**: 初始化 README, Roadmap 及技术文档框架。

**✅ 交付物 (DoD)**:
1. **Green Build**: 可在任意 Linux 环境一键复现的 CI/CD 流水线。
2. **Integration Test**: "Hello World" 级别的 C++/Python 混合调用测试通过。

---

## Phase 1: 核心算子与 SIMD 加速 (Core Kernels)
- [x] **SIMD 基础设施**: 实现 `Strategy` 模式，支持 Scalar/AVX2 运行时切换。
- [x] **内存对齐**: 实现 `AlignedAllocator`，确保 AVX2 `load_ps` 安全。
- [x] **算子实现**:
  - [x] 余弦相似度 (Cosine Similarity)
  - [x] 向量归一化 (L2 Normalize)
- [x] **基准测试工具**: 构建标准化的 Benchmark Suite。

**✅ 交付物 (DoD)**:
1. **Benchmark 规范**:
   - **编译**: `-O3 -march=native -fno-tree-vectorize` (禁用自动向量化以公平对比)。
   - **计时**: Warmup 100 次 + Loop 1000 次 (取 Median)。
   - **场景**: Dim=768/1024, Batch=1/32/256。
2. **性能报告 (`reports/phase1_benchmark.md`)**: 
   - 目标：AVX2 相比 Naive C++ Loop 提升 **≥ 3x** (Batch≥256)。
   - 产出：CSV 数据 + Matplotlib 对比图。
3. **正确性验证**: GTest 100% 通过，覆盖非对齐长度边界。

---

## Phase 2: 推理引擎集成 (Inference Engine)
- [ ] **ONNX Runtime 集成**: 封装 ORT C++ API，支持 `distilbert-base-uncased`。
- [ ] **数据流优化**: Python (Tokenizer) -> Int64 Tensors -> C++ (Inference) -> Pooling -> Norm。
- [ ] **Zero-Copy Binding**: 重点优化 `Tokenizer Output -> C++` 和 `C++ Internal` 的内存复用。
- [ ] **线程安全**: 实现 RAII 封装的 Session Pool。

**✅ 交付物 (DoD)**:
1. **完整推理类**: `FlashEmbedder` Python 类，接口 `.encode(text_list)`。
2. **精度报告 (`reports/phase2_accuracy.md`)**:
   - 对比对象：PyTorch 原生推理 vs C++ ORT 推理。
   - 阈值：Max Abs Error < `1e-4`，Cosine Similarity Diff < `1e-5`。
3. **内存分析**: 证明在 Contiguous Input 下，Python 到 C++ 无额外 Tensor Copy。

---

## Phase 3: 服务化与全链路优化 (Service & Pipeline)
- [ ] **FastAPI 服务**: 搭建 HTTP 接口，接入 HuggingFace Tokenizer。
- [ ] **动态批处理 (Dynamic Batching)**: 实现请求合并策略 (参数: `max_batch_size`, `max_wait_ms`)。
- [ ] **性能调优**: 解决 ORT 线程池与 HTTP Server 的资源竞争 (Affinity/Oversubscription)。
- [ ] **全链路压测**: 使用 Locust 进行真实负载测试。

**✅ 交付物 (DoD)**:
1. **压测报告 (`reports/phase3_loadtest.md`)**:
   - **场景**: SeqLen=128, Concurrency=32。
   - **对照组**: 
     1. Baseline A: FastAPI + PyTorch。
     2. Baseline B: FastAPI + ORT Python API。
   - **目标**: 相比 Baseline B，QPS 提升 **>30%**；相比 Baseline A 提升 **>50%**。
   - **SLA**: 在 200 QPS 下，**P99 < 50ms**。
2. **Docker 镜像**: 包含完整环境，一键启动。
3. **API 文档**: Swagger UI 可用。
