/*
    tests/test_pickling.cpp -- pickle support

    Copyright (c) 2016 Wenzel Jakob <wenzel.jakob@epfl.ch>
    Copyright (c) 2021 The Pybind Development Team.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include <pybind11/stl.h>

#include "pybind11_tests.h"

#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace exercise_trampoline {

struct SimpleBase {
    int num = 0;
    virtual ~SimpleBase() = default;

    // For compatibility with old clang versions:
    SimpleBase() = default;
    SimpleBase(const SimpleBase &) = default;
};

struct SimpleBaseTrampoline : SimpleBase {};

struct SimpleCppDerived : SimpleBase {};

void wrap(py::module m) {
    py::class_<SimpleBase, SimpleBaseTrampoline>(m, "SimpleBase")
        .def(py::init<>())
        .def_readwrite("num", &SimpleBase::num)
        .def(py::pickle(
            [](const py::object &self) {
                py::dict d;
                if (py::hasattr(self, "__dict__")) {
                    d = self.attr("__dict__");
                }
                return py::make_tuple(self.attr("num"), d);
            },
            [](const py::tuple &t) {
                if (t.size() != 2) {
                    throw std::runtime_error("Invalid state!");
                }
                auto cpp_state = std::unique_ptr<SimpleBase>(new SimpleBaseTrampoline);
                cpp_state->num = t[0].cast<int>();
                auto py_state = t[1].cast<py::dict>();
                return std::make_pair(std::move(cpp_state), py_state);
            }));

    m.def("make_SimpleCppDerivedAsBase",
          []() { return std::unique_ptr<SimpleBase>(new SimpleCppDerived); });
    m.def("check_dynamic_cast_SimpleCppDerived", [](const SimpleBase *base_ptr) {
        return dynamic_cast<const SimpleCppDerived *>(base_ptr) != nullptr;
    });
}

} // namespace exercise_trampoline

namespace exercise_getinitargs_getstate_setstate {

// Exercise `__setstate__[non-constructor]` (see google/pywrapcc#30094), which
// was added to support a pickle protocol as established with Boost.Python
// (in 2002):
//   https://www.boost.org/doc/libs/1_31_0/libs/python/doc/v2/pickle.html
// This mechanism was also adopted for PyCLIF:
//   https://github.com/google/clif/blob/7d388e1de7db5beeb3d7429c18a2776d8188f44f/clif/testing/python/pickle_compatibility.clif
// The code below exercises the pickle protocol intentionally exactly as used
// in PyCLIF, to ensure that pybind11 stays fully compatible with existing
// client code relying on the long-established protocol. (It is impractical to
// change all such client code.)

class StoreTwoWithState {
public:
    StoreTwoWithState(int v0, int v1) : values_{v0, v1}, state_{"blank"} {}
    const std::vector<int> &GetInitArgs() const { return values_; }
    const std::string &GetState() const { return state_; }
    void SetState(const std::string &state) { state_ = state; }

private:
    std::vector<int> values_;
    std::string state_;
};

void wrap(py::module m) {
    py::class_<StoreTwoWithState>(std::move(m), "StoreTwoWithState")
        .def(py::init<int, int>())
        .def("__getinitargs__",
             [](const StoreTwoWithState &self) { return py::tuple(py::cast(self.GetInitArgs())); })
        .def("__getstate__", &StoreTwoWithState::GetState)
        .def(
            "__reduce_ex__",
            [](py::handle self, int /*protocol*/) {
                return py::make_tuple(self.attr("__class__"),
                                      self.attr("__getinitargs__")(),
                                      self.attr("__getstate__")());
            },
            py::arg("protocol") = -1)
        .def(
            "__setstate__[non-constructor]", // See google/pywrapcc#30094 for background.
            [](StoreTwoWithState *self, const std::string &state) { self->SetState(state); },
            py::arg("state"));
}

} // namespace exercise_getinitargs_getstate_setstate

TEST_SUBMODULE(pickling, m) {
    m.def("simple_callable", []() { return 20220426; });

    // test_roundtrip
    class Pickleable {
    public:
        explicit Pickleable(const std::string &value) : m_value(value) {}
        const std::string &value() const { return m_value; }

        void setExtra1(int extra1) { m_extra1 = extra1; }
        void setExtra2(int extra2) { m_extra2 = extra2; }
        int extra1() const { return m_extra1; }
        int extra2() const { return m_extra2; }

    private:
        std::string m_value;
        int m_extra1 = 0;
        int m_extra2 = 0;
    };

    class PickleableNew : public Pickleable {
    public:
        using Pickleable::Pickleable;
    };

    py::class_<Pickleable> pyPickleable(m, "Pickleable");
    pyPickleable.def(py::init<std::string>())
        .def("value", &Pickleable::value)
        .def("extra1", &Pickleable::extra1)
        .def("extra2", &Pickleable::extra2)
        .def("setExtra1", &Pickleable::setExtra1)
        .def("setExtra2", &Pickleable::setExtra2)
        // For details on the methods below, refer to
        // http://docs.python.org/3/library/pickle.html#pickling-class-instances
        .def("__getstate__", [](const Pickleable &p) {
            /* Return a tuple that fully encodes the state of the object */
            return py::make_tuple(p.value(), p.extra1(), p.extra2());
        });
    ignoreOldStyleInitWarnings([&pyPickleable]() {
        pyPickleable.def("__setstate__", [](Pickleable &p, const py::tuple &t) {
            if (t.size() != 3) {
                throw std::runtime_error("Invalid state!");
            }
            /* Invoke the constructor (need to use in-place version) */
            new (&p) Pickleable(t[0].cast<std::string>());

            /* Assign any additional state */
            p.setExtra1(t[1].cast<int>());
            p.setExtra2(t[2].cast<int>());
        });
    });

    py::class_<PickleableNew, Pickleable>(m, "PickleableNew")
        .def(py::init<std::string>())
        .def(py::pickle(
            [](const PickleableNew &p) {
                return py::make_tuple(p.value(), p.extra1(), p.extra2());
            },
            [](const py::tuple &t) {
                if (t.size() != 3) {
                    throw std::runtime_error("Invalid state!");
                }
                auto p = PickleableNew(t[0].cast<std::string>());

                p.setExtra1(t[1].cast<int>());
                p.setExtra2(t[2].cast<int>());
                return p;
            }));

#if !defined(PYPY_VERSION)
    // test_roundtrip_with_dict
    class PickleableWithDict {
    public:
        explicit PickleableWithDict(const std::string &value) : value(value) {}

        std::string value;
        int extra;
    };

    class PickleableWithDictNew : public PickleableWithDict {
    public:
        using PickleableWithDict::PickleableWithDict;
    };

    py::class_<PickleableWithDict> pyPickleableWithDict(
        m, "PickleableWithDict", py::dynamic_attr());
    pyPickleableWithDict.def(py::init<std::string>())
        .def_readwrite("value", &PickleableWithDict::value)
        .def_readwrite("extra", &PickleableWithDict::extra)
        .def("__getstate__", [](const py::object &self) {
            /* Also include __dict__ in state */
            return py::make_tuple(self.attr("value"), self.attr("extra"), self.attr("__dict__"));
        });
    ignoreOldStyleInitWarnings([&pyPickleableWithDict]() {
        pyPickleableWithDict.def("__setstate__", [](const py::object &self, const py::tuple &t) {
            if (t.size() != 3) {
                throw std::runtime_error("Invalid state!");
            }
            /* Cast and construct */
            auto &p = self.cast<PickleableWithDict &>();
            new (&p) PickleableWithDict(t[0].cast<std::string>());

            /* Assign C++ state */
            p.extra = t[1].cast<int>();

            /* Assign Python state */
            self.attr("__dict__") = t[2];
        });
    });

    py::class_<PickleableWithDictNew, PickleableWithDict>(m, "PickleableWithDictNew")
        .def(py::init<std::string>())
        .def(py::pickle(
            [](const py::object &self) {
                return py::make_tuple(
                    self.attr("value"), self.attr("extra"), self.attr("__dict__"));
            },
            [](const py::tuple &t) {
                if (t.size() != 3) {
                    throw std::runtime_error("Invalid state!");
                }

                auto cpp_state = PickleableWithDictNew(t[0].cast<std::string>());
                cpp_state.extra = t[1].cast<int>();

                auto py_state = t[2].cast<py::dict>();
                return std::make_pair(cpp_state, py_state);
            }));
#endif

    exercise_trampoline::wrap(m);
    exercise_getinitargs_getstate_setstate::wrap(m);
}
