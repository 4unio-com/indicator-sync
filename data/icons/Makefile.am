SUBDIRS = 22x22 24x24

icondir = $(datadir)/icons/hicolor
gtk_update_icon_cache = gtk-update-icon-cache -f -t

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating Gtk icon cache."; \
		mkdir -p $(icondir); \
		$(gtk_update_icon_cache) $(icondir); \
	else \
		echo "*** Icon cache not updated.  After (un)install, run this:"; \
		echo "***   $(gtk_update_icon_cache)"; \
	fi

