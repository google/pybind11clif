// Copyright (c) 2024 The pybind Community.

// **WARNING:**
// The `as_void_ptr_capsule` feature is needed for PyCLIF-SWIG interoperability
// in the Google-internal environment, but the current implementation is lacking
// any safety checks.

// IWYU pragma: private, include "third_party/pybind11/include/pybind11/pybind11.h"

#pragma once

#include "../pytypes.h"
#include "common.h"
#include "typeid.h"

#include <string>

PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)
PYBIND11_NAMESPACE_BEGIN(detail)

// Replace all occurrences of substrings in a string.
inline void replace_all(std::string &str, const std::string &from, const std::string &to) {
    if (str.empty()) {
        return;
    }
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

// Forward declaration needed here: Refactoring opportunity.
extern "C" inline PyObject *pybind11_object_new(PyTypeObject *type, PyObject *, PyObject *);

inline bool type_is_pybind11_class_(PyTypeObject *type_obj) {
#if defined(PYPY_VERSION)
    auto &internals = get_internals();
    return bool(internals.registered_types_py.find(type_obj)
                != internals.registered_types_py.end());
#else
    return bool(type_obj->tp_new == pybind11_object_new);
#endif
}

inline bool is_instance_method_of_type(PyTypeObject *type_obj, PyObject *attr_name) {
    PyObject *descr = _PyType_Lookup(type_obj, attr_name);
    return bool((descr != nullptr) && PyInstanceMethod_Check(descr));
}

inline object try_get_as_capsule_method(PyObject *obj, PyObject *attr_name) {
    if (PyType_Check(obj)) {
        return object();
    }
    PyTypeObject *type_obj = Py_TYPE(obj);
    bool known_callable = false;
    if (type_is_pybind11_class_(type_obj)) {
        if (!is_instance_method_of_type(type_obj, attr_name)) {
            return object();
        }
        known_callable = true;
    }
    PyObject *method = PyObject_GetAttr(obj, attr_name);
    if (method == nullptr) {
        PyErr_Clear();
        return object();
    }
    if (!known_callable && PyCallable_Check(method) == 0) {
        Py_DECREF(method);
        return object();
    }
    return reinterpret_steal<object>(method);
}

inline void *try_as_void_ptr_capsule_get_pointer(handle src, const char *typeid_name) {
    std::string suffix = clean_type_id(typeid_name);
    replace_all(suffix, "::", "_"); // Convert `a::b::c` to `a_b_c`.
    replace_all(suffix, "*", "");
    object as_capsule_method = try_get_as_capsule_method(src.ptr(), str("as_" + suffix).ptr());
    if (as_capsule_method) {
        object void_ptr_capsule = as_capsule_method();
        if (isinstance<capsule>(void_ptr_capsule)) {
            return reinterpret_borrow<capsule>(void_ptr_capsule).get_pointer();
        }
    }
    return nullptr;
}

#define PYBIND11_HAS_TRY_AS_VOID_PTR_CAPSULE_GET_POINTER

PYBIND11_NAMESPACE_END(detail)
PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
