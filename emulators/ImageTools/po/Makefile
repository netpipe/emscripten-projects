# LOCALEDIR, RM, INSTALL defined in parent makefile

MOFILES = ar.mo be@latin.mo bg.mo ca.mo cs.mo da.mo de.mo el.mo es.mo et.mo fi.mo fo.mo fr.mo he.mo hr.mo hu.mo id.mo it.mo ja.mo lv.mo nb.mo nl.mo nn.mo pl.mo pt.mo pt_BR.mo ro.mo ru.mo sk.mo sq.mo sr.mo sl.mo sr@latin.mo sr@Latn.mo sv.mo tr.mo uk.mo ur_PK.mo zh_CN.mo zh_TW.mo

all: $(MOFILES)
	
# static pattern rule
$(MOFILES): %.mo: %.po Makefile
	@echo 'Compiling translation' $<
	@msgfmt -c -v $< -o $@

clean:
	$(RM) *.mo *.percent

# for info about DESTDIR see http://www.gnu.org/prep/standards/html_node/DESTDIR.html

install: all
	for NAME in $(MOFILES); do \
	  $(INSTALL) -v -d $(DESTDIR)$(LOCALEDIR)/`basename $$NAME .mo`/LC_MESSAGES; \
	  $(INSTALL) -v -m 644 $$NAME $(DESTDIR)$(LOCALEDIR)/`basename $$NAME .mo`/LC_MESSAGES/isomaster.mo; \
	done

uninstall:
	for NAME in $(MOFILES); do \
	  $(RM) -f $(DESTDIR)$(LOCALEDIR)/`basename $$NAME .mo`/LC_MESSAGES/isomaster.mo; \
	done
