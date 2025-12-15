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

### 3. 本地 Mamba 环境路径冲突
> **问题**: 无法激活 `flash-embed` 环境，`mamba info` 显示 `base environment` 指向 `~/.local/share/mamba`，但 `$MAMBA_ROOT_PREFIX` 却被全局设置为该路径，导致与实际安装在 `~/miniforge3` 的环境不匹配。
> **分析**:
> ```bash
> $ mamba info
> # ...
> # base environment : /home/aweo/.local/share/mamba
> # $MAMBA_ROOT_PREFIX: set in global scope ...
> ```
> Mamba 的 `root_prefix` 配置与 Miniforge 安装路径不一致。
> **解决**: 修改本地 `~/.config/fish/config.fish`，将 `root_prefix` 统一指向 `~/miniforge3`，确保环境路径解析正确。

### 4. CI 环境迁移 (Mambaforge Deprecation)
> **问题**: GitHub Actions CI 失败，报错 `Unexpected HTTP response: 404`，尝试下载 `Mambaforge` 安装包失败。
> **分析**: `Mambaforge` 发行版已停止更新并合并入 `Miniforge3`。`setup-miniconda` 尝试下载旧版链接导致 404。
> **解决**: 更新 `.github/workflows/build.yml`，将 `miniforge-variant` 设置为 `Miniforge3`。

---

## Phase 1: 核心算子与 SIMD 加速 (Core Kernels)

> **主要交付 (Key Deliverables)**:
> *   **核心算子**: 实现 `Cosine Similarity` 与 `L2 Norm`，支持 Scalar/AVX2 运行时自动切换。
> *   **测试套件**: 完成 GTest (C++) 与 Pytest (Python) 覆盖，确保精度一致性。
> *   **性能验证**: 输出基准测试报告，确认 AVX2 相比 Scalar 获得 >4.5x 加速。

### 5. 内存管理与对齐 (Memory Alignment)
> **问题**: 在实现 AVX2 加速时，`_mm256_load_ps` 指令触发 `Segmentation Fault`。
> **分析**: AVX2 指令集强制要求内存首地址必须 32 字节对齐。普通的 `std::vector` 或 `new float[]` 无法保证此对齐要求。
> **解决**: 实现自定义分配器 `AlignedAllocator` (封装 `posix_memalign`)，并将其作为 `std::vector` 的模板参数，确保所有向量内存满足 32 字节对齐。

### 6. Python 绑定与模块导出
> **问题**: 移除了 `bindings.cpp` 中的 `add` 示例函数后，Python 测试报错 `AttributeError`，无法导入 `FlashEmbedCore`。
> **分析**: `python/flash_embed/__init__.py` 未正确从 `_core` 扩展模块导出新的类，且旧的引用未清理。
> **解决**: 更新 `__init__.py`，显式导入并暴露 `FlashEmbedCore` 类，同时清理测试代码中的过时引用。

## Phase 2: 推理引擎集成 (Inference Engine)

### 7. Pybind11 生命周期管理 (Object Lifecycle)

> 待记录：Python 垃圾回收 (GC) 与 C++ 对象生命周期的冲突。
> *   分析：异步推理场景下 `numpy.ndarray` 被提前回收导致的悬垂指针问题。
> *   解决：`py::keep_alive` 策略或 `py::capsule` 的具体应用。

## Phase 3: 服务化与全链路优化

### 8. 多线程资源争抢 (Thread Contention)

> 待记录：高并发下 CPU 调度延迟抖动问题。
> *   分析：OpenMP 线程池与 ONNX Runtime 内部线程池的资源竞争（Oversubscription）。
> *   解决：线程绑定（Affinity）设置与 Docker 核心隔离策略。
