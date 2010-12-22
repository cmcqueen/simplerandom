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

The algorithms were obtained from a newsgroup post by George Marsaglia
[#marsaglia]_. The algorithms were specified in C in the newsgroup post. For
the purpose of prototyping software in a high-level language such as Python,
before writing it in faster C, it is useful to have identical algorithms
implemented in both Python and C.

One module provides Python iterators, which generate simple unsigned 32-bit
integers identical to their C counterparts.

Another module provides random classes that are sub-classed from the class
``Random`` in the ``random`` module of the standard Python library.


References
``````````

.. [#marsaglia] | `Random Numbers for C\: End, at last?`__
                | George Marsaglia
                | Newsgroup post, sci.stat.math and others, Thu, 21 Jan 1999

.. __:
.. _Random Numbers for C\: End, at last?:
    http://www.cse.yorku.ca/~oz/marsaglia-rng.html


----------------
Modules Provided
----------------

==========================  ===========================================================================
Module                      Description
==========================  ===========================================================================
``simplerandom.iterators``  Iterator classes, which generate unsigned 32-bit integers.
``simplerandom.random``     Classes that conform to standard Python ``random.Random`` API.
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

Currently this has had basic testing on Ubuntu 10.04 32-bit and Windows XP 32-bit.

In Ubuntu, it has been tested on Python 2.6 and 3.1 and passes.

In Windows, it has been tested on Python 2.6, 2.7 and 3.1 and passes. It has also
been tested on Python 2.5, and fails (due to use of ``next()`` function).

The pure Python code is expected to work on 64-bit platforms.
The Cython version of ``simplerandom.iterators`` should work on 64-bit
platforms, but has not been tested.

The plan is to support Python >= 2.5 and 3.x.


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
[#marsaglia]_. It should print seven lines, all containing 0.

To run it on Python >=2.5::

    python -m simplerandom.iterators.test

Alternatively, in the ``test`` directory run::

    python test_iterators.py


-------------
Documentation
-------------

No documentation is yet written, apart from this README.txt. The plan is to
write documentation using Sphinx, and provide it online at:

    http://packages.python.org/simplerandom/


-------
License
-------

The code is released under the MIT license. See LICENSE.txt for details.

