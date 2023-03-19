#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include "pybind11_tests.h"

#include <array>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#if defined(PYBIND11_HAS_OPTIONAL)
#    include <optional>
#endif

#if defined(PYBIND11_HAS_VARIANT)
#    include <variant>
#endif

namespace {

using PairString = std::pair<std::string, std::string>;

PairString return_pair_string() { return PairString({"", ""}); }

using NestedPairString = std::pair<PairString, PairString>;

NestedPairString return_nested_pair_string() {
    return NestedPairString(return_pair_string(), return_pair_string());
}

using MapString = std::map<std::string, std::string>;

MapString return_map_string() { return MapString({return_pair_string()}); }

using MapPairString = std::map<PairString, PairString>;

MapPairString return_map_pair_string() { return MapPairString({return_nested_pair_string()}); }

using SetPairString = std::set<PairString>;

SetPairString return_set_pair_string() { return SetPairString({return_pair_string()}); }

using VectorPairString = std::vector<PairString>;

VectorPairString return_vector_pair_string() { return VectorPairString({return_pair_string()}); }

using ArrayPairString = std::array<PairString, 1>;

ArrayPairString return_array_pair_string() { return ArrayPairString({{return_pair_string()}}); }

#if defined(PYBIND11_HAS_OPTIONAL)

using OptionalPairString = std::optional<PairString>;

OptionalPairString return_optional_pair_string() {
    return OptionalPairString(return_pair_string());
}

#endif // PYBIND11_HAS_OPTIONAL

#if defined(PYBIND11_HAS_VARIANT)

using VariantPairString = std::variant<PairString>;

VariantPairString return_variant_pair_string() { return VariantPairString(return_pair_string()); }

#endif // PYBIND11_HAS_VARIANT

std::string call_callback_pass_pair_string(const std::function<std::string(PairString)> &cb) {
    auto p = return_pair_string();
    return cb(p);
}

std::string level_0_si(int num) { return "level_0_si_" + std::to_string(num); }
int level_0_is(const std::string &s) { return 100 + std::atoi(s.c_str()); }

using level_1_callback_si = std::function<std::string(int)>;
using level_2_callback_si = std::function<std::string(level_1_callback_si)>;
using level_3_callback_si = std::function<std::string(level_2_callback_si)>;
using level_4_callback_si = std::function<std::string(level_3_callback_si)>;

using level_1_callback_is = std::function<int(std::string)>;
using level_2_callback_is = std::function<int(level_1_callback_is)>;
using level_3_callback_is = std::function<int(level_2_callback_is)>;
using level_4_callback_is = std::function<int(level_3_callback_is)>;

std::string call_level_1_callback_si(const level_1_callback_si &cb) { return cb(1001); }
std::string call_level_2_callback_si(const level_2_callback_si &cb) { return cb(level_0_si); }
std::string call_level_3_callback_si(const level_3_callback_si &cb) {
    return cb(call_level_1_callback_si);
}
std::string call_level_4_callback_si(const level_4_callback_si &cb) {
    return cb(call_level_2_callback_si);
}

int call_level_1_callback_is(const level_1_callback_is &cb) { return cb("101"); }
int call_level_2_callback_is(const level_2_callback_is &cb) { return cb(level_0_is); }
int call_level_3_callback_is(const level_3_callback_is &cb) {
    return cb(call_level_1_callback_is);
}
int call_level_4_callback_is(const level_4_callback_is &cb) {
    return cb(call_level_2_callback_is);
}

struct VirtualBase {
    VirtualBase() = default;
    virtual ~VirtualBase() = default;

    virtual int pure() const = 0;
    virtual int pure_s(const std::string & /*a0*/) const = 0;
    virtual int pure_b(const std::string & /*a0*/) const = 0;
    virtual int pure_sb(const std::string & /*a0*/, const std::string & /*a1*/) const = 0;
    virtual int pure_bs(const std::string & /*a0*/, const std::string & /*a1*/) const = 0;

    virtual int nonp() const { return -8; }
    virtual int nonp_s(const std::string & /*a0*/) const { return -20; }
    virtual int nonp_b(const std::string & /*a0*/) const { return -21; }
    virtual int nonp_sb(const std::string & /*a0*/, const std::string & /*a1*/) const {
        return -201;
    }
    virtual int nonp_bs(const std::string & /*a0*/, const std::string & /*a1*/) const {
        return -210;
    }
};

struct VirtualBaseTrampoline : VirtualBase {
    static constexpr auto rvpc = py::return_value_policy::_clif_automatic;
    static constexpr auto rvpb = py::return_value_policy::_return_as_bytes;

    int pure() const override {
        PYBIND11_OVERRIDE_PURE_NAME_RVPP(int, VirtualBase, "py_pure", pure, rvpc);
    }
    int pure_s(const std::string &a0) const override {
        PYBIND11_OVERRIDE_PURE_NAME_RVPP(int, VirtualBase, "py_pure_s", pure_s, rvpc, a0);
    }
    int pure_b(const std::string &a0) const override {
        PYBIND11_OVERRIDE_PURE_NAME_RVPP(int, VirtualBase, "py_pure_b", pure_b, rvpb, a0);
    }
    int pure_sb(const std::string &a0, const std::string &a1) const override {
        PYBIND11_OVERRIDE_PURE_NAME_RVPP(int,
                                         VirtualBase,
                                         "py_pure_sb",
                                         pure_sb,
                                         py::return_value_policy_pack({rvpc, rvpb}),
                                         a0,
                                         a1);
    }
    int pure_bs(const std::string &a0, const std::string &a1) const override {
        PYBIND11_OVERRIDE_PURE_NAME_RVPP(int,
                                         VirtualBase,
                                         "py_pure_bs",
                                         pure_bs,
                                         py::return_value_policy_pack({rvpb, rvpc}),
                                         a0,
                                         a1);
    }

    int nonp() const override {
        PYBIND11_OVERRIDE_NAME_RVPP(int, VirtualBase, "py_nonp", nonp, rvpc);
    }
    int nonp_s(const std::string &a0) const override {
        PYBIND11_OVERRIDE_NAME_RVPP(int, VirtualBase, "py_nonp_s", nonp_s, rvpc, a0);
    }
    int nonp_b(const std::string &a0) const override {
        PYBIND11_OVERRIDE_NAME_RVPP(int, VirtualBase, "py_nonp_b", nonp_b, rvpb, a0);
    }
    int nonp_sb(const std::string &a0, const std::string &a1) const override {
        PYBIND11_OVERRIDE_NAME_RVPP(int,
                                    VirtualBase,
                                    "py_nonp_sb",
                                    nonp_sb,
                                    py::return_value_policy_pack({rvpc, rvpb}),
                                    a0,
                                    a1);
    }
    int nonp_bs(const std::string &a0, const std::string &a1) const override {
        PYBIND11_OVERRIDE_NAME_RVPP(int,
                                    VirtualBase,
                                    "py_nonp_bs",
                                    nonp_bs,
                                    py::return_value_policy_pack({rvpb, rvpc}),
                                    a0,
                                    a1);
    }
};

int call_virtual_override(const VirtualBase &base, const std::string &which) {
    if (which == "pure") {
        return base.pure();
    }
    if (which == "pure_s") {
        return base.pure_s("");
    }
    if (which == "pure_b") {
        return base.pure_b("");
    }
    if (which == "pure_sb") {
        return base.pure_sb("", "");
    }
    if (which == "pure_bs") {
        return base.pure_bs("", "");
    }
    if (which == "nonp") {
        return base.nonp();
    }
    if (which == "nonp_s") {
        return base.nonp_s("");
    }
    if (which == "nonp_b") {
        return base.nonp_b("");
    }
    if (which == "nonp_sb") {
        return base.nonp_sb("", "");
    }
    if (which == "nonp_bs") {
        return base.nonp_bs("", "");
    }
    return -99; // Invalid which.
}

} // namespace

TEST_SUBMODULE(return_value_policy_pack, m) {
    static constexpr auto rvpc = py::return_value_policy::_clif_automatic;
    static constexpr auto rvpb = py::return_value_policy::_return_as_bytes;

    m.def("return_tuple_str_str", []() { return return_pair_string(); });
    m.def(
        "return_tuple_bytes_bytes", []() { return return_pair_string(); }, rvpb);
    m.def(
        "return_tuple_str_bytes",
        []() { return return_pair_string(); },
        py::return_value_policy_pack({rvpc, rvpb}));
    m.def(
        "return_tuple_bytes_str",
        []() {
            // Ensure cast(T*, ...) overload supports rvpp:
            static auto *p = new PairString(return_pair_string());
            return p;
        },
        py::return_value_policy_pack({rvpb, rvpc}));

    m.def("cast_tuple_str_str", []() {
        return py::cast(return_pair_string(), py::return_value_policy_pack({rvpc, rvpc}));
    });
    m.def("cast_tuple_bytes_bytes", []() {
        return py::cast(return_pair_string(), py::return_value_policy_pack({rvpb, rvpb}));
    });
    m.def("cast_tuple_str_bytes", []() {
        return py::cast(return_pair_string(), py::return_value_policy_pack({rvpc, rvpb}));
    });
    m.def("cast_tuple_bytes_str", []() {
        return py::cast(return_pair_string(), py::return_value_policy_pack({rvpb, rvpc}));
    });

    m.def("return_nested_tuple_str", []() { return return_nested_pair_string(); });
    m.def(
        "return_nested_tuple_bytes", []() { return return_nested_pair_string(); }, rvpb);
    m.def(
        "return_nested_tuple_str_bytes_bytes_str",
        []() { return return_nested_pair_string(); },
        py::return_value_policy_pack({{rvpc, rvpb}, {rvpb, rvpc}}));
    m.def(
        "return_nested_tuple_bytes_str_str_bytes",
        []() { return return_nested_pair_string(); },
        py::return_value_policy_pack({{rvpb, rvpc}, {rvpc, rvpb}}));

    m.def("return_dict_str_str", []() { return return_map_string(); });
    m.def(
        "return_dict_bytes_bytes", []() { return return_map_string(); }, rvpb);
    m.def(
        "return_dict_str_bytes",
        []() { return return_map_string(); },
        py::return_value_policy_pack({rvpc, rvpb}));
    m.def(
        "return_dict_bytes_str",
        []() {
            // Ensure cast(T*, ...) overload supports rvpp:
            static auto *d = new MapString(return_map_string());
            return d;
        },
        py::return_value_policy_pack({rvpb, rvpc}));

    m.def(
        "return_map_sbbs",
        []() { return return_map_pair_string(); },
        py::return_value_policy_pack({{rvpc, rvpb}, {rvpb, rvpc}}));
    m.def(
        "return_map_bssb",
        []() { return return_map_pair_string(); },
        py::return_value_policy_pack({{rvpb, rvpc}, {rvpc, rvpb}}));

    m.def(
        "return_set_sb",
        []() { return return_set_pair_string(); },
        py::return_value_policy_pack({rvpc, rvpb}));
    m.def(
        "return_set_bs",
        []() {
            // Ensure cast(T*, ...) overload supports rvpp:
            static auto *s = new SetPairString(return_set_pair_string());
            return s;
        },
        py::return_value_policy_pack({rvpb, rvpc}));

    m.def(
        "return_vector_sb",
        []() { return return_vector_pair_string(); },
        py::return_value_policy_pack({rvpc, rvpb}));
    m.def(
        "return_vector_bs",
        []() {
            // Ensure cast(T*, ...) overload supports rvpp:
            static auto *v = new VectorPairString(return_vector_pair_string());
            return v;
        },
        py::return_value_policy_pack({rvpb, rvpc}));

    m.def(
        "return_array_sb",
        []() { return return_array_pair_string(); },
        py::return_value_policy_pack({rvpc, rvpb}));
    m.def(
        "return_array_bs",
        []() {
            // Ensure cast(T*, ...) overload supports rvpp:
            static auto *a = new ArrayPairString(return_array_pair_string());
            return a;
        },
        py::return_value_policy_pack({rvpb, rvpc}));

    m.attr("PYBIND11_HAS_OPTIONAL") =
#if !defined(PYBIND11_HAS_OPTIONAL)
        false;
#else
        true;
    m.def(
        "return_optional_sb",
        []() { return return_optional_pair_string(); },
        py::return_value_policy_pack({rvpc, rvpb}));
    m.def(
        "return_optional_bs",
        []() {
            // Ensure cast(T*, ...) overload supports rvpp:
            static auto *o = new OptionalPairString(return_optional_pair_string());
            return o;
        },
        py::return_value_policy_pack({rvpb, rvpc}));
#endif

    m.attr("PYBIND11_HAS_VARIANT") =
#if !defined(PYBIND11_HAS_VARIANT)
        false;
#else
        true;
    m.def(
        "return_variant_sb",
        []() { return return_variant_pair_string(); },
        py::return_value_policy_pack({rvpc, rvpb}));
    m.def(
        "return_variant_bs",
        []() {
            // Ensure cast(T*, ...) overload supports rvpp:
            static auto *v = new VariantPairString(return_variant_pair_string());
            return v;
        },
        py::return_value_policy_pack({rvpb, rvpc}));
#endif

    auto arg_cb_b = py::arg("cb").policies(py::return_value_policy_pack(rvpb));

    m.def("call_callback_pass_pair_default", call_callback_pass_pair_string, py::arg("cb"));
    m.def("call_callback_pass_pair_s",
          call_callback_pass_pair_string,
          py::arg("cb").policies(py::return_value_policy_pack(rvpc)));
    m.def("call_callback_pass_pair_b", call_callback_pass_pair_string, arg_cb_b);
    m.def("call_callback_pass_pair_sb",
          call_callback_pass_pair_string,
          py::arg("cb").policies(py::return_value_policy_pack({{rvpc, rvpb}})));
    m.def("call_callback_pass_pair_bs",
          call_callback_pass_pair_string,
          py::arg("cb").policies(py::return_value_policy_pack({{rvpb, rvpc}})));

    m.def("call_level_1_callback_si_s", call_level_1_callback_si);
    m.def("call_level_2_callback_si_s", call_level_2_callback_si);
    m.def("call_level_3_callback_si_s", call_level_3_callback_si);
    m.def("call_level_4_callback_si_s", call_level_4_callback_si);

    m.def("call_level_1_callback_si_b", call_level_1_callback_si, arg_cb_b, rvpb);
    m.def("call_level_2_callback_si_b", call_level_2_callback_si, arg_cb_b, rvpb);
    m.def("call_level_3_callback_si_b", call_level_3_callback_si, arg_cb_b, rvpb);
    m.def("call_level_4_callback_si_b", call_level_4_callback_si, arg_cb_b, rvpb);

    m.def("call_level_1_callback_is_s", call_level_1_callback_is);
    m.def("call_level_2_callback_is_s", call_level_2_callback_is);
    m.def("call_level_3_callback_is_s", call_level_3_callback_is);
    m.def("call_level_4_callback_is_s", call_level_4_callback_is);

    m.def("call_level_1_callback_is_b", call_level_1_callback_is, arg_cb_b, rvpb);
    m.def("call_level_2_callback_is_b", call_level_2_callback_is, rvpb);
    m.def("call_level_3_callback_is_b", call_level_3_callback_is, rvpb);
    m.def("call_level_4_callback_is_b", call_level_4_callback_is, rvpb);

    py::class_<VirtualBase, VirtualBaseTrampoline>(m, "VirtualBase")
        .def(py::init<>())
        //
        .def("py_pure", &VirtualBase::pure)
        .def("py_pure_s", &VirtualBase::pure_s, py::arg("a0"))
        .def("py_pure_b", &VirtualBase::pure_b, py::arg("a0"))
        .def("py_pure_sb", &VirtualBase::pure_sb, py::arg("a0"), py::arg("a1"))
        .def("py_pure_bs", &VirtualBase::pure_bs, py::arg("a0"), py::arg("a1"))
        //
        .def("py_nonp", &VirtualBase::nonp)
        .def("py_nonp_s", &VirtualBase::nonp_s, py::arg("a0"))
        .def("py_nonp_b", &VirtualBase::nonp_b, py::arg("a0"))
        .def("py_nonp_sb", &VirtualBase::nonp_sb, py::arg("a0"), py::arg("a1"))
        .def("py_nonp_bs", &VirtualBase::nonp_bs, py::arg("a0"), py::arg("a1"));

    m.def("call_virtual_override", call_virtual_override);
}
