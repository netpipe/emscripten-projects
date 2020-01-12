#pragma once

#include "types.h"

void copymem(void *dest, const void *src, uint32_t n);
void setmem(void* dest, int value, uint32_t n);

template<typename T>
void clearmem(T* dest, uint32_t length)
{
	for (uint32_t n = 0; n < length; n++)
	{
		dest[n] = 0;
	}
}

uint8_t* allocmem(uint32_t size);
void freemem(uint8_t* ptr);
