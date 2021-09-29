#!/usr/bin/env python3

# Set this to True to enable building extensions using Cython.
# Set it to False to build extensions from the C file (that
# was previously created using Cython).
# Set it to 'auto' to build with Cython if available, otherwise
# from the C file.
USE_CYTHON = False


import sys

from setuptools import setup
from setuptools import Extension

import _version

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
    name='simplerandom',
    version=_version.__version__,
    description='Simple random number generators',
    author='Craig McQueen',
    author_email='python@craig.mcqueen.id.au',
    url='http://github.com/cmcqueen/simplerandom',
    packages=[ 'simplerandom', 'simplerandom.iterators', 'simplerandom.random', 'simplerandom._bitcolumnmatrix', 'simplerandom._version', ],
    package_dir={
        'simplerandom' : 'python3/simplerandom',
        'simplerandom._version' : '_version',
    },
    cmdclass = cmdclass,
    ext_modules=ext_modules,

    long_description=open('README.rst').read(),

    license="MIT",
    classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Cython',
        'Topic :: Scientific/Engineering :: Mathematics',
    ],
    keywords='simple random pseudorandom RNG PRNG',
)

