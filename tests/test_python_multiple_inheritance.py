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


class PPPCCC0(PPCC0, m.CppDrvd20):
    pass


class PC10(m.CppDrvd0):
    pass


class PC20(m.CppDrvd20):
    pass


class PCD0(PC10, PC20):
    pass


class PCDI0(PC10, PC20):
    def __init__(self):
        PC10.__init__(self, 11)
        PC20.__init__(self, 12)


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


class PPPCCC1(PPCC1, m.CppDrvd21):
    pass


class PC11(m.CppDrvd1):
    pass


class PC21(m.CppDrvd21):
    pass


class PCD1(PC11, PC21):
    pass


class PCDI1(PC11, PC21):
    def __init__(self):
        PC11.__init__(self, 11)
        PC21.__init__(self, 12)


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


def NOtest_PPPCCC0():
    # terminate called after throwing an instance of 'pybind11::error_already_set'
    # what():  TypeError: bases include diverging derived types:
    #     base=pybind11_tests.python_multiple_inheritance.CppBase0,
    #     derived1=pybind11_tests.python_multiple_inheritance.CppDrvd0,
    #     derived2=pybind11_tests.python_multiple_inheritance.CppDrvd20
    PPPCCC0(11)


def NOtest_PPPCCC1():
    # terminate called after throwing an instance of 'pybind11::error_already_set'
    # what():  TypeError: bases include diverging derived types:
    #     base=pybind11_tests.python_multiple_inheritance.CppBase1,
    #     derived1=pybind11_tests.python_multiple_inheritance.CppDrvd1,
    #     derived2=pybind11_tests.python_multiple_inheritance.CppDrvd21
    PPPCCC1(11)


@pytest.mark.parametrize(
    ("pcd_type", "cppdrvd2"), [(PCD0, "CppDrvd20"), (PCD1, "CppDrvd21")]
)
def test_PCD(pcd_type, cppdrvd2):
    # This escapes all_type_info_check_for_divergence() because CppBase does not appear in bases.
    with pytest.raises(
        TypeError,
        match=cppdrvd2 + r"\.__init__\(\) must be called when overriding __init__$",
    ):
        pcd_type(11)


def test_PCD1():
    # This escapes all_type_info_check_for_divergence() because CppBase0 does not appear in bases.
    with pytest.raises(
        TypeError,
        match=r"CppDrvd21\.__init__\(\) must be called when overriding __init__$",
    ):
        PCD1(11)


@pytest.mark.parametrize(
    ("pcdi_type", "pass_fn"), [(PCDI0, m.pass_CppBase0), (PCDI1, m.pass_CppBase1)]
)
def test_PCDI(pcdi_type, pass_fn):
    obj = pcdi_type()
    with pytest.raises(TypeError, match="^bases include diverging derived types: "):
        pass_fn(obj)
