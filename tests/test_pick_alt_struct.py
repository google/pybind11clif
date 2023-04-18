import pytest

from pybind11_tests import pick_alt_struct as m


@pytest.mark.parametrize(
    ("func", "expected"),
    [
        (m.is_primary_int, True),
        (m.is_primary_dbl, False),
        (m.pos_ft_int_show, "primary+"),
        (m.pos_ft_dbl_show, "secondary+"),
        (m.pos_tf_int_show, "+primary"),
        (m.pos_tf_dbl_show, "+secondary"),
        (m.neg_ft_int_show, "primary-"),
        (m.neg_ft_dbl_show, "secondary-"),
        (m.neg_tf_int_show, "-primary"),
        (m.neg_tf_dbl_show, "-secondary"),
    ],
)
def test_show(func, expected):
    assert func() == expected
