from pybind11_tests import type_caster_parameter_pack as m


def test_from_python():
    m.from_python(None)
    assert m.type_sink_void() == 0
    assert m.type_sink_my_type_t_int_int() == 1
    assert m.type_sink_my_type_l_int_2() == 0  # NOT GOOD
