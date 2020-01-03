#pragma once

#include <SDCard/emmc.h>
#include "../src/fake86/disk.h"

class MMCDisk : public DiskInterface
{
private:
	CEMMCDevice* device;

public:
	MMCDisk(CEMMCDevice* inDevice) : device(inDevice) {}
	
	virtual int read (uint8_t *buffer, unsigned count) override 
	{
		return device->Read(buffer, count);
	}
	virtual int write (const uint8_t *buffer, unsigned count) override
	{
		return device->Write(buffer, count);
	}
	virtual uint64_t seek (uint64_t offset) override
	{
		return device->Seek(offset);
	}
	virtual uint64_t getSize() override
	{
		return 1024 * 1024 * 1024;
	}
};
