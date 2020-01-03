# Other paths are relative to this.
PREFIX ?= /usr/local

# Where to install the executable.
BINPATH ?= $(PREFIX)/bin

# Where to install the icons.
# Used by icons/Makefile
export ICONPATH ?= $(PREFIX)/share/isomaster/icons

# Used by gettext and po/Makefile (translations)
# To disable installation of some .mo files edit or delete the MOFILES 
# variable in po/Makefile. This is safe to do.
export LOCALEDIR ?= $(PREFIX)/share/locale

# Where to install the man page.
MYMANPATH ?= $(PREFIX)/share/man/man1

# Where to install the bkisofs docs
MYDOCPATH ?= $(PREFIX)/share/doc/bkisofs

# Where to install the .desktop file
DESKTOPPATH ?= $(PREFIX)/share/applications

# The default editor for files from the image. Users can change this. I
# recommend you set it to a graphical text editor that is likely to be 
# installed by default on your distribution.
DEFAULT_EDITOR ?= mousepad

# The default viewer for files from the image. I recommend you make it 
# a web browser because it can display the widest range of files.
DEFAULT_VIEWER ?= firefox

# To disable i18n completely, uncomment the following line 
# or define WITHOUT_NLS somewhere else.
# This option is desired in the FreeBSD ports guidelines.
#WITHOUT_NLS = 1

# This enables overwriting the original iso,
# don't uncomment it unless you are willing to risk losing data.
#CPPFLAGS += -DENABLE_SAVE_OVERWRITE

# programs used in the Makefiles:
export CC      ?= gcc
export AR      = ar
export RM      = rm -f
export INSTALL = install
export CP      = cp
export ECHO    = echo

VERSION = 1.3.8

# -DDEBUG and -g only used during development
CFLAGS += -Wall -pedantic -std=gnu99 -Wundef -Wcast-align -W -Wpointer-arith -Wwrite-strings -Wno-unused-parameter `pkg-config --cflags gtk+-2.0`
ifndef WITHOUT_NLS
	CFLAGS += -DENABLE_NLS
endif

CPPFLAGS +=  -DICONPATH=\"$(ICONPATH)\" -DLOCALEDIR=\"$(LOCALEDIR)\" -DDEFAULT_EDITOR=\"$(DEFAULT_EDITOR)\" -DDEFAULT_VIEWER=\"$(DEFAULT_VIEWER)\" -DVERSION=\"$(VERSION)\"

# the _FILE_OFFSET_BITS=64 is to enable stat() for large files on linuxish systems
CPPFLAGS += -D_FILE_OFFSET_BITS=64

OBJECTS = isomaster.o window.o browser.o fsbrowser.o isobrowser.o error.o about.o settings.o boot.o editfile.o

all: translations isomaster.desktop isomaster

isomaster: $(OBJECTS) lib iniparser
	@echo 'Linking isomaster'
	@$(CC) $(OBJECTS) bk/bk.a iniparser-2.17/libiniparser.a $(LDFLAGS) $(CFLAGS) $(CPPFLAGS) `pkg-config --libs gtk+-2.0` -o isomaster

# static pattern rule
$(OBJECTS): %.o: %.c %.h bk/bk.h Makefile
	@echo 'Compiling' $<
	@$(CC) $< $(CFLAGS) $(CPPFLAGS) -c -o $@

lib:
	cd bk && $(MAKE)

iniparser:
	cd iniparser-2.17 && $(MAKE)

translations:
ifndef WITHOUT_NLS
	cd po && $(MAKE)
endif

isomaster.desktop: isomaster.desktop.src
	$(CP) isomaster.desktop.src isomaster.desktop
	$(ECHO) Icon=$(ICONPATH)/isomaster.png >> isomaster.desktop

bk-doc:
	cd bkisofs-manual && $(MAKE)

clean: 
	cd bk && $(MAKE) clean
	cd iniparser-2.17 && $(MAKE) clean
ifndef WITHOUT_NLS
	cd po && $(MAKE) clean
endif
	$(RM) *.o isomaster isomaster.desktop

# for info about DESTDIR see http://www.gnu.org/prep/standards/html_node/DESTDIR.html

install: all
	$(INSTALL) -d $(DESTDIR)$(BINPATH)
	$(INSTALL) isomaster $(DESTDIR)$(BINPATH)
	cd icons && $(MAKE) install
ifndef WITHOUT_NLS
	cd po && $(MAKE) install
endif
	$(INSTALL) -d $(DESTDIR)$(MYMANPATH)
	$(INSTALL) -m 644 isomaster.1 $(DESTDIR)$(MYMANPATH)
	$(INSTALL) -d $(DESTDIR)$(DESKTOPPATH)
	$(INSTALL) -m 644 isomaster.desktop $(DESTDIR)$(DESKTOPPATH)
	$(INSTALL) -d $(DESTDIR)$(MYDOCPATH)
	for FILE in bkisofs-manual/*html; do \
            $(INSTALL) -m 644 $$FILE $(DESTDIR)$(MYDOCPATH); \
        done;

uninstall: 
	$(RM) -f $(DESTDIR)$(BINPATH)/isomaster
	cd icons && $(MAKE) uninstall
ifndef WITHOUT_NLS
	cd po && $(MAKE) uninstall
endif
	$(RM) -f $(DESTDIR)$(MYMANPATH)/isomaster.1
	$(RM) -f $(DESTDIR)$(DESKTOPPATH)/isomaster.desktop
	$(RM) -rf $(DESTDIR)$(MYDOCPATH)
