TESTS =
CLEANFILES =
check_PROGRAMS =

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

###
###
###

TESTS += test-client
check_PROGRAMS += test-client

test_client_SOURCES = \
  $(top_builddir)/src/sync-service-dbus.c \
  test-client.cpp

test_client_LDADD = \
  $(top_builddir)/src/sync-menu/.libs/libsync-menu.la \
  $(SYNC_LIB_DEPS_LIBS) \
  libgtest.a

test_client_CPPFLAGS = \
  -DINDICATOR_SERVICE="\"$(abs_top_builddir)/src/service/indicator-sync-service\"" \
  -I$(top_srcdir)/src/ \
  -I$(top_builddir)/src/ \
  $(SYNC_LIB_DEPS_CFLAGS) \
  $(AM_CPPFLAGS)
