#pragma once
#include <memory>
#include <vector>

#include "kernels/math_ops.h"

namespace flash_embed {

/**
 * @brief Flash-Embedding 的核心引擎类。
 *
 * 该类作为整个 C++ 后端的入口点 (Facade)，负责：
 * 1. 管理底层的算子后端 (MathOps)。
 * 2. 在构造时自动检测 CPU 指令集 (AVX2/Scalar)，并初始化对应的策略。
 * 3. 向 Python 层提供统一的高性能计算接口。
 */
class FlashEmbedCore {
public:
    /**
     * @brief 构造函数：初始化并选择最优的算子后端。
     */
    FlashEmbedCore();
    ~FlashEmbedCore() = default;

    // 删除拷贝构造，只允许移动 (Rule of Five)
    FlashEmbedCore(const FlashEmbedCore&) = delete;
    FlashEmbedCore& operator=(const FlashEmbedCore&) = delete;

    /**
     * @brief 计算两个向量的余弦相似度。
     *
     * @param a 第一个向量的指针 (若使用 AVX2 后端，内存首地址必须 32 字节对齐)
     * @param b 第二个向量的指针 (若使用 AVX2 后端，内存首地址必须 32 字节对齐)
     * @param len 向量维度 (元素个数)
     * @return float 余弦相似度 (-1.0 到 1.0)
     */
    float cosine_similarity(const float* a, const float* b, size_t len) const;

    /**
     * @brief 获取当前使用的算子后端名称 (Scalar 或 AVX2)
     */
    const char* get_backend_name() const;

private:
    std::unique_ptr<kernels::MathOps> ops_;
    const char* backend_name_;
};

}  // namespace flash_embed
