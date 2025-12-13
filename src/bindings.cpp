#include <pybind11/pybind11.h>

#include "flash_embed.h"

namespace py = pybind11;

PYBIND11_MODULE(_core, m) {
    m.doc() = "Flash-Embed C++ Core Module";  // optional module docstring

    m.def("add", &flash_embed::add, "A function that adds two numbers");
}
