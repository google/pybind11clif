#include <pybind11/smart_holder.h>

#include "pybind11_tests.h"

#include <string>
#include <vector>

namespace pybind11_tests {
namespace type_caster_addressof {

template <int> // Using int as a trick to easily generate a series of types.
class UnusualOpRef {
private:
    std::vector<std::string> non_trivial_member;

public:
    std::vector<std::string> operator&() { return non_trivial_member; }
};

} // namespace type_caster_addressof
} // namespace pybind11_tests

PYBIND11_SMART_HOLDER_TYPE_CASTERS(pybind11_tests::type_caster_addressof::UnusualOpRef<1>)

namespace pybind11_tests {
namespace type_caster_addressof {

py::object CastConstRef0(const UnusualOpRef<0> &cref) { return py::cast(cref); }
py::object CastConstRef1(const UnusualOpRef<1> &cref) { return py::cast(cref); }
py::object CastMovable0(UnusualOpRef<0> &&mvbl) { return py::cast(std::move(mvbl)); }
py::object CastMovable1(UnusualOpRef<1> &&mvbl) { return py::cast(std::move(mvbl)); }

} // namespace type_caster_addressof
} // namespace pybind11_tests

TEST_SUBMODULE(type_caster_addressof, m) {
    using namespace pybind11_tests::type_caster_addressof;
    py::class_<UnusualOpRef<0>>(m, "UnusualOpRef0");
    py::classh<UnusualOpRef<1>>(m, "UnusualOpRef1");
    m.def("CallCastConstRef0", []() { return CastConstRef0(UnusualOpRef<0>()); });
    m.def("CallCastConstRef1", []() { return CastConstRef1(UnusualOpRef<1>()); });
    m.def("CallCastMovable0", []() { return CastMovable0(UnusualOpRef<0>()); });
    m.def("CallCastMovable1", []() { return CastMovable1(UnusualOpRef<1>()); });
}
