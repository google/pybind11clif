pybind11 ``type_caster`` design & `clangd Include Cleaner`_
===========================================================

This document is purely to explain — in a nutshell — why pybind11 include
files with ``type_caster`` specializations require this:

.. code-block:: cpp

  // IWYU pragma: always_keep

For general technical information about the pybind11 ``type_caster`` mechanism
see the `Custom type casters`_ section.

The problem
-----------

The `clangd Include Cleaner`_ cannot possibly have the ability to detect which
``type_caster`` specialization is the correct one to use in a given C++
translation unit. Without IWYU pragmas, Include Cleaner is likely to suggest
removing ``type_caster`` include files.

The consequences
----------------

1. Incorrect runtime behavior.
2. `ODR violations`_.

Example for 1. Incorrect runtime behavior
-----------------------------------------

.. code-block:: cpp

  #include <pybind11/pybind11.h>
  #include <pybind11/stl.h>

If the stl.h include is removed (as suggested by Include Cleaner), conversions
between e.g. ``std::vector`` and Python ``list`` will no longer work. In most
cases this will be very obvious at runtime, but there are less obvious
situations, most notably for ``type_caster`` specializations that inherit from
``type_caster_base`` (e.g. `pybind11_abseil/status_caster.h`_).
Some conversions may still work correctly, while others will have unexpected
behavior.

Explanation for 2. ODR violations
---------------------------------

This problem only arises if two or more C++ translation units are statically
linked (e.g. one ``.so`` Python extension built from multiple .cpp files), or
all visibility-restricting features (e.g. ``-fvisibility=hidden``,
``namespace pybind11`` ``__attribute__((visibility("hidden")))``,
``RTLD_LOCAL``) are disabled.

Consider the same simple code example as above: if the stl.h include is missing
(as suggested by Include Cleaner) in one translation unit, but not in another
(maybe because Include Cleaner was never applied), the resulting Python
extension will have link incompatibilities, which is often referred to as
ODR violations. The behavior is entirely undefined. For better or worse, the
extension may perform as desired for years, until one day it suddenly does
not, because of some unrelated change in the environment (e.g. new compiler
version, a system library update), resulting in seemingly inexplicable failures

See also: `pybind/pybind11#4022`_ (pybind11 smart_holder branch)

.. _`clangd Include Cleaner`: https://clangd.llvm.org/design/include-cleaner
.. _`Custom type casters`: https://pybind11.readthedocs.io/en/stable/advanced/cast/custom.html
.. _`ODR violations`: https://en.cppreference.com/w/cpp/language/definition
.. _`pybind11_abseil/status_caster.h`: https://github.com/pybind/pybind11_abseil/blob/4b883e48ae749ff984c220484d54fdeb0cb4302c/pybind11_abseil/status_caster.h#L52-L53).
.. _`pybind/pybind11#4022`: https://github.com/pybind/pybind11/pull/4022
