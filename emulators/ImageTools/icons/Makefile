# ICONPATH, RM, INSTALL defined in parent makefile

ICONS = isomaster.png go-back-kearone.png folder-new-kearone.png add2-kearone.png extract2-kearone.png delete-kearone.png

install: 
	$(INSTALL) -d $(DESTDIR)$(ICONPATH)
	for NAME in $(ICONS); do \
	  $(INSTALL) -v -m 644 $$NAME $(DESTDIR)$(ICONPATH); \
	done

uninstall:
	for NAME in $(ICONS); do \
	  $(RM) -f $(DESTDIR)$(ICONPATH)/$$NAME; \
	done
