#include <pybind11/stl.h>

#include "pybind11_tests.h"

#include <array>

namespace pybind11_tests {
namespace stl_no_default_ctor {

struct Node {
    explicit constexpr Node(int val) : val{val} {}

    int val = -88;
};

struct NodeArray {
    explicit constexpr NodeArray(int i) : arr{{Node(10 + i), Node(20 + i)}} {}

    std::array<Node, 2> arr;
};

} // namespace stl_no_default_ctor
} // namespace pybind11_tests

using namespace pybind11_tests::stl_no_default_ctor;

TEST_SUBMODULE(stl_no_default_ctor, m) {
    py::class_<Node>(m, "Node").def_readonly("val", &Node::val);
    py::class_<NodeArray>(m, "NodeArray")
        .def(py::init<int>())
        .def_readwrite("arr", &NodeArray::arr);
}
