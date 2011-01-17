from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

ext_modules = [Extension("find_cycles", ["find_cycles.pyx"])]

setup(
  name = 'Find cycles app',
  cmdclass = {'build_ext': build_ext},
  ext_modules = ext_modules
)

