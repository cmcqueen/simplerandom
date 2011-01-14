=============
Simple Random
=============

:Author: Craig McQueen
:Contact: http://craig.mcqueen.id.au/
:Copyright: 2010 Craig McQueen


Simple pseudo-random number generators, from George Marsaglia.

-----
Intro
-----

The ``simplerandom`` package is provided, which contains modules containing
classes for various simple pseudo-random number generators.

The algorithms were obtained from two newsgroup posts by George Marsaglia
[#marsaglia1999]_ [#marsaglia2003]_. The algorithms were specified in C in the
newsgroup posts. For the purpose of prototyping software in a high-level
language such as Python, before writing it in faster C, it is useful to have
identical algorithms implemented in both Python and C.

One module provides Python iterators, which generate simple unsigned 32-bit
integers identical to their C counterparts.

Another module provides random classes that are sub-classed from the class
``Random`` in the ``random`` module of the standard Python library.


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

.. [#cook]      | `Simple Random Number Generation`__
                | John D. Cook
                | The Code Project, 27 Jul 2010

.. __:
.. _Simple Random Number Generation:
    http://www.codeproject.com/KB/recipes/SimpleRNG.aspx


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
``RandomCongIterator``      This and below from From [#marsaglia1999]_.
``RandomSHR3Iterator``      
``RandomMWCIterator``       Slightly different algorithm from that in [#cook]_.
``RandomKISSIterator``      Combination of MWC, Cong and SHR3
``RandomLFIB4Iterator``
``RandomSWBIterator``
``_RandomFibIterator``      Not useful on its own, but can be used in a combination with other generators.
``RandomCong2Iterator``     This and below from From [#marsaglia2003]_. Very similar to Cong, but different added constant and default seed.
``RandomSHR3_2Iterator``    Similar to SHR3, but different shift values and default seed.
``RandomMWC64Iterator``     A single 64-bit multiply-with-carry calculation.
``RandomKISS2Iterator``     Combination of MWC64, Cong2 and SHR3_2
==========================  ===========================================================================

In ``simplerandom.random``, the following pseudo-random number generators are provided:

==========================  ===========================================================================
Generator                   Notes
==========================  ===========================================================================
``RandomCong``              This and below from From [#marsaglia1999]_.
``RandomSHR3``      
``RandomMWC``               Slightly different algorithm from that in [#cook]_.
``RandomKISS``              Combination of MWC, Cong and SHR3
``RandomLFIB4``
``RandomSWB``
==========================  ===========================================================================


-----
Usage
-----

Iterators
`````````

    >>> import simplerandom.iterators as sri
    >>> rng = sri.RandomKISSIterator(123958, 34987243, 3495825239, 2398172431)
    >>> next(rng)
    21111917L
    >>> next(rng)
    1327965872L
    >>> next(rng)
    2128842716L

Random class API
````````````````

    >>> import simplerandom.random as srr
    >>> rng = srr.RandomKISS(258725234)
    >>> rng.random()
    0.30631872435766117
    >>> rng.random()
    0.43903576112750442
    >>> rng.random()
    0.69756297733927486


-------------------------
Supported Python Versions
-------------------------

Currently this has had basic testing on Ubuntu 10.04 32-bit and
Windows XP 32-bit. It passes the basic ``simplerandom.iterators.test`` unit
tests, as well as basic manual testing of ``simplerandom.random``. A more
thorough unit test suite is needed.

In Ubuntu, it has been tested on Python 2.6 and 3.1 and passes.

In Windows, it has been tested on Python 2.4, 2.5, 2.6, 2.7, 3.1 and 3.2.
It passes under these versions.

The pure Python code is expected to work on 64-bit platforms, but has not been
tested. The Cython version of ``simplerandom.iterators`` should work on 64-bit
platforms, but has not been tested.


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

Basic unit testing of the iterators is in ``simplerandom.iterators.test``. It
duplicates the tests of the C algorithms given in the original newsgroup post
[#marsaglia1999]_, as well as other unit tests.

To run it on Python >=2.5::

    python -m simplerandom.iterators.test

Alternatively, in the ``test`` directory run::

    python test_iterators.py

A more thorough unit test suite is needed.


-------
License
-------

The code is released under the MIT license. See LICENSE.txt for details.

