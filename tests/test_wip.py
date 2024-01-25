import abc

from pybind11_tests import wip as m


class VanillaABC(m.Vanilla, metaclass=abc.ABCMeta):
    pass


def test_vanilla():
    assert m.Vanilla().__class__.__name__ == "Vanilla"
    assert type(m.Vanilla).__name__ == "type"
