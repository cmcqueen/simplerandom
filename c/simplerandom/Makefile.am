
#######################################
# The list of libraries we are building separated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = lib@PACKAGE_NAME@-@PACKAGE_VERSION@.la

#######################################
# Build information for each library


library_include_simplerandomdir=$(includedir)/@PACKAGE_NAME@-@PACKAGE_VERSION@
library_include_simplerandom_HEADERS = simplerandom.h simplerandom-c.h simplerandom-cpp.h
lib@PACKAGE_NAME@_@PACKAGE_VERSION@_la_SOURCES = simplerandom.c simplerandom-discard.c bitcolumnmatrix.c bitcolumnmatrix.h maths.c maths.h
lib@PACKAGE_NAME@_@PACKAGE_VERSION@_la_LDFLAGS = -version-info @LIB_SO_VERSION@

library_include_simplerandom_simplerandomdir=$(includedir)/@PACKAGE_NAME@-@PACKAGE_VERSION@/simplerandom
library_include_simplerandom_simplerandom_HEADERS = simplerandom/bitcolumnmatrix.h simplerandom/maths.h simplerandom/tools.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @PACKAGE_NAME@.pc


#######################################
# Tests

TESTS = test_simple test_cpp_simple

check_PROGRAMS = test_simple test_cpp_simple

test_simple_SOURCES = tests/test_simple.c
test_simple_LDADD = lib@PACKAGE_NAME@-@PACKAGE_VERSION@.la

test_cpp_simple_SOURCES = tests/test_cpp_simple.cpp
test_cpp_simple_LDADD = lib@PACKAGE_NAME@-@PACKAGE_VERSION@.la

if WITH_CXXTEST

runner.cpp: tests/test_runner.h
	$(CXXTESTGEN) -o $@ --error-printer $^

TESTS += test_runner
check_PROGRAMS += test_runner

dist_noinst_DATA = tests/test_runner.h
nodist_test_runner_SOURCES = @builddir@/runner.cpp
test_runner_LDADD = lib@PACKAGE_NAME@-@PACKAGE_VERSION@.la

endif
