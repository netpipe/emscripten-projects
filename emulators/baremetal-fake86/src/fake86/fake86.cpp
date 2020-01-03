/*
  Fake86: A portable, open-source 8086 PC emulator.
  Copyright (C)2010-2012 Mike Chambers

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/* main.c: functions to initialize the different components of Fake86,
   load ROM binaries, and kickstart the CPU emulator. */

#include "config.h"
#include "types.h"
#include "log.h"
#include "mem.h"
#include "disk.h"
#include "ports.h"
#include "render.h"
#include "video.h"
#include "cpu.h"

#include "../../data/dosboot.h"
#include "../../data/pcxtbios.h"
#include "../../data/videorom.h"

const char *build = "Fake86 v0.12.9.19";

extern uint8_t running, renderbenchmark;

extern void reset86();
extern void exec86 (uint32_t execloops);
extern uint8_t initscreen (uint8_t *ver);
extern void VideoThread();
extern void doscrmodechange();
extern void handleinput();
void draw();

extern uint8_t scrmodechange, doaudio;
extern uint64_t totalexec, totalframes;
uint64_t starttick, endtick;
extern uint64_t lasttick;

extern uint8_t verbose;
uint8_t cgaonly = 0, useconsole = 0;
uint32_t speed = 0;

#ifdef TODO
uint32_t loadbinary (uint32_t addr32, char *filename, uint8_t roflag) {
	FILE *binfile = nullptr;
	uint32_t readsize;

	binfile = fopen (filename, "rb");
	if (binfile == nullptr) {
			return (0);
		}

	fseek (binfile, 0, SEEK_END);
	readsize = ftell (binfile);
	fseek (binfile, 0, SEEK_SET);
	fread ( (void *) &RAM[addr32], 1, readsize, binfile);
	fclose (binfile);

	memset ( (void *) &readonly[addr32], roflag, readsize);
	return (readsize);
}

uint32_t loadrom (uint32_t addr32, char *filename, uint8_t failure_fatal) {
	uint32_t readsize;
	readsize = loadbinary (addr32, filename, 1);
	if (!readsize) {
			if (failure_fatal) log("FATAL: ");
			else log("WARNING: ");
			log ("Unable to load %s\n", filename);
			return (0);
		}
	else {
			log ("Loaded %s at 0x%05X (%lu KB)\n", filename, addr32, readsize >> 10);
			return (readsize);
		}
}
#endif

uint32_t loadembeddedrom(uint32_t addr32, const uint8_t* rom, uint32_t size) {
	copymem(RAM + addr32, rom, size);
	setmem((void *)&readonlyflag[addr32], 1, size);
	return size;
}

extern uint8_t insertdisk (uint8_t drivenum, char *filename);
extern void ejectdisk (uint8_t drivenum);
extern uint8_t bootdrive, ethif, net_enabled;
extern void doirq (uint8_t irqnum);
//extern void isa_ne2000_init(int baseport, uint8_t irq);
void inittiming();
void initaudio();
void init8253();
void init8259();
extern void init8237();
extern void initVideoPorts();
extern void killaudio();
extern void initsermouse (uint16_t baseport, uint8_t irq);
extern void initadlib (uint16_t baseport);
extern void initsoundsource();
extern void isa_ne2000_init (uint16_t baseport, uint8_t irq);
extern void initBlaster (uint16_t baseport, uint8_t irq);

void printbinary (uint8_t value) {
	int8_t curbit;

	for (curbit=7; curbit>=0; curbit--) {
			if ( (value >> curbit) & 1) log ("1");
			else log ("0");
		}
}

uint8_t usessource = 0;
void inithardware() {
	log ("Initializing emulated hardware:\n");
	log ("  - Intel 8253 timer: ");
	init8253();
	log ("OK\n");
	log ("  - Intel 8259 interrupt controller: ");
	init8259();
	log ("OK\n");
	log ("  - Intel 8237 DMA controller: ");
	init8237();
	log ("OK\n");
	initVideoPorts();
	if (usessource) {
			log ("  - Disney Sound Source: ");
			initsoundsource();
			log ("OK\n");
		}
#ifndef NETWORKING_OLDCARD
	log ("  - Novell NE2000 ethernet adapter: ");
	isa_ne2000_init (0x300, 6);
	log ("OK\n");
#endif
	log ("  - Adlib FM music card: ");
	initadlib (0x388);
	log ("OK\n");
	log ("  - Creative Labs Sound Blaster Pro: ");
	initBlaster (0x220, 7);
	log ("OK\n");
	log ("  - Serial mouse (Microsoft compatible): ");
	initsermouse (0x3F8, 4);
	log ("OK\n");
	if (doaudio) initaudio();
	inittiming();
	initscreen ( (uint8_t *) build);
}

uint8_t dohardreset = 0;
uint8_t audiobufferfilled();
extern void bufsermousedata (uint8_t value);

void initfake86()
{
	log("%s (c)2010-2012 Mike Chambers\n", build);
	log("[A portable, open-source 8086 PC emulator]\n\n");

	initRAM();

	log("Inserting boot disk\n");
	insertembeddeddisk(0, dosboot, sizeof(dosboot));

	insertembeddeddisk(0x80, dosboot, sizeof(dosboot));

	log("Loading BIOS\n");
	loadembeddedrom(0xFE000UL, pcxtbios, sizeof(pcxtbios));
	log("Loading video ROM\n");
	loadembeddedrom(0xC0000UL, videorom, sizeof(videorom));

#if 0
	if (!loadrom(0xFE000UL, biosfile, 1)) return (-1);
	//	loadrom (0xF6000UL, PATH_DATAFILES "rombasic.bin", 0);
#ifdef DISK_CONTROLLER_ATA
	if (!loadrom(0xD0000UL, PATH_DATAFILES "ide_xt.bin", 1)) return (-1);
#endif
	if (!loadrom(0xC0000UL, PATH_DATAFILES "videorom.bin", 1)) return (-1);
#endif
	log("\nInitializing CPU... ");
	running = 1;
	reset86();
	log("OK!\n");

	inithardware();


}

bool simulatefake86()
{
	if (!running)
		return false;

	exec86(10000);
	if (scrmodechange) doscrmodechange();

	/*while (!audiobufferfilled()) {
		timing();
		tickaudio();
	}*/
	
	
	//handleinput();

	return running != 0;
}

bool drawfake86(uint8_t* buffer)
{
	draw();
	return blitscreen(buffer);
}

void shutdownfake86()
{
	killaudio();
}

void getactivepalette(uint8_t** palette, int* paletteSize)
{
	*palette = (uint8_t*) activepalette;
	*paletteSize = (activepalette == palettevga) ? 256 : 16;
}
