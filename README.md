Simple Random
=============

Contact
:   [http://craig.mcqueen.id.au](http://craig.mcqueen.id.au/)

Copyright
:   2010 Craig McQueen

Simple pseudo-random number generators for C and Python.

Intro
-----

This project provides `simplerandom`, simple pseudo-random number
generators.

Features:

* Main API functions:
    * Seed
    * Generate "next" random value
    * "Discard" also known as "jumpahead" to skip the generator
      ahead by 'n' samples.
    * Mix real random data into the generator state
* Simple algorithms that are easily ported to different languages.
* Currently supported languages are:
    * C
    * Python
* Same numeric output in each supported language. It can be useful
  to be able to implement the identical algorithm on muliple
  platforms and/or languages.
* Simple algorithms and state size appropriate for limited RAM and ROM
  (e.g. in embedded systems).
* Decent cross-platform support.
    * Various OS.
    * Various processors, 8- to 64-bit.
* Implement target language's API idioms and/or existing random number
  generator API.
* Reasonable statistical properties of pseudo-random output (though
  not for all generators provided).

Algorithms
----------

Most algorithms were obtained from two newsgroup posts by George
Marsaglia [^1] [^2]. However, some modifications have been made. From
[^3], it seems that the SHR3 algorithm defined in [^1] is flawed and
should not be used. It doesn't actually have a period of 2**32-1 as
expected, but has 64 different cycles, some with very short periods. The
SHR3 in the 2003 post is very similar, but with two shift values
swapped. It has a period of 2**32-1 as expected.

We still find KISS from [^1] useful mainly because it uses 32-bit
calculations for MWC, which can be more suitable for small embedded
systems. So we define KISS that uses a MWC based on [^1], but the Cong
and SHR3 from [^2].

From Pierre L'Ecuyer [^4] [^5], the Combined LFSR (Tausworthe) LFSR113
algorithm [^6] and LFSR88 (aka Taus88) have been implemented.

### Random Number Generators Provided

The following pseudo-random number generators are provided:

| Generator   | Notes
| ----------- | --------------------------------------------------------------------------------------------------------------
| `MWC1`      | Two 32-bit MWCs combined. From [^1].
| `MWC2`      | Very similar to `MWC1`, but slightly modified to improve its statistical properties.
| `Cong`      | From [^2].
| `SHR3`      | From [^2].
| `MWC64`     | A single 64-bit multiply-with-carry calculation. From [^2].
| `KISS`      | Combination of MWC2, Cong and SHR3. Based on [^1] but using Cong and SHR3 from [^2], and the modified MWC.
| `KISS2`     | Combination of MWC64, Cong and SHR3. From [^2].
| `LFSR113`   | Combined LFSR (Tausworthe) random number generator by L'Ecuyer. From [^4] [^6].
| `LFSR88`    | Combined LFSR (Tausworthe) random number generator by L'Ecuyer. From [^5].

Python
------

The `simplerandom` package is provided, which contains modules
containing classes for various simple pseudo-random number generators.

One module provides Python iterators, which generate simple unsigned
32-bit integers identical to their C counterparts.

Another module provides random classes that are sub-classed from the
class `Random` in the `random` module of the standard Python library.

### Modules Provided

| Module                     | Description
| -------------------------- | --------------------------------------------------------------
| `simplerandom.iterators`   | Iterator classes, which generate unsigned 32-bit integers.
| `simplerandom.random`      | Classes that conform to standard Python `random.Random` API.

In `simplerandom.iterators`, the generators are provided as Python
iterators, of infinite length (they never raise `StopIteration`). They
implement the `next()` method (`__next__()` in Python 3.x) to generate
the next random integer. All the generators output 32-bit unsigned
values, and take one or more 32-bit seed values during initialisation/
seeding.

In `simplerandom.random`, pseudo-random number generators are provided
which have the same names as those in `simplerandom.iterators`, but
these generators implement the standard Python `random.Random` API.
However the `jumpahead()` function (Python 2.x) is not implemented in
all cases. Each generator uses the iterator of the same name in
`simplerandom.iterators` to generate the random bits used to produce the
random floats.

### Iterators Usage

    >>> import simplerandom.iterators as sri
    >>> rng = sri.KISS(123958, 34987243, 3495825239, 2398172431)
    >>> next(rng)
    702862187L
    >>> next(rng)
    13888114L
    >>> next(rng)
    699722976L

### Random class API Usage

    >>> import simplerandom.random as srr
    >>> rng = srr.KISS(258725234)
    >>> rng.random()
    0.0925917826051541
    >>> rng.random()
    0.02901686453730415
    >>> rng.random()
    0.9024972981686489

### Supported Python Versions

Currently this has had basic testing on Ubuntu 10.04 32-bit and Windows
XP 32-bit. It passes the basic `simplerandom.iterators.test` unit tests,
as well as basic manual testing of `simplerandom.random`. A more
thorough unit test suite is needed.

In Ubuntu, it has been tested on Python 2.6 and 3.1 and passes.

In Windows, it has been tested on Python 2.4, 2.5, 2.6, 2.7, 3.1 and
3.2. It passes under these versions.

The pure Python code is expected to work on 64-bit platforms, but has
not been tested. The Cython version of `simplerandom.iterators` should
work on 64-bit platforms, but has not been tested.

### Use of Cython

[Cython](http://cython.org/) is used to make a fast implementation of
`simplerandom.iterators`. Cython creates a `.c` file that can be
compiled into a Python binary extension module.

The `simplerandom` source distribution package includes a `.c` file that
was created with Cython, so it is not necessary to have Cython installed
to install `simplerandom`.

The Cython `.pyx` file is also included, if you want to modify the
Cython source code, in which case you do need to have Cython installed.
But by default, `setup.py` builds the extension from the `.c` file (to
ensure that the build doesn't fail due to particular Cython version
issues). If you wish to build using Cython from the included `.pyx`
file, you must set `USE_CYTHON=True` in `setup.py`.

### Installation

The simplerandom package is installed using `distutils`. If you have the
tools installed to build a Python extension module, run the following
command:

    python setup.py install

If you cannot build the C extension, you may install just the pure
Python implementation, using the following command:

    python setup.py build_py install --skip-build

### Unit Testing

Basic unit testing of the iterators is in `simplerandom.iterators.test`.
It duplicates the tests of the C algorithms given in the original
newsgroup post [^1], as well as other unit tests.

To run it on Python \>=2.5:

    python -m simplerandom.iterators.test

Alternatively, in the `test` directory run:

    python test_iterators.py

A more thorough unit test suite is needed.

License
-------

The code is released under the MIT license. See LICENSE.txt for details.

References
----------

[^1]: [Random Numbers for C: End, at last?](http://www.cse.yorku.ca/~oz/marsaglia-rng.html)\
    George Marsaglia\
    Newsgroup post, sci.stat.math and others, Thu, 21 Jan 1999

[^2]: [RNGs](http://groups.google.com/group/sci.math/msg/9959175f66dd138f)\
    George Marsaglia\
    Newsgroup post, sci.math, 26 Feb 2003

[^3]: [KISS: A Bit Too Simple](http://eprint.iacr.org/2011/007.pdf)\
    Greg Rose\
    Qualcomm Inc.

[^4]: [Tables of Maximally-Equidistributed Combined LFSR Generators](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.43.3639)\
    Pierre L'Ecuyer\
    Mathematics of Computation, 68, 225 (1999), 261–269.

[^5]: [Maximally Equidistributed Combined Tausworthe Generators](http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.43.4155)\
    P. L'Ecuyer\
    Mathematics of Computation, 65, 213 (1996), 203–213.

[^6]: [LFSR113 C double implementation](http://www.iro.umontreal.ca/~simardr/rng/lfsr113.c)\
    Pierre L'Ecuyer

