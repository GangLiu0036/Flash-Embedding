# 🛠️ 开发日志 & 难点攻克 (Engineering Challenges)

本日志用于记录开发过程中遇到的底层系统编程难题及其解决方案，重点关注性能瓶颈与稳定性问题。

## 1. 内存管理与对齐 (Memory Alignment)

> 待记录：在实现 SIMD 加速时遇到的 `Segmentation Fault` 问题。
> *   分析：AVX2 指令对内存地址 32 字节对齐的要求。
> *   解决：自定义 `AlignedAllocator` 或使用 Padding 策略的实施过程。

## 2. Pybind11 生命周期管理 (Object Lifecycle)

> 待记录：Python 垃圾回收 (GC) 与 C++ 对象生命周期的冲突。
> *   分析：异步推理场景下 `numpy.ndarray` 被提前回收导致的悬垂指针问题。
> *   解决：`py::keep_alive` 策略或 `py::capsule` 的具体应用。

## 3. 多线程资源争抢 (Thread Contention)

> 待记录：高并发下 CPU 调度延迟抖动问题。
> *   分析：OpenMP 线程池与 ONNX Runtime 内部线程池的资源竞争（Oversubscription）。
> *   解决：线程绑定（Affinity）设置与 Docker 核心隔离策略。
