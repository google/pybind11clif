#include <pybind11/pybind11.h>

#include "pybind11_tests.h"

namespace pybind11_tests {
namespace wip {

struct Vanilla {};

} // namespace wip
} // namespace pybind11_tests

TEST_SUBMODULE(wip, m) {
    using namespace pybind11_tests::wip;

    py::class_<Vanilla>(m, "Vanilla", py::metaclass((PyObject *) &PyType_Type)).def(py::init<>());
}
