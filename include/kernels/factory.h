#pragma once
#include <memory>

#include "kernels/math_ops.h"

namespace flash_embed {
namespace kernels {

// 工厂函数声明
std::unique_ptr<MathOps> create_scalar_ops();
std::unique_ptr<MathOps> create_avx2_ops();

}  // namespace kernels
}  // namespace flash_embed
