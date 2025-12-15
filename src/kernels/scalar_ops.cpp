#include "kernels/math_ops.h"
#include "kernels/factory.h"
#include <cmath>
#include <memory>

namespace flash_embed {
namespace kernels {

/**
 * @brief 标量 (Scalar) 数学运算实现。
 * 
 * 这是最基础的实现版本，不使用任何显式的 SIMD 指令。
 * 主要用于作为 Benchmark 的基准 (Baseline) 以及在不支持 AVX2 的旧硬件上运行。
 */
class ScalarOps : public MathOps {
public:
    /**
     * @brief 计算余弦相似度 (标量版本)
     */
    float cosine_similarity(const float* a, const float* b, size_t len) const override {
        float dot = 0.0f;
        float norm_a = 0.0f;
        float norm_b = 0.0f;

        // 朴素循环 - 编译器可能会尝试自动向量化 (Auto-vectorize)
        // 我们在 GCC 编译 Benchmark.cpp 时添加构建标志 (-fno-tree-vectorize) 来确保这是真正的标量实现
        for (size_t i = 0; i < len; ++i) {
            dot += a[i] * b[i];
            norm_a += a[i] * a[i];
            norm_b += b[i] * b[i];
        }

        if (norm_a == 0.0f || norm_b == 0.0f) {
            return 0.0f;
        }

        return dot / (std::sqrt(norm_a) * std::sqrt(norm_b));
    }

    /**
     * @brief 计算 L2 范数 (标量版本)
     */
    float l2_norm(const float* a, size_t len) const override {
        float sum = 0.0f;
        for (size_t i = 0; i < len; ++i) {
            sum += a[i] * a[i];
        }
        return std::sqrt(sum);
    }
};

std::unique_ptr<MathOps> create_scalar_ops() {
    return std::make_unique<ScalarOps>();
}

} // namespace kernels
} // namespace flash_embed

