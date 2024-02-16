import copy
import pickle

import pytest

import env
from pybind11_tests import pickling as m


def test_assumptions():
    assert pickle.HIGHEST_PROTOCOL >= 0


@pytest.mark.parametrize("protocol", range(pickle.HIGHEST_PROTOCOL + 1))
def test_pickle_simple_callable(protocol):
    assert m.simple_callable() == 20220426
    serialized = pickle.dumps(m.simple_callable, protocol=protocol)
    assert b"pybind11_tests.pickling" in serialized
    assert b"simple_callable" in serialized
    deserialized = pickle.loads(serialized)
    assert deserialized() == 20220426
    assert deserialized is m.simple_callable
    orig_red = m.simple_callable.__reduce_ex__(protocol)
    desz_red = deserialized.__reduce_ex__(protocol)
    orig_fr = orig_red[1][0]
    desz_fr = desz_red[1][0]
    assert orig_fr is desz_fr
    seri_orig_fr = pickle.dumps(orig_fr)
    desz_orig_fr = pickle.loads(seri_orig_fr)
    print(f"\nLOOOK {dir(m.simple_callable)=}")
    print(f"\nLOOOK {repr(orig_fr)=}")
    print(f"\nLOOOK {repr(m.simple_callable.__self__)=}")
    print(f"\nLOOOK {dir(type(m.simple_callable))=}")
    print(f"\nLOOOK {repr(desz_orig_fr)=}", flush=True)

    """

(<built-in function eval>, ("__import__('importlib').import_module('pybind11_tests.pickling').simple_callable.__self__",))

LOOOK repr(orig_fr)='<pybind11_detail_function_record_v1__gcc_libstdcpp_cxxabi1018_sh_def object at 0x7efd316d9f70>'

LOOOK repr(m.simple_callable.__self__)='<pybind11_detail_function_record_v1__gcc_libstdcpp_cxxabi1018_sh_def object at 0x7efd316d9f70>'

LOOOK dir(type(m.simple_callable))=['__call__', '__class__', '__delattr__', '__dir__', '__doc__', '__eq__', '__format__', '__ge__', '__getattribute__', '__getstate__', '__gt__', '__hash__', '__init__', '__init_subclass__', '__le__', '__lt__', '__module__', '__name__', '__ne__', '__new__', '__qualname__', '__reduce__', '__reduce_ex__', '__repr__', '__self__', '__setattr__', '__sizeof__', '__str__', '__subclasshook__', '__text_signature__']

LOOOK repr(desz_orig_fr)="<module 'pybind11_tests.pickling'>"

    """


@pytest.mark.parametrize("cls_name", ["Pickleable", "PickleableNew"])
def test_roundtrip(cls_name):
    cls = getattr(m, cls_name)
    p = cls("test_value")
    p.setExtra1(15)
    p.setExtra2(48)

    data = pickle.dumps(p, 2)  # Must use pickle protocol >= 2
    p2 = pickle.loads(data)
    assert p2.value() == p.value()
    assert p2.extra1() == p.extra1()
    assert p2.extra2() == p.extra2()


@pytest.mark.xfail("env.PYPY")
@pytest.mark.parametrize("cls_name", ["PickleableWithDict", "PickleableWithDictNew"])
def test_roundtrip_with_dict(cls_name):
    cls = getattr(m, cls_name)
    p = cls("test_value")
    p.extra = 15
    p.dynamic = "Attribute"

    data = pickle.dumps(p, pickle.HIGHEST_PROTOCOL)
    p2 = pickle.loads(data)
    assert p2.value == p.value
    assert p2.extra == p.extra
    assert p2.dynamic == p.dynamic


def test_enum_pickle():
    from pybind11_tests import enums as e

    data = pickle.dumps(e.EOne, 2)
    assert e.EOne == pickle.loads(data)


#
# exercise_trampoline
#
class SimplePyDerived(m.SimpleBase):
    pass


def test_roundtrip_simple_py_derived():
    p = SimplePyDerived()
    p.num = 202
    p.stored_in_dict = 303
    data = pickle.dumps(p, pickle.HIGHEST_PROTOCOL)
    p2 = pickle.loads(data)
    assert isinstance(p2, SimplePyDerived)
    assert p2.num == 202
    assert p2.stored_in_dict == 303


def test_roundtrip_simple_cpp_derived():
    p = m.make_SimpleCppDerivedAsBase()
    assert m.check_dynamic_cast_SimpleCppDerived(p)
    p.num = 404
    if not env.PYPY:
        # To ensure that this unit test is not accidentally invalidated.
        with pytest.raises(AttributeError):
            # Mimics the `setstate` C++ implementation.
            setattr(p, "__dict__", {})  # noqa: B010
    data = pickle.dumps(p, pickle.HIGHEST_PROTOCOL)
    p2 = pickle.loads(data)
    assert isinstance(p2, m.SimpleBase)
    assert p2.num == 404
    # Issue #3062: pickleable base C++ classes can incur object slicing
    #              if derived typeid is not registered with pybind11
    assert not m.check_dynamic_cast_SimpleCppDerived(p2)


#
# exercise_getinitargs_getstate_setstate
#
def test_StoreTwoWithState():
    obj = m.StoreTwoWithState(-38, 27)
    assert obj.__getinitargs__() == (-38, 27)
    assert obj.__getstate__() == "blank"
    obj.__setstate__("blue")
    reduced = obj.__reduce_ex__()
    assert reduced == (m.StoreTwoWithState, (-38, 27), "blue")
    cpy = copy.deepcopy(obj)
    assert cpy.__reduce_ex__() == reduced
    assert pickle.HIGHEST_PROTOCOL > 0  # To be sure the loop below makes sense.
    for protocol in range(-1, pickle.HIGHEST_PROTOCOL + 1):
        serialized = pickle.dumps(obj, protocol)
        restored = pickle.loads(serialized)
        assert restored.__reduce_ex__() == reduced
