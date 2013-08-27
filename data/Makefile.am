SUBDIRS = icons

upstart_jobsdir = $(datadir)/upstart/sessions
upstart_jobs_DATA = indicator-sync.conf

%.conf: %.conf.in
	sed -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@

EXTRA_DIST = indicator-sync.conf.in

CLEANFILES = indicator-sync.conf
