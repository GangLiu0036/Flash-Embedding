# 🛠️ 开发日志 & 难点攻克 (Engineering Challenges)

本日志用于记录开发过程中遇到的底层系统编程难题及其解决方案，重点关注性能瓶颈与稳定性问题。

## Phase 0: 项目初始化与基建

### 1. 混合构建系统的路径映射
> **问题**: 使用 `scikit-build-core` 时，CMake 的 `install(TARGETS)` 默认路径与 Python 包结构不匹配，导致 `ModuleNotFoundError`。
> **分析**: `scikit-build-core` 期望编译产物直接位于包目录内，而 CMake 默认安装到 `lib/` 或根目录。
> **解决**: 修改 `CMakeLists.txt` 的安装目标为 `install(TARGETS _core DESTINATION flash_embed)`，并调整 `pyproject.toml` 中的 `cmake.source-dir` 配置。

### 2. CI/CD 环境一致性 (Mamba)
> **问题**: GitHub Actions 使用 pip 安装依赖，导致与本地 Mamba 环境不一致，且无法获取系统级 C++ 库。
> **解决**: 引入 `conda-incubator/setup-miniconda`，并在 CI 中直接复用 `environment.yml`，确保 "Local" 与 "Remote" 环境 100% 同构。

---

## Phase 1: 核心算子与 SIMD 加速 (Core Kernels)

### 3. 内存管理与对齐 (Memory Alignment)

> 待记录：在实现 SIMD 加速时遇到的 `Segmentation Fault` 问题。
> *   分析：AVX2 指令对内存地址 32 字节对齐的要求。
> *   解决：自定义 `AlignedAllocator` 或使用 Padding 策略的实施过程。

## Phase 2: 推理引擎集成 (Inference Engine)

### 4. Pybind11 生命周期管理 (Object Lifecycle)

> 待记录：Python 垃圾回收 (GC) 与 C++ 对象生命周期的冲突。
> *   分析：异步推理场景下 `numpy.ndarray` 被提前回收导致的悬垂指针问题。
> *   解决：`py::keep_alive` 策略或 `py::capsule` 的具体应用。

## Phase 3: 服务化与全链路优化

### 5. 多线程资源争抢 (Thread Contention)

> 待记录：高并发下 CPU 调度延迟抖动问题。
> *   分析：OpenMP 线程池与 ONNX Runtime 内部线程池的资源竞争（Oversubscription）。
> *   解决：线程绑定（Affinity）设置与 Docker 核心隔离策略。
