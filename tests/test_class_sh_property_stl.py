import pytest

from pybind11_tests import class_sh_property_stl as m


def test_vec_fld_hld_ref():
    vfh = m.VectorFieldHolder()
    vfh0 = vfh.vec_fld_hld_ref[0]
    with pytest.raises(RuntimeError) as exc_info:
        vfh0.fld
    assert str(exc_info.value) == "Non-owning holder (loaded_as_shared_ptr)."


def test_vec_fld_hld_cpy():
    vfh = m.VectorFieldHolder()
    vfh0 = vfh.vec_fld_hld_cpy[0]
    assert vfh0.fld.wrapped_int == 300
