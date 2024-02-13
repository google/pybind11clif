// Copyright (c) 2024 The Pybind Development Team.
// All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

#pragma once

#include "../attr.h"
#include "../pytypes.h"
#include "common.h"

PYBIND11_NAMESPACE_BEGIN(PYBIND11_NAMESPACE)
PYBIND11_NAMESPACE_BEGIN(detail)

struct function_record_PyObject {
    PyObject_HEAD
    function_record *cpp_func_rec;
};

PYBIND11_NAMESPACE_BEGIN(function_record_PyTypeObject_methods)

PyObject *tp_new_impl(PyTypeObject *type, PyObject *args, PyObject *kwds);
PyObject *tp_alloc_impl(PyTypeObject *type, Py_ssize_t nitems);
int tp_init_impl(PyObject *self, PyObject *args, PyObject *kwds);
void tp_dealloc_impl(PyObject *self);
void tp_free_impl(void *self);

static PyObject *reduce_ex_impl(PyObject *self, PyObject *, PyObject *);

PYBIND11_WARNING_PUSH
#if defined(__GNUC__) && __GNUC__ >= 8
PYBIND11_WARNING_DISABLE_GCC("-Wcast-function-type")
#endif
static PyMethodDef tp_methods_impl[]
    = {{"__reduce_ex__", (PyCFunction) reduce_ex_impl, METH_VARARGS | METH_KEYWORDS, nullptr},
       {nullptr, nullptr, 0, nullptr}};
PYBIND11_WARNING_POP

PYBIND11_NAMESPACE_END(function_record_PyTypeObject_methods)

// Designated initializers are a C++20 feature:
// https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers
// MSVC rejects them unless /std:c++20 is used (error code C7555).
PYBIND11_WARNING_PUSH
PYBIND11_WARNING_DISABLE_CLANG("-Wmissing-field-initializers")
#if defined(__GNUC__) && __GNUC__ >= 8
PYBIND11_WARNING_DISABLE_GCC("-Wmissing-field-initializers")
#endif
static PyTypeObject function_record_PyTypeObject = {
    PyVarObject_HEAD_INIT(nullptr, 0)
    /* const char *tp_name */ "pybind11_detail_function_"
                              "record_" PYBIND11_DETAIL_FUNCTION_RECORD_ABI_ID
                              "_" PYBIND11_PLATFORM_ABI_ID_V4,
    /* Py_ssize_t tp_basicsize */ sizeof(function_record_PyObject),
    /* Py_ssize_t tp_itemsize */ 0,
    /* destructor tp_dealloc */ function_record_PyTypeObject_methods::tp_dealloc_impl,
    /* Py_ssize_t tp_vectorcall_offset */ 0,
    /* getattrfunc tp_getattr */ nullptr,
    /* setattrfunc tp_setattr */ nullptr,
    /* PyAsyncMethods *tp_as_async */ nullptr,
    /* reprfunc tp_repr */ nullptr,
    /* PyNumberMethods *tp_as_number */ nullptr,
    /* PySequenceMethods *tp_as_sequence */ nullptr,
    /* PyMappingMethods *tp_as_mapping */ nullptr,
    /* hashfunc tp_hash */ nullptr,
    /* ternaryfunc tp_call */ nullptr,
    /* reprfunc tp_str */ nullptr,
    /* getattrofunc tp_getattro */ nullptr,
    /* setattrofunc tp_setattro */ nullptr,
    /* PyBufferProcs *tp_as_buffer */ nullptr,
    /* unsigned long tp_flags */ Py_TPFLAGS_DEFAULT,
    /* const char *tp_doc */ nullptr,
    /* traverseproc tp_traverse */ nullptr,
    /* inquiry tp_clear */ nullptr,
    /* richcmpfunc tp_richcompare */ nullptr,
    /* Py_ssize_t tp_weaklistoffset */ 0,
    /* getiterfunc tp_iter */ nullptr,
    /* iternextfunc tp_iternext */ nullptr,
    /* struct PyMethodDef *tp_methods */ function_record_PyTypeObject_methods::tp_methods_impl,
    /* struct PyMemberDef *tp_members */ nullptr,
    /* struct PyGetSetDef *tp_getset */ nullptr,
    /* struct _typeobject *tp_base */ nullptr,
    /* PyObject *tp_dict */ nullptr,
    /* descrgetfunc tp_descr_get */ nullptr,
    /* descrsetfunc tp_descr_set */ nullptr,
    /* Py_ssize_t tp_dictoffset */ 0,
    /* initproc tp_init */ function_record_PyTypeObject_methods::tp_init_impl,
    /* allocfunc tp_alloc */ function_record_PyTypeObject_methods::tp_alloc_impl,
    /* newfunc tp_new */ function_record_PyTypeObject_methods::tp_new_impl,
    /* freefunc tp_free */ function_record_PyTypeObject_methods::tp_free_impl,
    /* inquiry tp_is_gc */ nullptr,
    /* PyObject *tp_bases */ nullptr,
    /* PyObject *tp_mro */ nullptr,
    /* PyObject *tp_cache */ nullptr,
    /* PyObject *tp_subclasses */ nullptr,
    /* PyObject *tp_weaklist */ nullptr,
    /* destructor tp_del */ nullptr,
    /* unsigned int tp_version_tag */ 0,
    /* destructor tp_finalize */ nullptr,
#if PY_VERSION_HEX >= 0x03080000
    /* vectorcallfunc tp_vectorcall */ nullptr,
#endif
};
PYBIND11_WARNING_POP

static bool function_record_PyTypeObject_PyType_Ready_first_call = true;

inline void function_record_PyTypeObject_PyType_Ready() {
    if (function_record_PyTypeObject_PyType_Ready_first_call) {
        if (PyType_Ready(&function_record_PyTypeObject) < 0) {
            throw error_already_set();
        }
        function_record_PyTypeObject_PyType_Ready_first_call = false;
    }
}

inline bool is_function_record_PyObject(PyObject *obj) {
    if (PyType_Check(obj) != 0) {
        return false;
    }
    PyTypeObject *obj_type = Py_TYPE(obj);
    // Fast path (pointer comparison).
    if (obj_type == &function_record_PyTypeObject) {
        return true;
    }
    // This works across extension modules. Note that tp_name is versioned.
    if (strcmp(obj_type->tp_name, function_record_PyTypeObject.tp_name) == 0) {
        return true;
    }
    return false;
}

inline function_record *function_record_ptr_from_PyObject(PyObject *obj) {
    if (is_function_record_PyObject(obj)) {
        return ((detail::function_record_PyObject *) obj)->cpp_func_rec;
    }
    return nullptr;
}

inline object function_record_PyObject_New() {
    auto *py_func_rec = PyObject_New(function_record_PyObject, &function_record_PyTypeObject);
    if (py_func_rec == nullptr) {
        throw error_already_set();
    }
    py_func_rec->cpp_func_rec = nullptr; // For clarity/purity. Redundant in practice.
    return reinterpret_steal<object>((PyObject *) py_func_rec);
}

// The implementation needs the definition of `class module_`.
PyObject *function_record_pickle_helper(PyObject *, PyObject *tup_ptr);

constexpr char function_record_pickle_helper_name[] = "_function_record_pickle_helper_v1";

static PyMethodDef function_record_pickle_helper_PyMethodDef
    = {function_record_pickle_helper_name,
       (PyCFunction) function_record_pickle_helper,
       METH_O,
       nullptr};

inline object get_function_record_pickle_helper(handle mod) {
    if (!hasattr(mod, function_record_pickle_helper_name)) {
        PyObject *pycfunc = PyCFunction_New(&function_record_pickle_helper_PyMethodDef, nullptr);
        if (!pycfunc) {
            pybind11_fail("FATAL: get_function_record_pickle_helper() PyCFunction_NewEx() FAILED");
        }
        if (PyModule_AddObject(
                mod.ptr(), function_record_pickle_helper_name, pycfunc /* steals a reference */)
            < 0) {
            Py_DECREF(pycfunc);
            pybind11_fail(
                "FATAL: get_function_record_pickle_helper() PyModule_AddObject() FAILED");
        }
    }
    return getattr(mod, function_record_pickle_helper_name);
}

PYBIND11_NAMESPACE_BEGIN(function_record_PyTypeObject_methods)

inline PyObject *tp_new_impl(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    // Create a new instance using the type's tp_alloc slot.
    if (type) {
        pybind11_fail("UNEXPECTED CALL OF function_record_PyTypeObject_methods::tp_new_impl");
    }
    return PyType_GenericNew(type, args, kwds);
}

inline PyObject *tp_alloc_impl(PyTypeObject *type, Py_ssize_t nitems) {
    // Use Python's default memory allocation mechanism to allocate a new instance
    // and initialize all its contents to NULL.
    if (type) {
        pybind11_fail("UNEXPECTED CALL OF function_record_PyTypeObject_methods::tp_alloc_impl");
    }
    return PyType_GenericAlloc(type, nitems);
}

inline int tp_init_impl(PyObject *self, PyObject *, PyObject *) {
    if (self) {
        pybind11_fail("UNEXPECTED CALL OF function_record_PyTypeObject_methods::tp_init_impl");
    }
    return -1;
}

// The implementation needs the definition of `class cpp_function`.
void tp_dealloc_impl(PyObject *self);

inline void tp_free_impl(void *self) {
    if (self) {
        pybind11_fail("UNEXPECTED CALL OF function_record_PyTypeObject_methods::tp_free_impl");
    }
}

inline PyObject *reduce_ex_impl(PyObject *self, PyObject *, PyObject *) {
    // Deliberately ignoring the arguments for simplicity (expected is `protocol: int`).
    const function_record *rec = function_record_ptr_from_PyObject(self);
    if (rec == nullptr) {
        pybind11_fail(
            "FATAL: function_record_PyTypeObject reduce_ex_impl(): cannot obtain cpp_func_rec.");
    }
    if (rec->name != nullptr && rec->name[0] != '\0' && rec->scope) {
        // TODO 30099: Move to helper function.
        object scope_module;
        if (hasattr(rec->scope, "__module__")) {
            scope_module = rec->scope.attr("__module__");
        } else if (hasattr(rec->scope, "__name__")) {
            scope_module = rec->scope.attr("__name__");
        }
        if (scope_module && PyModule_Check(rec->scope.ptr()) != 0) {
            return make_tuple(get_function_record_pickle_helper(rec->scope),
                              make_tuple(make_tuple(scope_module, rec->name)))
                .release()
                .ptr();
        }
    }
    set_error(PyExc_RuntimeError, repr(self) + str(" is not pickleable."));
    return nullptr;
}

PYBIND11_NAMESPACE_END(function_record_PyTypeObject_methods)

PYBIND11_NAMESPACE_END(detail)
PYBIND11_NAMESPACE_END(PYBIND11_NAMESPACE)
