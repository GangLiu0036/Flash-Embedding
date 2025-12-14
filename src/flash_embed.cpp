#include "flash_embed.h"
#include "kernels/factory.h"
#include <iostream>

#if defined(__x86_64__) || defined(_M_X64)
#include <cpuid.h>
#endif

namespace flash_embed {

namespace {
// 简单的 CPUID 检查，判断是否支持 AVX2 和 FMA
bool has_avx2_support() {
#if defined(__x86_64__) || defined(_M_X64)
    unsigned int eax, ebx, ecx, edx;
    // Check for AVX2: CPUID.(EAX=07H, ECX=0H):EBX.AVX2[bit 5]
    if (__get_cpuid(7, &eax, &ebx, &ecx, &edx)) {
        if ((ebx & (1 << 5)) != 0) {
            // Check for FMA: CPUID.(EAX=01H):ECX.FMA[bit 12]
            if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
                return (ecx & (1 << 12)) != 0;
            }
        }
    }
#endif
    return false;
}
} // namespace

FlashEmbedCore::FlashEmbedCore() {
    if (has_avx2_support()) {
        ops_ = kernels::create_avx2_ops();
        backend_name_ = "AVX2";
    } else {
        ops_ = kernels::create_scalar_ops();
        backend_name_ = "Scalar";
    }
}

float FlashEmbedCore::cosine_similarity(const float* a, const float* b, size_t len) const {
    return ops_->cosine_similarity(a, b, len);
}

const char* FlashEmbedCore::get_backend_name() const {
    return backend_name_;
}

} // namespace flash_embed
