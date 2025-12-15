#include <gtest/gtest.h>

#include <cmath>
#include <random>
#include <vector>

#include "kernels/allocator.h"
#include "kernels/factory.h"

namespace {

// 使用 AlignedAllocator 创建对齐的向量
template <typename T>
using AlignedVector = std::vector<T, flash_embed::kernels::AlignedAllocator<T, 32>>;

// 辅助函数：生成随机向量
AlignedVector<float> generate_random_vector(size_t len, float seed_offset = 0.0f) {
    AlignedVector<float> vec(len);
    std::mt19937 gen(42 + static_cast<int>(seed_offset));
    std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    for (size_t i = 0; i < len; ++i) {
        vec[i] = dis(gen);
    }
    return vec;
}

// 辅助函数：手动计算 Cosine Similarity (作为 ground truth)
float reference_cosine(const std::vector<float>& a, const std::vector<float>& b) {
    double dot = 0.0;
    double norm_a = 0.0;
    double norm_b = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }
    return static_cast<float>(dot / (std::sqrt(norm_a) * std::sqrt(norm_b)));
}

}  // namespace

class MathOpsTest : public ::testing::Test {
protected:
    void SetUp() override {
        scalar_ops = flash_embed::kernels::create_scalar_ops();
        avx2_ops = flash_embed::kernels::create_avx2_ops();
    }

    std::unique_ptr<flash_embed::kernels::MathOps> scalar_ops;
    std::unique_ptr<flash_embed::kernels::MathOps> avx2_ops;
};

// 测试 1: Scalar 实现的基本正确性 (对比手动计算)
TEST_F(MathOpsTest, ScalarCorrectness) {
    size_t len = 128;
    auto a = generate_random_vector(len, 0);
    auto b = generate_random_vector(len, 100);

    // 将 AlignedVector 转换为 std::vector 以供 reference 函数使用
    std::vector<float> ref_a(a.begin(), a.end());
    std::vector<float> ref_b(b.begin(), b.end());

    float expected = reference_cosine(ref_a, ref_b);
    float actual = scalar_ops->cosine_similarity(a.data(), b.data(), len);

    EXPECT_NEAR(actual, expected, 1e-5);
}

// 测试 2: AVX2 与 Scalar 的一致性 (Consistency)
// 这是验证 SIMD 实现正确性的金标准
TEST_F(MathOpsTest, Avx2VsScalarConsistency) {
    // 测试多种长度，覆盖 8 的倍数和非倍数
    std::vector<size_t> lengths = {1, 7, 8, 9, 16, 768, 769, 1024, 1025};

    for (size_t len : lengths) {
        auto a = generate_random_vector(len, 0);
        auto b = generate_random_vector(len, 100);

        float scalar_res = scalar_ops->cosine_similarity(a.data(), b.data(), len);
        float avx2_res = avx2_ops->cosine_similarity(a.data(), b.data(), len);

        // 浮点数运算顺序不同会导致微小误差，通常 < 1e-5 是安全的
        EXPECT_NEAR(avx2_res, scalar_res, 1e-5) << "Mismatch at length " << len;
    }
}

// 测试 3: L2 Norm 正确性
TEST_F(MathOpsTest, L2NormConsistency) {
    std::vector<size_t> lengths = {1, 7, 8, 15, 16, 769};

    for (size_t len : lengths) {
        auto a = generate_random_vector(len, 0);

        float scalar_norm = scalar_ops->l2_norm(a.data(), len);
        float avx2_norm = avx2_ops->l2_norm(a.data(), len);

        EXPECT_NEAR(avx2_norm, scalar_norm, 1e-5) << "L2 Norm mismatch at length " << len;
    }
}

// 测试 4: 零向量处理
TEST_F(MathOpsTest, ZeroVector) {
    size_t len = 32;
    AlignedVector<float> a(len, 0.0f);
    AlignedVector<float> b(len, 1.0f);

    EXPECT_EQ(avx2_ops->cosine_similarity(a.data(), b.data(), len), 0.0f);
}

// 测试 5: 内存未对齐抛出异常 (Negative Test)
TEST_F(MathOpsTest, UnalignedMemoryThrows) {
    // 分配一个未对齐的内存 (使用 malloc 或 new char[])
    // 注意：new float[] 通常只保证对齐到 float (4 bytes)，不保证 32 bytes
    float* unaligned_ptr = new float[32];

    // 强制制造一个未对齐的地址（如果 new 偶然对齐了，我们加 1）
    float* ptr = unaligned_ptr;
    if (reinterpret_cast<uintptr_t>(ptr) % 32 == 0) {
        ptr += 1;
    }

    // 只需要测试对齐检查是否生效
    // 注意：我们必须确保 ptr 指向的内存是可访问的，这里我们只是简单的移动指针
    // 实际上访问 ptr+1 可能越界，所以我们分配大一点
    float* safe_unaligned_buffer = new float[64];
    float* test_ptr = safe_unaligned_buffer;
    if (reinterpret_cast<uintptr_t>(test_ptr) % 32 == 0) {
        test_ptr += 1;  // 偏移 4 字节，肯定不对齐 32
    }

    // 应该抛出 runtime_error
    EXPECT_THROW(avx2_ops->l2_norm(test_ptr, 16), std::runtime_error);

    delete[] unaligned_ptr;
    delete[] safe_unaligned_buffer;
}
