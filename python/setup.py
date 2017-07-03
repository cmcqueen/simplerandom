#!/usr/bin/env python

# Set this to True to enable building extensions using Cython.
# Set it to False to build extensions from the C file (that
# was previously created using Cython).
# Set it to 'auto' to build with Cython if available, otherwise
# from the C file.
USE_CYTHON = False


import sys

from distutils.core import setup
from distutils.extension import Extension

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
    base_dir = 'python2'
elif sys.version_info[0] == 3:
    # Still build from python2 code, but use build_py_2to3 to translate.
    base_dir = 'python2'
    from distutils.command.build_py import build_py_2to3 as build_py
    cmdclass.update({ 'build_py': build_py })

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
        'simplerandom' : base_dir + '/simplerandom',
        'simplerandom._version' : '_version',
    },
    cmdclass = cmdclass,
    ext_modules=ext_modules,

    long_description=open('README.txt').read(),

    license="MIT",
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.1',
        'Programming Language :: Python :: 3.2',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Cython',
        'Topic :: Scientific/Engineering :: Mathematics',
    ],
    keywords='simple random pseudorandom RNG PRNG',
)

