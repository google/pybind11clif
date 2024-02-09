import copy
import pickle

import pytest

import env
from pybind11_tests import pickling as m

_orig_simple_callable = m.simple_callable


def _wrapped_simple_callable(*args, **kwargs):
    return _orig_simple_callable(*args, **kwargs)


m.simple_callable = _wrapped_simple_callable


def test_pickle_simple_callable():
    assert m.simple_callable() == 20220426
    serialized = pickle.dumps(m.simple_callable)
    deserialized = pickle.loads(serialized)
    assert deserialized() == 20220426


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
