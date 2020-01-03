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

/* ports.c: functions to handle port I/O from the CPU module, as well
   as functions for emulated hardware components to register their
   read/write callback functions across the port address range. */

#include "types.h"
#include "cpu.h"
#include "ports.h"
#include "audio.h"

extern uint8_t keyboardwaitack;

write_redirector* port_write_callback;
read_redirector* port_read_callback;
write_redirector_16* port_write_callback16;
read_redirector_16* port_read_callback16;

void portout (uint16_t portnum, uint8_t value) {
	portram[portnum] = value;
	//printf("portout(0x%X, 0x%02X);\n", portnum, value);
	switch (portnum) {
			case 0x61:
				if ( (value & 3) == 3) speakerenabled = 1;
				else speakerenabled = 0;
				return;
		}

	if (port_write_callback[portnum])
		port_write_callback[portnum](portnum, value);
}

uint8_t portin (uint16_t portnum) {
	//printf("portin(0x%X);\n", portnum);
	switch (portnum) {
			case 0x62:
				return (0x00);
			case 0x60:
			case 0x61:
			case 0x63:
			case 0x64:
				return (portram[portnum]);
		}

	if (port_read_callback[portnum])
		return port_read_callback[portnum](portnum);

	return (0xFF);
}

void portout16 (uint16_t portnum, uint16_t value) {

	if (port_write_callback16[portnum])
		port_write_callback16[portnum](portnum, value);

	portout (portnum, (uint8_t) value);
	portout (portnum + 1, (uint8_t) (value >> 8) );
}

uint16_t portin16 (uint16_t portnum) {
	uint16_t ret;

	if (port_read_callback16[portnum])
		return port_read_callback16[portnum](portnum);

	ret = (uint16_t) portin (portnum);
	ret |= (uint16_t) portin (portnum+1) << 8;
	return (ret);
}

void set_port_write_redirector(uint16_t startport, uint16_t endport, write_redirector callback) {
	uint16_t i;
	for (i=startport; i<=endport; i++) {
			port_write_callback[i] = callback;
		}
}

void set_port_read_redirector (uint16_t startport, uint16_t endport, read_redirector callback) {
	uint16_t i;
	for (i=startport; i<=endport; i++) {
			port_read_callback[i] = callback;
		}
}

void set_port_write_redirector_16 (uint16_t startport, uint16_t endport, write_redirector_16 callback) {
	uint16_t i;
	for (i=startport; i<=endport; i++) {
			port_write_callback16[i] = callback;
		}
}

void set_port_read_redirector_16 (uint16_t startport, uint16_t endport, read_redirector_16 callback) {
	uint16_t i;
	for (i=startport; i<=endport; i++) {
			port_read_callback16[i] = callback;
		}
}
