#include <pybind11/smart_holder.h>

#include "pybind11_tests.h"

namespace pybind11_tests {
namespace type_caster_addressof {

struct UnusualOpRefReturnType {};

template <int> // Using int as a trick to easily generate a series of types.
struct UnusualOpRef {
    UnusualOpRefReturnType operator&() { return UnusualOpRefReturnType(); }
};

} // namespace type_caster_addressof
} // namespace pybind11_tests

PYBIND11_SMART_HOLDER_TYPE_CASTERS(pybind11_tests::type_caster_addressof::UnusualOpRef<1>)

namespace pybind11_tests {
namespace type_caster_addressof {

py::object PassMovable0(UnusualOpRef<0> &&mvbl) { return py::cast(std::move(mvbl)); }
py::object PassMovable1(UnusualOpRef<1> &&mvbl) { return py::cast(std::move(mvbl)); }

} // namespace type_caster_addressof
} // namespace pybind11_tests

TEST_SUBMODULE(type_caster_addressof, m) {
    using namespace pybind11_tests::type_caster_addressof;
    py::class_<UnusualOpRef<0>>(m, "UnusualOpRef0");
    py::classh<UnusualOpRef<1>>(m, "UnusualOpRef1");
    m.def("CallPassMovable0", []() { return PassMovable0(UnusualOpRef<0>()); });
    m.def("CallPassMovable1", []() { return PassMovable1(UnusualOpRef<1>()); });
}
