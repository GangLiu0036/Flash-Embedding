import pytest
import numpy as np
import flash_embed


# 辅助函数：生成对齐的 numpy 数组
# 虽然我们的 Pybind11 接口支持任意 numpy 数组（通过 internal copy 即使不对齐也能跑，但这会触发 copy）
# 但为了测试 Zero-Copy 路径，我们最好能构造对齐的数据。
# 简单的 np.array(...) 并不保证 32 字节对齐。
def aligned_array(shape, dtype=np.float32, alignment=32):
    # 分配多一点内存
    n_bytes = np.prod(shape) * np.dtype(dtype).itemsize
    buffer = np.empty(n_bytes + alignment, dtype=np.uint8)

    # 计算对齐偏移
    start_index = buffer.ctypes.data % alignment
    offset = 0
    if start_index != 0:
        offset = alignment - start_index

    # 创建视图
    aligned_buffer = buffer[offset : offset + n_bytes]
    return np.frombuffer(aligned_buffer.data, dtype=dtype).reshape(shape)


def test_cosine_similarity_correctness():
    """验证 Python 接口计算结果与 Numpy 的一致性"""

    # 1. 构造数据
    dim = 768
    # 使用 aligned_array 确保内存对齐，满足 AVX2 Zero-Copy 要求
    a = aligned_array((dim,), dtype=np.float32)
    b = aligned_array((dim,), dtype=np.float32)

    # 填充随机数据
    a[:] = np.random.randn(dim).astype(np.float32)
    b[:] = np.random.randn(dim).astype(np.float32)

    # 2. 调用 C++ 扩展
    core = flash_embed.FlashEmbedCore()
    cpp_sim = core.cosine_similarity(a, b)

    # 3. 调用 Numpy 基准
    norm_a = np.linalg.norm(a)
    norm_b = np.linalg.norm(b)
    numpy_sim = np.dot(a, b) / (norm_a * norm_b)

    # 4. 验证
    # 允许 1e-5 的误差 (float32精度限制)
    assert np.isclose(
        cpp_sim, numpy_sim, atol=1e-5
    ), f"Mismatch: C++={cpp_sim}, Numpy={numpy_sim}"


def test_backend_selection():
    """验证能够正确获取后端名称"""
    core = flash_embed.FlashEmbedCore()
    backend = core.get_backend_name()
    print(f"\n[Info] Active Backend: {backend}")
    assert backend in ["AVX2", "Scalar"]


def test_input_validation():
    """验证输入维度检查"""
    core = flash_embed.FlashEmbedCore()
    a = np.zeros(10, dtype=np.float32)
    b = np.zeros(11, dtype=np.float32)  # 长度不同

    with pytest.raises(RuntimeError) as excinfo:
        core.cosine_similarity(a, b)
    assert "same size" in str(excinfo.value)
