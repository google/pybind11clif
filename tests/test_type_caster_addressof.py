from pybind11_tests import type_caster_addressof as m


def test_merely_that_it_actually_built():
    assert m.CallCastConstRef0().__class__.__name__ == "UnusualOpRef0"
    assert m.CallCastConstRef1().__class__.__name__ == "UnusualOpRef1"
    assert m.CallCastMovable0().__class__.__name__ == "UnusualOpRef0"
    assert m.CallCastMovable1().__class__.__name__ == "UnusualOpRef1"
