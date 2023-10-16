#!/usr/bin/env python3

# Set this to True to enable building extensions using Cython.
# Set it to False to build extensions from the C file (that
# was previously created using Cython).
# Set it to 'auto' to build with Cython if available, otherwise
# from the C file.
USE_CYTHON = True


import sys

from setuptools import setup
from setuptools import Extension

if USE_CYTHON:
    try:
        from Cython.Distutils import build_ext
    except ImportError:
        if USE_CYTHON=='auto':
            USE_CYTHON=False
        else:
            raise

cmdclass = { }
ext_modules = [ ]

if sys.version_info[0] == 2:
    raise Exception('Python 2.x is no longer supported')

if USE_CYTHON:
    ext_modules += [
        Extension("simplerandom.iterators._iterators_cython", [ "cython/_iterators_cython.pyx" ]),
        Extension("simplerandom._bitcolumnmatrix._bitcolumnmatrix_cython", [ "cython/_bitcolumnmatrix_cython.pyx" ]),
    ]
    cmdclass.update({ 'build_ext': build_ext })
else:
    ext_modules += [
        Extension("simplerandom.iterators._iterators_cython", [ "cython/_iterators_cython.c" ]),
        Extension("simplerandom._bitcolumnmatrix._bitcolumnmatrix_cython", [ "cython/_bitcolumnmatrix_cython.c" ]),
    ]

setup(
    packages=[ 'simplerandom', 'simplerandom.iterators', 'simplerandom.random', 'simplerandom._bitcolumnmatrix', 'simplerandom._version', ],
    package_dir={
        'simplerandom' : 'src/simplerandom',
    },
    cmdclass = cmdclass,
    ext_modules=ext_modules,
)

