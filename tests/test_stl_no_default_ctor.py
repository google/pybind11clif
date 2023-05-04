from pybind11_tests import stl_no_default_ctor as m


def test_readwrite_arr():
    obj = m.NodeArray(3)
    assert [e.val for e in obj.arr] == [13, 23]
    obj.arr = m.NodeArray(4).arr
    assert [e.val for e in obj.arr] == [14, 24]
