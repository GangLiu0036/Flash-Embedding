#pragma once
#include <memory>
#include <new>
#include <limits>
#include <stdexcept>
#include <cstdlib>

namespace flash_embed {
namespace kernels {

/**
 * @brief 兼容 STL 的内存分配器，强制执行 32 字节内存对齐。
 * 这是使用 AVX2 _mm256_load_ps 指令防止段错误 (SegFault) 的必要条件。
 * 
 * @tparam T 元素类型
 * @tparam Alignment 对齐字节数 (AVX2 默认为 32)
 */
template <typename T, size_t Alignment = 32>
class AlignedAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = std::size_t;

    AlignedAllocator() noexcept = default;

    template <typename U>
    AlignedAllocator(const AlignedAllocator<U, Alignment>&) noexcept {}

    /**
     * @brief 分配对齐的内存块。
     * 
     * @param n 需要分配的元素数量
     * @return pointer 指向对齐内存的指针
     * @throws std::bad_alloc 如果分配失败或 n 太大
     */
    pointer allocate(size_type n) {
        if (n > std::numeric_limits<size_type>::max() / sizeof(T)) {
            throw std::bad_alloc();
        }

        void* ptr = nullptr;
        // posix_memalign 要求对齐必须是 2 的幂且 >= sizeof(void*)
        if (posix_memalign(&ptr, Alignment, n * sizeof(T)) != 0) {
            throw std::bad_alloc();
        }
        return static_cast<pointer>(ptr);
    }

    /**
     * @brief 释放内存块。
     * 
     * @param p 指向要释放内存的指针
     * @param size_type 要释放的元素数量 (未使用，但符合分配器接口规范)
     */
    void deallocate(pointer p, size_type) noexcept {
        free(p);
    }

    template <typename U>
    struct rebind {
        using other = AlignedAllocator<U, Alignment>;
    };

    bool operator==(const AlignedAllocator&) const noexcept { return true; }
    bool operator!=(const AlignedAllocator&) const noexcept { return false; }
};

} // namespace kernels
} // namespace flash_embed




