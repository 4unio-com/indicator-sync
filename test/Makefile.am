TESTS =

check_LIBRARIES = libgtest.a

AM_CPPFLAGS = $(GTEST_CPPFLAGS) -I${top_srcdir}/src

nodist_libgtest_a_SOURCES = $(GTEST_SOURCE)/src/gtest-all.cc \
                            $(GTEST_SOURCE)/src/gtest_main.cc
libgtest_a_CXXFLAGS = $(AM_CXXFLAGS)
libgtest_a_CPPFLAGS = $(AM_CPPFLAGS) $(GTEST_CPPFLAGS) -w

