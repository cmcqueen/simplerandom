
AM_CPPFLAGS = -DSYSCONFDIR=\"$(sysconfdir)\"
AM_LDFLAGS = -version-info @LIB_SO_VERSION@

#######################################
# The list of libraries we are building seperated by spaces.
# The 'lib_' indicates that these build products will be installed
# in the $(libdir) directory. For example /usr/lib
lib_LTLIBRARIES = lib@PACKAGE_NAME@-@PACKAGE_VERSION@.la

#######################################
# Build information for each library


# files for at-parse
library_include_at_parsedir=$(includedir)/@PACKAGE_NAME@-@PACKAGE_VERSION@
library_include_at_parse_HEADERS = simplerandom.h
lib@PACKAGE_NAME@_@PACKAGE_VERSION@_la_SOURCES = simplerandom.c

# Linker options libTestProgram
#libsocket_tools_@SOCKET_TOOLS_API_VERSION@_la_LDFLAGS = 

# Compiler options. Here we are adding the include directory
# to be searched for headers included in the source code.
#libsocket_tools_@SOCKET_TOOLS_API_VERSION@_la_CPPFLAGS = -I$(top_srcdir)/include

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = @PACKAGE_NAME@.pc


# Tests
TESTS = test_simple

check_PROGRAMS = test_simple

test_simple_SOURCES = tests/test_simple.c
test_simple_LDADD = lib@PACKAGE_NAME@-@PACKAGE_VERSION@.la
