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

} // namespace

TEST_SUBMODULE(return_value_policy_pack, m) {
    auto rvpc = py::return_value_policy::_clif_automatic;
    auto rvpb = py::return_value_policy::_return_as_bytes;

    m.def("return_tuple_str_str", []() { return return_pair_string(); });
    m.def(
        "return_tuple_bytes_bytes", []() { return return_pair_string(); }, rvpb);
    m.def(
        "return_tuple_str_bytes",
        []() { return return_pair_string(); },
        py::return_value_policy_pack({rvpc, rvpb}));
    m.def(
        "return_tuple_bytes_str",
        []() { return return_pair_string(); },
        py::return_value_policy_pack({rvpb, rvpc}));

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
        []() { return return_map_string(); },
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
        []() { return return_set_pair_string(); },
        py::return_value_policy_pack({rvpb, rvpc}));

    m.def(
        "return_vector_sb",
        []() { return return_vector_pair_string(); },
        py::return_value_policy_pack({rvpc, rvpb}));
    m.def(
        "return_vector_bs",
        []() { return return_vector_pair_string(); },
        py::return_value_policy_pack({rvpb, rvpc}));

    m.def(
        "return_array_sb",
        []() { return return_array_pair_string(); },
        py::return_value_policy_pack({rvpc, rvpb}));
    m.def(
        "return_array_bs",
        []() { return return_array_pair_string(); },
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
        []() { return return_optional_pair_string(); },
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
        []() { return return_variant_pair_string(); },
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
}
