===========================================================================
pybind11k — A fork of pybind11 set up for sustained innovation & continuity
===========================================================================

Warning — New features are still in flux
========================================

pybind11k is used Google-internally to build thousands of extensions that are deployed to production and is also regularly tested with all CLANG sanitizers. However, feature stability guarantees are currently limited in this way:

0. pybind11k is meant to maximize backward-compatiblity with `pybind11 (master) <https://github.com/pybind/pybind11/tree/master>`_.

1. The main driving force for adding new features is the PyCLIF-pybind11 integration work (for Googlers: `go/pyclif_pybind11_fusion <http://go/pyclif_pybind11_fusion>`_). Until this work is completed, it is impractical for us to take external use cases into account when evolving new features. This is because we can globally test changes fairly easily internally, but not externally.

2. After the PyCLIF-pybind11 work is completed we will commit to greater feature stability and update the documentation.

Outstanding new features: ``py::native_enum`` (google/pybind11k#30005), ``py::return_value_policy_pack`` (google/pybind11k#30011), enhanced pybind11/functional.h API (google/pybind11k#30022)

Background / Overview
=====================

This pybind11k fork originated from the `pybind11 smart_holder <https://github.com/pybind/pybind11/tree/smart_holder>`_ branch. It was created with two important goals in mind:

1. Sustained innovation and proactive bug fixes.
2. Sustained continuity.

With original pybind11, these two goals are in a conflict. In the early days of pybind11 this was not so much of a problem, but this has been changing with growing adoption.

Regarding goal 1: pybind11 has two serious long-standing bugs (pybind/pybind11#1138, pybind/pybind11#1333) that have never been fixed on the master branch, but were fixed on the smart_holder branch in early-mid 2021.

Regarding goal 2: The original and still current ``pybind11::smart_holder`` implementation is a compromise solution that avoids an `"ABI break" <https://github.com/pybind/pybind11/blob/09db6445d8da6e918c2d2be3aa4e7b0ddd8077c7/include/pybind11/detail/internals.h#L25>`_, concretely, changes to the ``pybind11::details::internals`` ``struct``. Each time the ``internals`` are changed, the ``PYBIND11_INTERNALS_VERSION`` needs to be incremented, cutting off interoperability with existing PyPI wheels based on pybind11, without giving any hint about this problem at runtime. In the meantime, two other PRs were merged on the pybind11 master branch that require changes to the ``internals``, PRs pybind/pybind11#3275 and pybind/pybind11#4254. To avoid ABI breaks, these PRs were effectively held back behind ``#ifdef`` s. This problem came to a breaking point with PR pybind/pybind11#4329, for which hiding the new feature behind ``#ifdef`` s is not a practical option.

Obviously, neither repeatedly breaking interoperability with existing PyPI wheels, nor holding back bug fixes and new features, is desirable. Therefore PR pybind/pybind11#4329 was extended to include a generalization of the ``internals`` approach, under the name ``cross_extension_shared_state``. The fundamental difference is that established shared state is left untouched, and new shared states are added as needed, largely resolving the conflict between innovation & continuity. The price to pay is added complexity managing the evolving shared states, but that is assumed to be a relatively small extra effort for a few developers, resulting in a big usability gain for a much larger number of users. Ultimately, this is just the familiar innovate-deprecate-cleanup life cycle typical for many (all?) long-term successful major projects (e.g. C++, Python). Even pybind11k developers are likely to see this as a win worth paying a price for, because they are more free to innovate.

A direct consequence of goal 2. is that the C++ pybind11 namespace cannot abruptly be changed, because renaming would break both API and ABI compatibility. The intent is to change the API gradually, driven primarily by code health and innovation-related refactoring needs.

Notes regarding the choice of name for this fork
================================================

The main motivations for this choice of name are:

* We want it to be immediately obvious that pybind11k is rooted in pybind11. Users should not be surprised to find that the main include is still ``pybind11/pybind11.h``, the C++ namespace name is still ``pybind11``, the C++ macros are still prefixed with ``PYBIND11_``, and that ``pip install pybind11k`` (not yet available) produces an installation similar to that of ``pip install pybind11``.

* ``11k`` is short for ``11000``, not to be taken too seriously, but it is meant to signal that we will inevitably need to move beyond the C++11 standard, and that any software needs to evolve to survive long-term.

* And yes, the ``k`` is also playing on `"Py3k" <https://www.python.org/download/releases/3.0/>`_.

ORIGINAL pybind11 README below (to be updated)
==============================================

**pybind11** is a lightweight header-only library that exposes C++ types
in Python and vice versa, mainly to create Python bindings of existing
C++ code. Its goals and syntax are similar to the excellent
`Boost.Python <http://www.boost.org/doc/libs/1_58_0/libs/python/doc/>`_
library by David Abrahams: to minimize boilerplate code in traditional
extension modules by inferring type information using compile-time
introspection.

The main issue with Boost.Python—and the reason for creating such a
similar project—is Boost. Boost is an enormously large and complex suite
of utility libraries that works with almost every C++ compiler in
existence. This compatibility has its cost: arcane template tricks and
workarounds are necessary to support the oldest and buggiest of compiler
specimens. Now that C++11-compatible compilers are widely available,
this heavy machinery has become an excessively large and unnecessary
dependency.

Think of this library as a tiny self-contained version of Boost.Python
with everything stripped away that isn't relevant for binding
generation. Without comments, the core header files only require ~4K
lines of code and depend on Python (3.6+, or PyPy) and the C++
standard library. This compact implementation was possible thanks to
some C++11 language features (specifically: tuples, lambda functions and
variadic templates). Since its creation, this library has grown beyond
Boost.Python in many ways, leading to dramatically simpler binding code in many
common situations.

Tutorial and reference documentation is provided at
`pybind11.readthedocs.io <https://pybind11.readthedocs.io/en/latest>`_.
A PDF version of the manual is available
`here <https://pybind11.readthedocs.io/_/downloads/en/latest/pdf/>`_.
And the source code is always available at
`github.com/pybind/pybind11 <https://github.com/pybind/pybind11>`_.


Core features
-------------


pybind11 can map the following core C++ features to Python:

- Functions accepting and returning custom data structures per value,
  reference, or pointer
- Instance methods and static methods
- Overloaded functions
- Instance attributes and static attributes
- Arbitrary exception types
- Enumerations
- Callbacks
- Iterators and ranges
- Custom operators
- Single and multiple inheritance
- STL data structures
- Smart pointers with reference counting like ``std::shared_ptr``
- Internal references with correct reference counting
- C++ classes with virtual (and pure virtual) methods can be extended
  in Python
- Integrated NumPy support (NumPy 2 requires pybind11 2.12+)

Goodies
-------

In addition to the core functionality, pybind11 provides some extra
goodies:

- Python 3.6+, and PyPy3 7.3 are supported with an implementation-agnostic
  interface (pybind11 2.9 was the last version to support Python 2 and 3.5).

- It is possible to bind C++11 lambda functions with captured
  variables. The lambda capture data is stored inside the resulting
  Python function object.

- pybind11 uses C++11 move constructors and move assignment operators
  whenever possible to efficiently transfer custom data types.

- It's easy to expose the internal storage of custom data types through
  Pythons' buffer protocols. This is handy e.g. for fast conversion
  between C++ matrix classes like Eigen and NumPy without expensive
  copy operations.

- pybind11 can automatically vectorize functions so that they are
  transparently applied to all entries of one or more NumPy array
  arguments.

- Python's slice-based access and assignment operations can be
  supported with just a few lines of code.

- Everything is contained in just a few header files; there is no need
  to link against any additional libraries.

- Binaries are generally smaller by a factor of at least 2 compared to
  equivalent bindings generated by Boost.Python. A recent pybind11
  conversion of PyRosetta, an enormous Boost.Python binding project,
  `reported <https://graylab.jhu.edu/Sergey/2016.RosettaCon/PyRosetta-4.pdf>`_
  a binary size reduction of **5.4x** and compile time reduction by
  **5.8x**.

- Function signatures are precomputed at compile time (using
  ``constexpr``), leading to smaller binaries.

- With little extra effort, C++ types can be pickled and unpickled
  similar to regular Python objects.

Supported compilers
-------------------

1. Clang/LLVM 3.3 or newer (for Apple Xcode's clang, this is 5.0.0 or
   newer)
2. GCC 4.8 or newer
3. Microsoft Visual Studio 2017 or newer
4. Intel classic C++ compiler 18 or newer (ICC 20.2 tested in CI)
5. Cygwin/GCC (previously tested on 2.5.1)
6. NVCC (CUDA 11.0 tested in CI)
7. NVIDIA PGI (20.9 tested in CI)

About
-----

This project was created by `Wenzel
Jakob <http://rgl.epfl.ch/people/wjakob>`_. Significant features and/or
improvements to the code were contributed by Jonas Adler, Lori A. Burns,
Sylvain Corlay, Eric Cousineau, Aaron Gokaslan, Ralf Grosse-Kunstleve, Trent Houliston, Axel
Huebl, @hulucc, Yannick Jadoul, Sergey Lyskov, Johan Mabille, Tomasz Miąsko,
Dean Moldovan, Ben Pritchard, Jason Rhinelander, Boris Schäling, Pim
Schellart, Henry Schreiner, Ivan Smirnov, Boris Staletic, and Patrick Stewart.

We thank Google for a generous financial contribution to the continuous
integration infrastructure used by this project.


Contributing
~~~~~~~~~~~~

See the `contributing
guide <https://github.com/pybind/pybind11/blob/master/.github/CONTRIBUTING.md>`_
for information on building and contributing to pybind11.

License
~~~~~~~

pybind11 is provided under a BSD-style license that can be found in the
`LICENSE <https://github.com/pybind/pybind11/blob/master/LICENSE>`_
file. By using, distributing, or contributing to this project, you agree
to the terms and conditions of this license.
