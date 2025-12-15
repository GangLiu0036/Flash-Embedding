#include <immintrin.h>

#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>

#include "kernels/factory.h"
#include "kernels/math_ops.h"

namespace flash_embed {
namespace kernels {

/**
 * @brief AVX2 SIMD 数学运算实现。
 *
 * 使用 AVX2 指令集（_mm256_*）对浮点数计算进行加速。
 * 核心思想是一次处理 8 个 float，并通过 FMA (Fused Multiply-Add) 指令提升吞吐量。
 *
 * 注意：输入内存地址必须是 32 字节对齐的，否则会导致段错误。
 */
class Avx2Ops : public MathOps {
public:
    /**
     * @brief 计算余弦相似度 (AVX2 加速版本)
     *
     * 算法流程：
     * 1. 初始化 3 个累加寄存器 (sum_dot, sum_aa, sum_bb) 为 0。
     * 2. 主循环：每次处理 8 个 float。
     *    - Load: _mm256_load_ps 加载两个向量的数据。
     *    - FMA: _mm256_fmadd_ps 同时更新点积和两个模长平方和。
     * 3. 剩余元素处理 (Peeling): 对不足 8 个的尾部元素使用标量计算。
     * 4. 归约 (Reduction): 将 AVX2 寄存器中的 8 个分量累加到单个 float。
     * 5. 最终计算: dot / (sqrt(norm_a) * sqrt(norm_b))。
     */
    float cosine_similarity(const float* a, const float* b, size_t len) const override {
        // 检查内存对齐 (32字节)
        if ((reinterpret_cast<uintptr_t>(a) % 32 != 0) ||
            (reinterpret_cast<uintptr_t>(b) % 32 != 0)) {
            throw std::runtime_error("Memory not aligned to 32 bytes for AVX2");
        }

        // 初始化累加器 (8个float的零向量)
        __m256 sum_dot = _mm256_setzero_ps();
        __m256 sum_aa = _mm256_setzero_ps();
        __m256 sum_bb = _mm256_setzero_ps();

        size_t i = 0;
        // 主循环：每次处理 8 个 float (256 bits / 32 bits = 8)
        // 使用循环展开 (Unrolling) 还可以进一步提升性能，但这里先保持简单
        for (; i + 7 < len; i += 8) {
            __m256 va = _mm256_load_ps(a + i);  // 加载 a[i...i+7]
            __m256 vb = _mm256_load_ps(b + i);  // 加载 b[i...i+7]

            // FMA: a * b + c
            sum_dot = _mm256_fmadd_ps(va, vb, sum_dot);  // sum_dot += va * vb
            sum_aa = _mm256_fmadd_ps(va, va, sum_aa);    // sum_aa  += va * va
            sum_bb = _mm256_fmadd_ps(vb, vb, sum_bb);    // sum_bb  += vb * vb
        }

        // 水平归约 (Horizontal Reduction)
        // 将 sum_dot 寄存器里的 8 个 float 加起来
        float dot = reduce_add_avx2(sum_dot);
        float norm_a = reduce_add_avx2(sum_aa);
        float norm_b = reduce_add_avx2(sum_bb);

        // 处理剩余元素 (Tail Loop)
        for (; i < len; ++i) {
            float val_a = a[i];
            float val_b = b[i];
            dot += val_a * val_b;
            norm_a += val_a * val_a;
            norm_b += val_b * val_b;
        }

        if (norm_a == 0.0f || norm_b == 0.0f) {
            return 0.0f;
        }

        return dot / (std::sqrt(norm_a) * std::sqrt(norm_b));
    }

    /**
     * @brief 计算 L2 范数 (AVX2 加速版本)
     */
    float l2_norm(const float* a, size_t len) const override {
        if (reinterpret_cast<uintptr_t>(a) % 32 != 0) {
            throw std::runtime_error("Memory not aligned to 32 bytes for AVX2");
        }

        __m256 sum_sq = _mm256_setzero_ps();
        size_t i = 0;

        for (; i + 7 < len; i += 8) {
            __m256 va = _mm256_load_ps(a + i);
            sum_sq = _mm256_fmadd_ps(va, va, sum_sq);
        }

        float norm = reduce_add_avx2(sum_sq);

        for (; i < len; ++i) {
            norm += a[i] * a[i];
        }

        return std::sqrt(norm);
    }

private:
    /**
     * @brief 辅助函数：将 __m256 中的 8 个 float 相加
     *
     * 经典的 AVX2 reduce 模式：
     * 1. hadd: 水平相加，将相邻元素相加。
     * 2. extract + add: 提取低位和高位进行最终求和。
     */
    inline float reduce_add_avx2(__m256 v) const {
        // Step 1: 将高 128 位加到低 128 位
        // v_low = [x0+x4, x1+x5, x2+x6, x3+x7]
        __m128 v_low = _mm_add_ps(_mm256_castps256_ps128(v), _mm256_extractf128_ps(v, 1));

        // Step 2: 水平加法
        // v_hadd = [x0+x4+x1+x5, x2+x6+x3+x7, ...]
        __m128 v_hadd = _mm_hadd_ps(v_low, v_low);

        // Step 3: 再做一次水平加法
        // v_hadd2 = [Sum, Sum, ...]
        __m128 v_hadd2 = _mm_hadd_ps(v_hadd, v_hadd);

        // 提取最低位的 float
        return _mm_cvtss_f32(v_hadd2);
    }
};

std::unique_ptr<MathOps> create_avx2_ops() {
    return std::make_unique<Avx2Ops>();
}

}  // namespace kernels
}  // namespace flash_embed
