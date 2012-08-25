SUBDIRS = src data examples po

if BUILD_TESTS
  SUBDIRS += test
  # build src first
  test: src
endif

DISTCHECK_CONFIGURE_FLAGS = \
  --enable-localinstall \
  --enable-deprecations \
  --enable-introspection \
  --enable-tests=yes

include $(top_srcdir)/Makefile.am.authors
include $(top_srcdir)/Makefile.am.coverage
