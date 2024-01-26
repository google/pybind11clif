# Adapted from:
# https://github.com/google/clif/blob/7d388e1de7db5beeb3d7429c18a2776d8188f44f/clif/testing/python/python_multiple_inheritance_test.py

import pytest

from pybind11_tests import python_multiple_inheritance as m


class PC(m.CppBase):
    pass


class PPCC(PC, m.CppDrvd):
    pass


class PCExplicitInitWithSuper(m.CppBase):
    def __init__(self, value):
        super().__init__(value + 1)


class PCExplicitInitMissingSuper(m.CppBase):
    def __init__(self, value):
        del value


class PCExplicitInitMissingSuper2(m.CppBase):
    def __init__(self, value):
        del value


def test_PC():
    d = PC(11)
    assert d.get_base_value() == 11
    d.reset_base_value(13)
    assert d.get_base_value() == 13


def test_PPCC():
    d = PPCC(11)
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


def testPCExplicitInitWithSuper():
    d = PCExplicitInitWithSuper(14)
    assert d.get_base_value() == 15


@pytest.mark.parametrize(
    ("derived_type"), [PCExplicitInitMissingSuper, PCExplicitInitMissingSuper2]
)
def testPCExplicitInitMissingSuper(derived_type):
    derived_type = PCExplicitInitMissingSuper
    with pytest.raises(TypeError) as excinfo:
        derived_type(0)
    assert str(excinfo.value).endswith(
        "python_multiple_inheritance.CppBase.__init__() must be called when overriding __init__"
    )
