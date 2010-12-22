#!/usr/bin/env python2

import sys

from distutils.core import setup
from distutils.extension import Extension

try:
    from Cython.Distutils import build_ext
except ImportError:
    use_cython = False
else:
    use_cython = True


if sys.version_info[0] == 2:
    base_dir = 'python2'
elif sys.version_info[0] == 3:
    base_dir = 'python3'

if use_cython:
    ext_modules = [
        Extension("simplerandom.iterators._iterators_cython", [ base_dir + "/cython/_iterators_cython.pyx" ]),
    ]
    cmdclass = {'build_ext': build_ext}
else:
    ext_modules = [
        Extension("simplerandom.iterators._iterators_cython", [ base_dir + "/cython/_iterators_cython.c" ]),
    ]
    cmdclass = { }

setup(
    name='simplerandom',
    version='0.5.0',
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
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Cython',
        'Topic :: Scientific/Engineering :: Mathematics',
    ],
)
