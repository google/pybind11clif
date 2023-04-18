#include "pybind11_tests.h"

#include <type_traits>

namespace pybind11_tests {
namespace pick_alt_struct {

template <typename T>
struct is_primary : std::false_type {};

template <>
struct is_primary<int> : std::true_type {};

// ----------------------------------------------------------------------------

template <typename Type, bool = is_primary<Type>::value>
struct pos_ft;

template <typename Type>
struct pos_ft<Type, false> {
    static const char *show() { return "secondary+"; }
};

template <typename Type>
struct pos_ft<Type, true> {
    static const char *show() { return "primary+"; }
};

// ----------------------------------------------------------------------------

template <typename Type, bool = is_primary<Type>::value>
struct pos_tf;

template <typename Type>
struct pos_tf<Type, true> {
    static const char *show() { return "+primary"; }
};

template <typename Type>
struct pos_tf<Type, false> {
    static const char *show() { return "+secondary"; }
};

// ----------------------------------------------------------------------------

template <typename Type, bool = !is_primary<Type>::value>
struct neg_ft;

template <typename Type>
struct neg_ft<Type, false> {
    static const char *show() { return "primary-"; }
};

template <typename Type>
struct neg_ft<Type, true> {
    static const char *show() { return "secondary-"; }
};

// ----------------------------------------------------------------------------

template <typename Type, bool = !is_primary<Type>::value>
struct neg_tf;

template <typename Type>
struct neg_tf<Type, true> {
    static const char *show() { return "-secondary"; }
};

template <typename Type>
struct neg_tf<Type, false> {
    static const char *show() { return "-primary"; }
};

// ----------------------------------------------------------------------------

template <typename, typename, typename = void, typename = void>
struct four_tp {};

} // namespace pick_alt_struct
} // namespace pybind11_tests

using namespace pybind11_tests::pick_alt_struct;

TEST_SUBMODULE(pick_alt_struct, m) {
    m.def("is_primary_int", []() { return is_primary<int>::value; });
    m.def("is_primary_dbl", []() { return is_primary<double>::value; });

    m.def("pos_ft_int_show", &pos_ft<int>::show);
    m.def("pos_ft_dbl_show", &pos_ft<double>::show);

    m.def("pos_tf_int_show", &pos_tf<int>::show);
    m.def("pos_tf_dbl_show", &pos_tf<double>::show);

    m.def("neg_ft_int_show", &neg_ft<int>::show);
    m.def("neg_ft_dbl_show", &neg_ft<double>::show);

    m.def("neg_tf_int_show", &neg_tf<int>::show);
    m.def("neg_tf_dbl_show", &neg_tf<double>::show);
}
