# 8086tiny: a tiny, highly functional, highly portable PC emulator/VM
# Copyright 2013-14, Adrian Cable (adrian.cable@gmail.com) - http://www.megalith.co.uk/8086tiny
#
# This work is licensed under the MIT License. See included LICENSE.TXT.

# 8086tiny builds with graphics and sound support
# 8086tiny_slowcpu improves graphics performance on slow platforms (e.g. Raspberry Pi)
# no_graphics compiles without SDL graphics/sound

OPTS_ALL=-O3 -fsigned-char -std=c99
OPTS_SDL=`sdl-config --cflags --libs`
OPTS_NOGFX=-DNO_GRAPHICS
OPTS_SLOWCPU=-DGRAPHICS_UPDATE_DELAY=25000
OPTS_TMT=-DUSE_TMT -DTMT_HAS_WCWIDTH
OPTS_DEBUG=-g

SRC_TMT=tmt.c


all: 
	${MAKE} 8086tiny
	${MAKE} 8086tiny_emsc
	${MAKE} 8086tiny_node

8086tiny: 8086tiny.c
	${CC} 8086tiny.c ${SRC_TMT} ${OPTS_TMT} ${OPTS_SDL} ${OPTS_ALL} -o ./build/8086tiny
	strip ./build/8086tiny

8086tiny_emsc: 8086tiny.c
	emcc 8086tiny.c ${SRC_TMT} ${OPTS_TMT} -DVTERM_SMALL_CONSOLE -o build/8086tiny.html -fsigned-char -O1 --preload-file bios --preload-file fd.img -s USE_SDL_TTF=1

8086tiny_node: 8086tiny.c
	emcc 8086tiny.c ${SRC_TMT} ${OPTS_TMT} -DVTERM_USE_CR -DNO_GRAPHICS -o build/8086tiny_node.js -fsigned-char -O1 --embed-file bios --embed-file fd.img

8086tiny_no_tmt: 8086tiny.c
	${CC} 8086tiny.c ${OPTS_SDL} ${OPTS_ALL} -o ./build/8086tiny
	strip ./build/8086tiny

8086tiny_slowcpu: 8086tiny.c
	${CC} 8086tiny.c ${OPTS_SDL} ${OPTS_ALL} ${OPTS_SLOWCPU} -o ./build/8086tiny
	strip ./build/8086tiny

no_graphics: 8086tiny.c
	${CC} 8086tiny.c ${OPTS_NOGFX} ${OPTS_ALL} -o ./build/8086tiny
	strip ./build/8086tiny

clean:
	rm 8086tiny
