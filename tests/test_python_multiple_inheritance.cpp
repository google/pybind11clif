#include "pybind11_tests.h"

namespace test_python_multiple_inheritance {

// Copied from:
// https://github.com/google/clif/blob/5718e4d0807fd3b6a8187dde140069120b81ecef/clif/testing/python_multiple_inheritance.h

template <int> // Using int as a trick to easily generate a series of types.
struct CppBase {
    explicit CppBase(int value) : base_value(value) {}
    int get_base_value() const { return base_value; }
    void reset_base_value(int new_value) { base_value = new_value; }

private:
    int base_value;
};

template <int SerNo>
struct CppDrvd : CppBase<SerNo> {
    explicit CppDrvd(int value) : CppBase<SerNo>(value), drvd_value(value * 3) {}
    int get_drvd_value() const { return drvd_value; }
    void reset_drvd_value(int new_value) { drvd_value = new_value; }

    int get_base_value_from_drvd() const { return CppBase<SerNo>::get_base_value(); }
    void reset_base_value_from_drvd(int new_value) { CppBase<SerNo>::reset_base_value(new_value); }

private:
    int drvd_value;
};

template <int SerNo>
struct CppDrvd2 : CppBase<SerNo> {
    explicit CppDrvd2(int value) : CppBase<SerNo>(value), drvd2_value(value * 5) {}
    int get_drvd2_value() const { return drvd2_value; }
    void reset_drvd2_value(int new_value) { drvd2_value = new_value; }

    int get_base_value_from_drvd2() const { return CppBase<SerNo>::get_base_value(); }
    void reset_base_value_from_drvd2(int new_value) {
        CppBase<SerNo>::reset_base_value(new_value);
    }

private:
    int drvd2_value;
};

template <int SerNo, typename... Extra>
void wrap_classes(py::module_ &m,
                  const char *name_base,
                  const char *name_drvd,
                  const char *name_drvd2,
                  const char *pass_base,
                  Extra... extra) {
    py::class_<CppBase<SerNo>>(m, name_base, std::forward<Extra>(extra)...)
        .def(py::init<int>())
        .def("get_base_value", &CppBase<SerNo>::get_base_value)
        .def("reset_base_value", &CppBase<SerNo>::reset_base_value);

    py::class_<CppDrvd<SerNo>, CppBase<SerNo>>(m, name_drvd, std::forward<Extra>(extra)...)
        .def(py::init<int>())
        .def("get_drvd_value", &CppDrvd<SerNo>::get_drvd_value)
        .def("reset_drvd_value", &CppDrvd<SerNo>::reset_drvd_value)
        .def("get_base_value_from_drvd", &CppDrvd<SerNo>::get_base_value_from_drvd)
        .def("reset_base_value_from_drvd", &CppDrvd<SerNo>::reset_base_value_from_drvd);

    py::class_<CppDrvd2<SerNo>, CppBase<SerNo>>(m, name_drvd2, std::forward<Extra>(extra)...)
        .def(py::init<int>())
        .def("get_drvd2_value", &CppDrvd2<SerNo>::get_drvd2_value)
        .def("reset_drvd2_value", &CppDrvd2<SerNo>::reset_drvd2_value)
        .def("get_base_value_from_drvd2", &CppDrvd2<SerNo>::get_base_value_from_drvd2)
        .def("reset_base_value_from_drvd2", &CppDrvd2<SerNo>::reset_base_value_from_drvd2);

    m.def(pass_base, [](const CppBase<SerNo> *) {});
}

} // namespace test_python_multiple_inheritance

TEST_SUBMODULE(python_multiple_inheritance, m) {
    using namespace test_python_multiple_inheritance;
    wrap_classes<0>(m, "CppBase0", "CppDrvd0", "CppDrvd20", "pass_CppBase0");
    wrap_classes<1>(m,
                    "CppBase1",
                    "CppDrvd1",
                    "CppDrvd21",
                    "pass_CppBase1",
                    py::metaclass((PyObject *) &PyType_Type));

    m.attr("if_defined_PYBIND11_INIT_SAFETY_CHECKS_VIA_DEFAULT_PYBIND11_METACLASS") =
#if defined(PYBIND11_INIT_SAFETY_CHECKS_VIA_DEFAULT_PYBIND11_METACLASS)
        true;
#else
        false;
#endif
}
