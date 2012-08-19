defsdir = $(datadir)/pygtk/2.0/defs
defs_DATA = syncindicator.defs

#CFLAGS = -Wall -Werror
INCLUDES = \
	-I$(top_srcdir) \
        -DG_LOG_DOMAIN=\"syncindicator-python\" \
        -DDATADIR=\"$(datadir)\" \
        -DLIBDIR=\"$(libdir)\" \
        $(SYNC_PYTHON_CFLAGS) \
        $(PYTHON_INCLUDES) \
	$(SYNC_CLIENT_CFLAGS)

pkgpythondir = $(pyexecdir)
pkgpyexecdir = $(pyexecdir)

pkgindicatedir = $(pkgpythondir)/indicate
pkgindicate_PYTHON = __init__.py

indicatedir = $(pkgpyexecdir)/indicate
indicate_LTLIBRARIES = _syncindicator.la

_syncindicator_la_LDFLAGS = \
	-module \
	-avoid-version \
	-export-symbols-regex \
	init_syncindicator

_syncindicator_la_LIBADD = \
	$(SYNC_PYTHON_LIBS) \
	-L$(top_builddir)/src/.libs/ -lindicator-sync-source \
	$(SYNC_CLIENT_LIBS)

_syncindicator_la_SOURCES = \
	syncindicatormodule.c

nodist__syncindicator_la_SOURCES = \
	syncindicator.c

CLEANFILES = sync.c
EXTRA_DIST = syncindicator.override sync-arg-types.py $(defs_DATA)
syncindicator.c: $(defs_DATA) syncindicator.override

%.c: %.defs
	(cd $(srcdir) \
	 && $(PYGTK_CODEGEN) \
		--register $(PYGTK_DEFSDIR)/gtk-types.defs \
		--register $(PYGTK_DEFSDIR)/gdk-types.defs \
		--load-types $*-arg-types.py \
	    --override $*.override \
	    --prefix py$* $*.defs) > gen-$*.c \
	 && cp gen-$*.c $*.c \
	 && rm -f gen-$*.c
