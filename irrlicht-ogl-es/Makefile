ROOT := $(shell pwd)
IRRLICHT_DIR = $(ROOT)/irrlicht/
IRRLICHT_LIB = $(IRRLICHT_DIR)lib/Linux/libIrrlicht.a
IRRLICHT_TIMESTAMP = $(IRRLICHT_DIR)timestamp
IRRLICHT_TIMESTAMP_INT = $(ROOT)irrlicht_timestamp
IRRLICHT_URL_SVN = http://svn.code.sf.net/p/irrlicht/code/branches/ogl-es/

CXX=emcc -s
EMMAKE=emmake
CPPFLAGS="-D_IRR_COMPILE_WITH_EMSCRIPTEN -m32 -W -Wall -Os -pipe -fno-exceptions -fno-rtti -fstrict-aliasing"
CXXFLAGS += "-D_IRR_COMPILE_WITH_EMSCRIPTEN -m32 -W -Wall -Os -pipe -fno-exceptions -fno-rtti -fstrict-aliasing"

.PHONY : irrlicht_download $(IRRLICHT_TIMESTAMP) clean clean_irrlicht \
	 irrlicht


$(IRRLICHT_TIMESTAMP) : irrlicht_download
	@LAST_MODIF=$$(find ${IRRLICHT_DIR} -type f -printf '%T@ %p\n' | sort -n | tail -1 | cut -f2- -d" "); \
	if [ $$(basename $$LAST_MODIF) != "timestamp" ] ; then                     \
		touch ${IRRLICHT_TIMESTAMP};                                           \
	fi

irrlicht_download :
	@if [ ! -d "irrlicht" ] ; then                                        \
		echo "irrlicht sources missing, downloading...";                       \
		svn co ${IRRLICHT_URL_SVN} irrlicht || exit 1;                         \
		cd irrlicht;                                                           \
	fi

irrlicht : $(IRRLICHT_LIB)

$(IRRLICHT_LIB): $(IRRLICHT_TIMESTAMP)
	@REFRESH=0;                                                                \
	if [ ! -e ${IRRLICHT_TIMESTAMP_INT} ] ; then                               \
		REFRESH=1;                                                             \
	fi;                                                                        \
	if [ ! -e ${IRRLICHT_LIB} ] ; then                                         \
		REFRESH=1;                                                             \
	fi;                                                                        \
	if [ ${IRRLICHT_TIMESTAMP} -nt ${IRRLICHT_TIMESTAMP_INT} ] ; then          \
		REFRESH=1;                                                             \
	fi;                                                                        \
	if [ $$REFRESH -ne 0 ] ; then                                              \
	mkdir -p ${IRRLICHT_DIR};                                                  \
	echo "changed timestamp for irrlicht detected building...";                \
	cd irrlicht/source/Irrlicht; 				                               \
	CPPFLAGS=$(CPPFLAGS) $(EMMAKE) $(MAKE) || exit 1;     \
	touch ${IRRLICHT_TIMESTAMP};                                               \
	touch ${IRRLICHT_TIMESTAMP_INT};                                           \
	else                                                                       \
		echo "nothing to be done for irrlicht";                                \
	fi

realclean :
	$(RM) -rf irrlicht

clean :
	$(MAKE) -C irrlicht/source/Irrlicht clean
	rm -f $(IRRLICHT_LIB)
