#include "pybind11_tests.h"

namespace pybind11_tests {
namespace type_caster_parameter_pack {

template <typename, typename>
struct my_type_t {};

template <typename, long>
struct my_type_l {};

template <typename>
struct type_sink {
    static constexpr int num = 0;
};

template <typename... T>
struct type_sink<my_type_t<T...>> {
    static constexpr int num = 1;
};

template <typename... T>
struct type_sink<my_type_l<T...>> {
    static constexpr int num = 2;
};

///

template <typename, typename>
struct atype {};

} // namespace type_caster_parameter_pack
} // namespace pybind11_tests

using namespace pybind11_tests::type_caster_parameter_pack;

namespace pybind11 {
namespace detail {

template <typename... T>
struct type_caster<atype<T...>> {
    static auto constexpr name = const_name<atype<T...>>();

    static handle
    cast(atype<T...> const & /*src*/, return_value_policy /*policy*/, handle /*parent*/) {
        return none().release();
    }

    template <typename>
    using cast_op_type = const atype<T...> &;

    // NOLINTNEXTLINE(google-explicit-constructor)
    operator atype<T...> const &() {
        static atype<T...> obj;
        return obj;
    }

    bool load(handle /*src*/, bool /*convert*/) {
        printf("\nLOOOK %s:%d\n", __FILE__, __LINE__);
        fflush(stdout);
        return true;
    }
};

} // namespace detail
} // namespace pybind11

TEST_SUBMODULE(type_caster_parameter_pack, m) {
    m.def("from_python", [](const atype<int, int> &) {});
    m.def("type_sink_void", []() { return type_sink<void>::num; });
    m.def("type_sink_my_type_t_int_int", []() { return type_sink<my_type_t<int, int>>::num; });
    m.def("type_sink_my_type_l_int_2", []() { return type_sink<my_type_l<int, 2>>::num; });
}
