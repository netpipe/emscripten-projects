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

#pragma once
#include "types.h"

class DiskInterface
{
public:
	virtual ~DiskInterface() {}
	virtual int read (uint8_t *buffer, unsigned count) = 0;
	virtual int write (const uint8_t *buffer, unsigned count) = 0;

	virtual uint64_t seek (uint64_t offset) = 0;
	virtual uint64_t getSize() = 0;
};

class EmbeddedDisk : public DiskInterface
{
public:
	EmbeddedDisk(uint8_t* inData, uint64_t inLength) :
		data(inData), length(inLength), position(0) {}
	
	virtual int read (uint8_t *buffer, unsigned count) override;
	virtual int write (const uint8_t *buffer, unsigned count) override;

	virtual uint64_t seek (uint64_t offset) override;
	virtual uint64_t getSize() override { return length; }

private:
	uint8_t* data;
	uint64_t length;	
	uint64_t position;
};

struct struct_drive {
	DiskInterface* disk;
	//uint8_t* embedded;
//	FILE *diskfile;
	uint32_t filesize;
	uint16_t cyls;
	uint16_t sects;
	uint16_t heads;
	uint8_t inserted;
	char *filename;
};

#ifdef _WIN32
#include <stdio.h>
class ImagedDisk : public DiskInterface
{
public:
	ImagedDisk(const char* filename);
	virtual ~ImagedDisk();
	virtual int read (uint8_t *buffer, unsigned count) override;
	virtual int write (const uint8_t *buffer, unsigned count) override;

	virtual uint64_t seek (uint64_t offset) override;
	virtual uint64_t getSize() override;

private:
	FILE* diskFile;
	uint64_t diskSize;
};
#endif

void insertembeddeddisk(uint8_t drivenum, uint8_t* data, uint32_t size);
void insertdisk(uint8_t drivenum, DiskInterface* disk);
void ejectdisk(uint8_t drivenum);
