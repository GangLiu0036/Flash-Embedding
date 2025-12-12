# 💻 核心代码展示 (Show Me The Code)

本章节将精选项目中几个最具代表性的代码片段，展示 C++ 底层优化与 Python 绑定的具体实现。

## 1. Pybind11 Zero-Copy 实现

> 待补充：展示如何利用 `py::array_t` 的 Buffer Protocol 获取原生指针，以及 `nocopy` 标志位的处理，证明数据在 Python 与 C++ 之间无缝传输。

## 2. AVX2 SIMD Kernel (Cosine Similarity)

> 待补充：展示手动编写的 Intrinsic 代码片段。包括 `_mm256_fmadd_ps` 指令的使用，以及 Loop Unrolling 和剩余元素处理（Peeling）的逻辑。

## 3. 线程安全的内存池 (Thread-Safe Memory Pool)

> 待补充：展示如何使用 RAII 锁（`std::lock_guard`）与对象池技术复用推理 Session，减少高并发下的内存分配开销。
