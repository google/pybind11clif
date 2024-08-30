// THIS MUST STAY AT THE TOP!
// DO NOT USE ANY OTHER pybind11 HEADERS HERE!
#include <pybind11/detail/platform_abi_id.h>

#include "test_cpp_transporter_traveler_types.h"

#include <Python.h>
#include <typeinfo>

namespace {

void *get_cpp_transporter_void_ptr(PyObject *py_obj, const std::type_info *cpp_type_info) {
    PyObject *cap_cpp_type_info
        = PyCapsule_New(const_cast<void *>(static_cast<const void *>(cpp_type_info)),
                        "const std::type_info *",
                        nullptr);
    if (cap_cpp_type_info == nullptr) {
        return nullptr;
    }
    PyObject *cpp_transporter = PyObject_CallMethod(py_obj,
                                                    "__cpp_transporter__",
                                                    "sOs",
                                                    PYBIND11_PLATFORM_ABI_ID,
                                                    cap_cpp_type_info,
                                                    "raw_pointer_ephemeral");
    Py_DECREF(cap_cpp_type_info);
    if (cpp_transporter == nullptr) {
        return nullptr;
    }
    void *void_ptr = PyCapsule_GetPointer(cpp_transporter, cpp_type_info->name());
    Py_DECREF(cpp_transporter);
    if (PyErr_Occurred()) {
        return nullptr;
    }
    return void_ptr;
}

template <typename T>
T *get_cpp_transporter_type_ptr(PyObject *py_obj) {
    void *void_ptr = get_cpp_transporter_void_ptr(py_obj, &typeid(T));
    if (void_ptr == nullptr) {
        return nullptr;
    }
    return static_cast<T *>(void_ptr);
}

extern "C" PyObject *wrapGetLuggage(PyObject * /*self*/, PyObject *traveler) {
    const auto *cpp_traveler
        = get_cpp_transporter_type_ptr<pybind11_tests::test_cpp_transporter::Traveler>(traveler);
    if (cpp_traveler == nullptr) {
        return nullptr;
    }
    return PyUnicode_FromString(cpp_traveler->luggage.c_str());
}

extern "C" PyObject *wrapGetPoints(PyObject * /*self*/, PyObject *premium_traveler) {
    const auto *cpp_premium_traveler
        = get_cpp_transporter_type_ptr<pybind11_tests::test_cpp_transporter::PremiumTraveler>(
            premium_traveler);
    if (cpp_premium_traveler == nullptr) {
        return nullptr;
    }
    return PyLong_FromLong(static_cast<long>(cpp_premium_traveler->points));
}

PyMethodDef ThisMethodDef[] = {{"GetLuggage", wrapGetLuggage, METH_O, nullptr},
                               {"GetPoints", wrapGetPoints, METH_O, nullptr},
                               {nullptr, nullptr, 0, nullptr}};

struct PyModuleDef ThisModuleDef = {
    PyModuleDef_HEAD_INIT, // m_base
    "exo_planet_c_api",    // m_name
    nullptr,               // m_doc
    -1,                    // m_size
    ThisMethodDef,         // m_methods
    nullptr,               // m_slots
    nullptr,               // m_traverse
    nullptr,               // m_clear
    nullptr                // m_free
};

} // namespace

#if defined(WIN32) || defined(_WIN32)
#    define EXO_PLANET_C_API_EXPORT __declspec(dllexport)
#else
#    define EXO_PLANET_C_API_EXPORT __attribute__((visibility("default")))
#endif

extern "C" EXO_PLANET_C_API_EXPORT PyObject *PyInit_exo_planet_c_api() {
    PyObject *m = PyModule_Create(&ThisModuleDef);
    if (m == nullptr) {
        return nullptr;
    }
    return m;
}
