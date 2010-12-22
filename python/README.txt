=============
Simple Random
=============

:Author: Craig McQueen
:Contact: http://craig.mcqueen.id.au/
:Copyright: 2010 Craig McQueen


Python functions for simple random number generators.

-----
Intro
-----

The ``simplerandom`` package is provided, which contains modules containing
classes for various simple random number generators.


References
``````````

.. [#marsaglia] | `Random Numbers for C: End, at last?`__
                | George Marsaglia
                | Newsgroup post, sci.stat.math and others, Thu, 21 Jan 1999

.. __:
.. _Random Numbers for C: End, at last?:
    http://www.cse.yorku.ca/~oz/marsaglia-rng.html


----------------
Modules Provided
----------------

==========================  ===========================================================================
Module                      Description
==========================  ===========================================================================
``simplerandom.iterators``  Simple RNG in the form of iterators, which generate 32-bit random numbers.
``simplerandom.random``     Simple RNG in the form of classes that conform to Python ``random`` API.
==========================  ===========================================================================


-----
Usage
-----



-------------------------
Supported Python Versions
-------------------------

Python >= 2.4 and 3.x are supported, and have both a C extension and a pure
Python implementation.

Python versions < 2.4 might work, but have not been tested. Python 3.0 has
also not been tested.


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

Basic unit testing is in the ``test`` sub-module, e.g. ``simplerandom.iterators.test``. To run it on Python >=2.5::

    python -m simplerandom.iterators.test

Alternatively, in the ``test`` directory run::

    python test_iterators.py


-------------
Documentation
-------------

Documentation is written with Sphinx. Source files are provided in the ``doc``
directory. It can be built using Sphinx 0.6.5. It uses the ``pngmath`` Sphinx
extension, which requires Latex and ``dvipng`` to be installed.

The documentation is available online at: http://packages.python.org/cobs/


-------
License
-------

The code is released under the MIT license. See LICENSE.txt for details.

