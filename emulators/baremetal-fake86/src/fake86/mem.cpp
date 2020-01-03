#include "types.h"
#include "log.h"

typedef uint32_t size_t;
extern "C" {

	extern void* malloc(size_t size);
}

void copymem(void *dest, const void *src, uint32_t n)
{
	const char *csrc = (const char *)src;
	char *cdest = (char *)dest;

	while (n--)
		*cdest++ = *csrc++;
}

void setmem(void* dest, int value, uint32_t n)
{
	char *cdest = (char *)dest;

	while (n--)
		*cdest++ = (char)(value);
}

uint8_t* allocmem(uint32_t size)
{
	uint8_t* result = new uint8_t[size];
	//log("Allocated %d bytes at 0x%x", size, result);
	setmem(result, 0, size);
	return result;
}

void freemem(uint8_t* ptr)
{
	delete[] ptr;
}