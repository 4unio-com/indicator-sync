SUBDIRS = icons

upstart_jobsdir = $(datadir)/upstart/sessions
upstart_jobs_DATA = indicator-sync.conf

%.conf: %.conf.in
	sed -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@

xdg_autostartdir = /etc/xdg/autostart
xdg_autostart_DATA = indicator-sync.conf

%.desktop: %.desktop.in
	sed -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@

EXTRA_DIST = \
	indicator-sync.desktop.in \
	indicator-sync.conf.in

CLEANFILES = \
	indicator-sync.desktop \
	indicator-sync.conf
