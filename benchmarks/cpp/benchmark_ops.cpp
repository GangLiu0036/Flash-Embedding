#include <benchmark/benchmark.h>

#include <memory>
#include <random>
#include <vector>

#include "kernels/allocator.h"
#include "kernels/factory.h"

namespace {

// 使用 AlignedAllocator 创建对齐的向量
template <typename T>
using AlignedVector = std::vector<T, flash_embed::kernels::AlignedAllocator<T, 32>>;

// 初始化随机数据
AlignedVector<float> generate_random_vector(size_t len) {
    AlignedVector<float> vec(len);
    std::mt19937 gen(42);  // 固定种子以复现
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (size_t i = 0; i < len; ++i) {
        vec[i] = dis(gen);
    }
    return vec;
}

}  // namespace

// Benchmark Scalar 实现
static void BM_CosineSimilarity_Scalar(benchmark::State& state) {
    size_t len = state.range(0);
    auto a = generate_random_vector(len);
    auto b = generate_random_vector(len);
    auto ops = flash_embed::kernels::create_scalar_ops();

    for (auto _ : state) {
        // 防止编译器优化掉计算过程
        benchmark::DoNotOptimize(ops->cosine_similarity(a.data(), b.data(), len));
    }
}

// Benchmark AVX2 实现
static void BM_CosineSimilarity_AVX2(benchmark::State& state) {
    size_t len = state.range(0);
    auto a = generate_random_vector(len);
    auto b = generate_random_vector(len);
    auto ops = flash_embed::kernels::create_avx2_ops();

    for (auto _ : state) {
        benchmark::DoNotOptimize(ops->cosine_similarity(a.data(), b.data(), len));
    }
}

// 注册 Benchmark
// 场景涵盖典型维度：768 (BERT), 1024 (Large), 4096 (LLM)
// 以及非对齐边界：769 (验证处理尾部元素的能力)
BENCHMARK(BM_CosineSimilarity_Scalar)->Arg(768)->Arg(1024)->Arg(4096)->Arg(769);
BENCHMARK(BM_CosineSimilarity_AVX2)->Arg(768)->Arg(1024)->Arg(4096)->Arg(769);

BENCHMARK_MAIN();
