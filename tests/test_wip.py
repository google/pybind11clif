from pybind11_tests import wip as m


def test_vanilla():
    assert m.Vanilla().__class__.__name__ == "Vanilla"
