
AM_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"
AM_LDFLAGS = -version-info @LIB_SO_VERSION@

#######################################
# The list of libraries we are building separated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = lib@PACKAGE_NAME@-@PACKAGE_VERSION@.la

#######################################
# Build information for each library


library_include_simplerandomdir=$(includedir)/@PACKAGE_NAME@-@PACKAGE_VERSION@
library_include_simplerandom_HEADERS = simplerandom.h
lib@PACKAGE_NAME@_@PACKAGE_VERSION@_la_SOURCES = simplerandom.c simplerandom-jumpahead.c bitcolumnmatrix.c bitcolumnmatrix.h pow.c pow.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @PACKAGE_NAME@.pc


#######################################
# Tests

TESTS = test_simple

check_PROGRAMS = test_simple

test_simple_SOURCES = tests/test_simple.c
test_simple_LDADD = lib@PACKAGE_NAME@-@PACKAGE_VERSION@.la

if WITH_CXXTEST

runner.cpp: tests/test_runner.h
	$(CXXTESTGEN) -o $@ --error-printer $^

TESTS += test_runner
check_PROGRAMS += test_runner

test_runner_SOURCES = $(builddir)/runner.cpp
test_runner_LDADD = lib@PACKAGE_NAME@-@PACKAGE_VERSION@.la

endif
