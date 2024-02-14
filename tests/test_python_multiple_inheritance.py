# Adapted from:
# https://github.com/google/clif/blob/7d388e1de7db5beeb3d7429c18a2776d8188f44f/clif/testing/python/python_multiple_inheritance_test.py

import pytest

from pybind11_tests import python_multiple_inheritance as m

#
# Using default py::metaclass() (used with py::class_<> for CppBase0, CppDrvd0):
#


class PC0(m.CppBase0):
    pass


class PPCC0(PC0, m.CppDrvd0):
    pass


class PCExplicitInitWithSuper0(m.CppBase0):
    def __init__(self, value):
        super().__init__(value + 1)


class PCExplicitInitMissingSuper0(m.CppBase0):
    def __init__(self, value):
        del value


class PCExplicitInitMissingSuperB0(m.CppBase0):
    def __init__(self, value):
        del value


#
# Using py::metaclass((PyObject *) &PyType_Type) (used with py::class_<> for CppBase1, CppDrvd1):
# COPY-PASTE block from above, replace 0 with 1:
#


class PC1(m.CppBase1):
    pass


class PPCC1(PC1, m.CppDrvd1):
    pass


class PCExplicitInitWithSuper1(m.CppBase1):
    def __init__(self, value):
        super().__init__(value + 1)


class PCExplicitInitMissingSuper1(m.CppBase1):
    def __init__(self, value):
        del value


class PCExplicitInitMissingSuperB1(m.CppBase1):
    def __init__(self, value):
        del value


@pytest.mark.parametrize(("pc_type"), [PC0, PC1])
def test_PC(pc_type):
    d = pc_type(11)
    assert d.get_base_value() == 11
    d.reset_base_value(13)
    assert d.get_base_value() == 13


@pytest.mark.parametrize(("ppcc_type"), [PPCC0, PPCC1])
def test_PPCC(ppcc_type):
    d = ppcc_type(11)
    assert d.get_drvd_value() == 33
    d.reset_drvd_value(55)
    assert d.get_drvd_value() == 55

    assert d.get_base_value() == 11
    assert d.get_base_value_from_drvd() == 11
    d.reset_base_value(20)
    assert d.get_base_value() == 20
    assert d.get_base_value_from_drvd() == 20
    d.reset_base_value_from_drvd(30)
    assert d.get_base_value() == 30
    assert d.get_base_value_from_drvd() == 30


@pytest.mark.parametrize(
    ("pc_type"), [PCExplicitInitWithSuper0, PCExplicitInitWithSuper1]
)
def testPCExplicitInitWithSuper(pc_type):
    d = pc_type(14)
    assert d.get_base_value() == 15


@pytest.mark.parametrize(
    ("derived_type"),
    [
        PCExplicitInitMissingSuper0,
        PCExplicitInitMissingSuperB0,
        PCExplicitInitMissingSuper1,
        PCExplicitInitMissingSuperB1,
    ],
)
def testPCExplicitInitMissingSuper(derived_type):
    if (
        m.if_defined_PYBIND11_INIT_SAFETY_CHECKS_VIA_DEFAULT_PYBIND11_METACLASS
        and derived_type
        in (
            PCExplicitInitMissingSuper1,
            PCExplicitInitMissingSuperB1,
        )
    ):
        pytest.skip(
            "PYBIND11_INIT_SAFETY_CHECKS_VIA_DEFAULT_PYBIND11_METACLASS is defined"
        )
    with pytest.raises(TypeError) as excinfo:
        derived_type(0)
    assert str(excinfo.value).endswith(
        ".__init__() must be called when overriding __init__"
    )


def test_derived_tp_init_registry_weakref_based_cleanup():
    def nested_function(i):
        class NestedClass(m.CppBase0):
            def __init__(self, value):
                super().__init__(value + 3)

        d1 = NestedClass(i + 7)
        d2 = NestedClass(i + 8)
        return (d1.get_base_value(), d2.get_base_value())

    for _ in range(100):
        assert nested_function(0) == (10, 11)
        assert nested_function(3) == (13, 14)
