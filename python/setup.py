#!/usr/bin/env python

import sys

from distutils.core import setup
from distutils.extension import Extension

try:
    from Cython.Distutils import build_ext
except ImportError:
    use_cython = False
else:
    use_cython = True

cmdclass = { }
ext_modules = [ ]

if sys.version_info[0] == 2:
    base_dir = 'python2'
elif sys.version_info[0] == 3:
    # Still build from python2 code, but use build_py_2to3 to translate.
    base_dir = 'python2'
    from distutils.command.build_py import build_py_2to3 as build_py
    cmdclass.update({ 'build_py': build_py })

if use_cython:
    ext_modules += [
        Extension("simplerandom.iterators._iterators_cython", [ "cython/_iterators_cython.pyx" ]),
    ]
    cmdclass.update({ 'build_ext': build_ext })
else:
    ext_modules += [
        Extension("simplerandom.iterators._iterators_cython", [ "cython/_iterators_cython.c" ]),
    ]

setup(
    name='simplerandom',
    version='0.8.1',
    description='Simple random number generators',
    author='Craig McQueen',
    author_email='python@craig.mcqueen.id.au',
    url='http://bitbucket.org/cmcqueen1975/simplerandom/',
    packages=[ 'simplerandom', 'simplerandom.iterators', 'simplerandom.random', ],
    package_dir={
        'simplerandom' : base_dir + '/simplerandom',
    },
    cmdclass = cmdclass,
    ext_modules=ext_modules,

    long_description=open('README.txt').read(),

    license="MIT",
    classifiers=[
        'Development Status :: 3 - Alpha',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.4',
        'Programming Language :: Python :: 2.5',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.1',
        'Programming Language :: Cython',
        'Topic :: Scientific/Engineering :: Mathematics',
    ],
    keywords='simple random pseudorandom RNG PRNG',
)
