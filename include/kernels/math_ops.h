#pragma once
#include <cstddef>

namespace flash_embed {
namespace kernels {

/**
 * @brief 数学运算的抽象策略接口。
 * 允许在运行时根据硬件能力切换 Scalar（标量）与 SIMD（向量化）实现。
 */
class MathOps {
public:
    virtual ~MathOps() = default;

    /**
     * @brief 计算两个向量的余弦相似度 (Cosine Similarity)。
     * 
     * @param a 第一个向量的指针 (若使用 AVX2，内存必须 32 字节对齐)
     * @param b 第二个向量的指针 (若使用 AVX2，内存必须 32 字节对齐)
     * @param len 向量维度
     * @return float 余弦相似度值 (-1.0 到 1.0)
     */
    virtual float cosine_similarity(const float* a, const float* b, size_t len) const = 0;

    /**
     * @brief 计算向量的 L2 范数 (Euclidean Norm)。
     * 
     * @param a 向量指针
     * @param len 向量维度
     * @return float L2 范数
     */
    virtual float l2_norm(const float* a, size_t len) const = 0;
};

} // namespace kernels
} // namespace flash_embed

