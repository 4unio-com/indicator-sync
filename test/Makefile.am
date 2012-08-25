TESTS =
CLEANFILES =

###
###
###

# stock UMB tests on user-visible strings
include $(srcdir)/Makefile.am.strings

# build libgtest
check_LIBRARIES = libgtest.a
nodist_libgtest_a_SOURCES = \
  $(GTEST_SOURCE)/src/gtest-all.cc \
  $(GTEST_SOURCE)/src/gtest_main.cc

AM_CPPFLAGS = $(GTEST_CPPFLAGS) $(SHARED_CPPFLAGS)
AM_CXXFLAGS = $(GTEST_CXXFLAGS)
