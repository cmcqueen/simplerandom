=============
Simple Random
=============

:Author: Craig McQueen
:Contact: http://craig.mcqueen.id.au/
:Copyright: 2010 Craig McQueen


Simple pseudo-random number generators.

-----
Intro
-----

The ``simplerandom`` package is provided, which contains modules containing
classes for various simple pseudo-random number generators.

One module provides Python iterators, which generate simple unsigned 32-bit
integers identical to their C counterparts.

Another module provides random classes that are sub-classed from the class
``Random`` in the ``random`` module of the standard Python library.

Why use this package? These random number generators are very simple, which
has two main advantages:

* It is easy to port them to a different platform and/or language. It can be
  useful to be able to implement the identical algorithm on multiple
  platforms and/or languages.
* Small and simple generators can be more appropriate for small embedded
  systems, with limited RAM and ROM.

An equivalent C implementation (of the Python ``simplerandom.iterators``
module) has been created. See:

    http://github.com/cmcqueen/simplerandom

Algorithms
``````````

Most algorithms were obtained from two newsgroup posts by George Marsaglia
[#marsaglia1999]_ [#marsaglia2003]_. However, some modifications have been
made. From [#rose]_, it seems that the SHR3 algorithm defined in
[#marsaglia1999]_ is flawed and should not be used. It doesn't actually have a
period of 2**32-1 as expected, but has 64 different cycles, some with very
short periods. The SHR3 in the 2003 post is very similar, but with two shift
values swapped. It has a period of 2**32-1 as expected.

We still find KISS from [#marsaglia1999]_ useful mainly because it uses 32-bit
calculations for MWC, which can be more suitable for small embedded systems.
So we define KISS that uses a MWC based on [#marsaglia1999]_, but the Cong and
SHR3 from [#marsaglia2003]_.

From Pierre L'Ecuyer [#lecuyer1999]_ [#lecuyer1996]_, the Combined LFSR
(Tausworthe) LFSR113 algorithm [#lfsr113]_ and LFSR88 (aka Taus88) have been
implemented.


References
``````````

.. [#marsaglia1999] | `Random Numbers for C\: End, at last?`__
                    | George Marsaglia
                    | Newsgroup post, sci.stat.math and others, Thu, 21 Jan 1999

.. __:
.. _Random Numbers for C\: End, at last?:
    http://www.cse.yorku.ca/~oz/marsaglia-rng.html

.. [#marsaglia2003] | `RNGs`__
                    | George Marsaglia
                    | Newsgroup post, sci.math, 26 Feb 2003

.. __:
.. _RNGs:
    http://groups.google.com/group/sci.math/msg/9959175f66dd138f

.. [#rose]          | `KISS: A Bit Too Simple`__
                    | Greg Rose
                    | Qualcomm Inc.

.. __:
.. _KISS\: A Bit Too Simple:
    http://eprint.iacr.org/2011/007.pdf

.. [#lecuyer1999]   | `Tables of Maximally-Equidistributed Combined LFSR Generators`__
                    | Pierre L'Ecuyer
                    | Mathematics of Computation, 68, 225 (1999), 261–269.

.. __:
.. _Tables of Maximally-Equidistributed Combined LFSR Generators:
    http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.43.3639

.. [#lfsr113]       | `LFSR113 C double implementation`__
                    | Pierre L'Ecuyer

.. __:
.. _LFSR113 C double implementation:
    http://www.iro.umontreal.ca/~simardr/rng/lfsr113.c

.. [#lecuyer1996]   | `Maximally Equidistributed Combined Tausworthe Generators`__
                    | P. L'Ecuyer
                    | Mathematics of Computation, 65, 213 (1996), 203–213.

.. __:
.. _Maximally Equidistributed Combined Tausworthe Generators:
    http://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.43.4155


----------------
Modules Provided
----------------

==========================  ===========================================================================
Module                      Description
==========================  ===========================================================================
``simplerandom.iterators``  Iterator classes, which generate unsigned 32-bit integers.
``simplerandom.random``     Classes that conform to standard Python ``random.Random`` API.
==========================  ===========================================================================


Random Number Generators Provided
`````````````````````````````````

In ``simplerandom.iterators``, the following pseudo-random number generators are provided:

==========================  ===========================================================================
Generator                   Notes
==========================  ===========================================================================
``MWC1``                    Two 32-bit MWCs combined. From [#marsaglia1999]_.
``MWC2``                    Very similar to ``MWC1``, but slightly modified to improve its statistical properties.
``Cong``                    From [#marsaglia2003]_.
``SHR3``                    From [#marsaglia2003]_.
``MWC64``                   A single 64-bit multiply-with-carry calculation. From [#marsaglia2003]_.
``KISS``                    Combination of MWC2, Cong and SHR3. Based on [#marsaglia1999]_ but using Cong and SHR3 from [#marsaglia2003]_, and the modified MWC.
``KISS2``                   Combination of MWC64, Cong and SHR3. From [#marsaglia2003]_.
``LFSR113``                 Combined LFSR (Tausworthe) random number generator by L'Ecuyer. From [#lecuyer1999]_ [#lfsr113]_.
``LFSR88``                  Combined LFSR (Tausworthe) random number generator by L'Ecuyer. From [#lecuyer1996]_.
==========================  ===========================================================================

These generators are Python iterators, of infinite length (they never raise
``StopIteration``). They implement the ``__next__()`` function to generate the
next random integer. All the generators output 32-bit unsigned values, and
take one or more 32-bit seed values during initialisation/seeding.


In ``simplerandom.random``, pseudo-random number generators are provided which
have the same names as those in ``simplerandom.iterators``, but these
generators implement the standard Python ``random.Random`` API. Each generator
uses the iterator of the same name in ``simplerandom.iterators`` to generate
the random bits used to produce the random floats. The ``jumpahead()`` function
(in the style of the Python 2.x API) is implemented in all cases, even though
``jumpahead()`` has officially been removed from the Python 3.x ``random`` API.


-----
Usage
-----

Iterators
`````````

    >>> import simplerandom.iterators as sri
    >>> rng = sri.KISS(123958, 34987243, 3495825239, 2398172431)
    >>> next(rng)
    702862187
    >>> next(rng)
    13888114
    >>> next(rng)
    699722976

Random class API
````````````````

    >>> import simplerandom.random as srr
    >>> rng = srr.KISS(258725234)
    >>> rng.random()
    0.0925917826051541
    >>> rng.random()
    0.02901686453730415
    >>> rng.random()
    0.9024972981686489


-------------------------
Supported Python Versions
-------------------------

Python >= 3.10 are supported.

Python versions < 3.10 might work, but have not been tested.


-------------
Use of Cython
-------------

`Cython`_ is used to make a fast implementation of ``simplerandom.iterators``.
Cython creates a ``.c`` file that can be compiled into a Python binary
extension module.

The ``simplerandom`` source distribution package includes a ``.c`` file that
was created with Cython, so it is not necessary to have Cython installed to
install ``simplerandom``.

The Cython ``.pyx`` file is also included, if you want to modify the Cython
source code, in which case you do need to have Cython installed. But by
default, ``setup.py`` builds the extension from the ``.c`` file (to ensure
that the build doesn't fail due to particular Cython version issues). If you
wish to build using Cython from the included ``.pyx`` file, you must set
``USE_CYTHON=True`` in ``setup.py``.

.. _Cython:
    http://cython.org/


------------
Installation
------------

The simplerandom package is installed using ``distutils``.  If you have the tools
installed to build a Python extension module, run the following command::

    python setup.py install

If you cannot build the C extension, you may install just the pure Python
implementation, using the following command::

    python setup.py build_py install --skip-build


------------
Unit Testing
------------

Unit testing of the iterators is in ``simplerandom.iterators.test``. It
duplicates the tests of the C algorithms given in the original newsgroup post
[#marsaglia1999]_, as well as other unit tests.

To run unit tests::

    python -m simplerandom.iterators.test

A more thorough unit test suite is needed. A unit test suite for
``simplerandom.random`` is needed.


-------
License
-------

The code is released under the MIT license. See LICENSE.txt for details.

