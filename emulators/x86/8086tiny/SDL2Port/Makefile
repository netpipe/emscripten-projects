# 8086tiny: a tiny, highly functional, highly portable PC emulator/VM
# Copyright 2013-14, Adrian Cable (adrian.cable@gmail.com) - http://www.megalith.co.uk/8086tiny
#
# This work is licensed under the MIT License. See included LICENSE.TXT.

# 8086tiny builds with graphics and sound support
# 8086tiny_slowcpu improves graphics performance on slow platforms (e.g. Raspberry Pi)
# no_graphics compiles without SDL graphics/sound

OPTS_ALL=-O3 -fsigned-char -std=c99
OPTS_SDL=`sdl2-config --cflags --libs` -lSDL2_ttf -lstdc++
OPTS_NOGFX=-DNO_GRAPHICS
OPTS_SLOWCPU=-DGRAPHICS_UPDATE_DELAY=25000
OPTS_TMT=-DUSE_TMT -DTMT_HAS_WCWIDTH
OPTS_DEBUG=-g



all: 
	${MAKE} 8086tiny

8086tiny: 8086tiny.cpp
	${CC} 8086tiny.cpp consolemgr.cpp tmt.c ${OPTS_SDL} ${OPTS_ALL} -o ./8086tiny
	strip ./8086tiny



clean:
	rm 8086tiny
