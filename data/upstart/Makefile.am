
xdg_autostartdir = $(datadir)/upstart/xdg/autostart
xdg_autostart_DATA = indicator-sync.desktop

%.desktop: %.desktop.in
	sed -e "s|\@pkglibexecdir\@|$(pkglibexecdir)|" $< > $@

EXTRA_DIST = \
	indicator-sync.desktop.in

CLEANFILES = \
	indicator-sync.desktop
