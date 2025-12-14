#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "flash_embed.h"

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {
    m.doc() = "Flash-Embed C++ Core Module";

    // 绑定核心类
    py::class_<flash_embed::FlashEmbedCore>(m, "FlashEmbedCore")
        .def(py::init<>())
        .def("get_backend_name", &flash_embed::FlashEmbedCore::get_backend_name,
             "Get the name of the active SIMD backend (e.g., 'AVX2' or 'Scalar').")
        .def("cosine_similarity", [](flash_embed::FlashEmbedCore& self, 
                                     py::array_t<float, py::array::c_style | py::array::forcecast> a, 
                                     py::array_t<float, py::array::c_style | py::array::forcecast> b) {
            // 请求 Buffer 信息
            py::buffer_info buf_a = a.request();
            py::buffer_info buf_b = b.request();

            if (buf_a.ndim != 1 || buf_b.ndim != 1) {
                throw std::runtime_error("Input arrays must be 1-dimensional");
            }
            if (buf_a.shape[0] != buf_b.shape[0]) {
                throw std::runtime_error("Input arrays must have the same size");
            }

            // 获取原始指针 (Zero-Copy)
            float* ptr_a = static_cast<float*>(buf_a.ptr);
            float* ptr_b = static_cast<float*>(buf_b.ptr);
            size_t len = buf_a.shape[0];

            // 检查 AVX2 对齐要求 (32 字节)
            // 如果是在 Python 端创建的 numpy 数组，默认可能不是 32 字节对齐的。
            // 这是一个潜在的坑。如果使用 AVX2 后端且指针未对齐，C++ 层的检查会抛出异常。
            // 在实际工程中，我们通常会在 Python 端封装一个对齐分配的函数，或者在 C++ 端做降级处理。
            // 这里为了演示 Zero-Copy + AVX2 的严格要求，保留对齐检查，
            // 并在 Python 测试中展示如何创建对齐数组。
            
            return self.cosine_similarity(ptr_a, ptr_b, len);
        }, "Compute cosine similarity between two vectors.");

    // 保留旧的 add 函数用于测试，或者删除它
    m.def("add", &flash_embed::add, "A function that adds two numbers");
}
