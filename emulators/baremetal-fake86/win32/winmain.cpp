#include <Windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdarg.h>
#include "../src/fake86/fake86.h"
#include "../src/fake86/video.h"
#include "../src/fake86/disk.h"
#include "lodepng.h"

uint8_t blitbuffer[OUTPUT_DISPLAY_WIDTH * OUTPUT_DISPLAY_HEIGHT];

void dumpscreen()
{
	int cursorx, cursory;
	char textbuffer[80 * 50 + 1];

	dumptextscreen(textbuffer, &cursorx, &cursory);
	COORD c = { 0, 0 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);

	printf(textbuffer);

	COORD cursorcoord = { cursorx, cursory };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorcoord);
}

void dumpscreenpng()
{
	unsigned error;
	unsigned char* png;
	size_t pngsize;
	LodePNGState state;

	lodepng_state_init(&state);
	state.info_raw.colortype = LCT_PALETTE;
	state.info_png.color.colortype = LCT_PALETTE;
	state.encoder.auto_convert = false;
	uint8_t* pal;
	int palsize;

	getactivepalette(&pal, &palsize);

	for (int n = 0; n < palsize; n++)
	{
		lodepng_palette_add(&state.info_png.color, pal[n * 4 + 0], pal[n * 4 + 1], pal[n * 4 + 2], 255);
		lodepng_palette_add(&state.info_raw, pal[n * 4 + 0], pal[n * 4 + 1], pal[n * 4 + 2], 255);
	}

	error = lodepng_encode(&png, &pngsize, (unsigned char*)blitbuffer, OUTPUT_DISPLAY_WIDTH, OUTPUT_DISPLAY_HEIGHT, &state);
	if (!error) lodepng_save_file(png, pngsize, "output.png");

	/*if there's an error, display it*/
	if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

	lodepng_state_cleanup(&state);
	free(png);
}

int pressedKeys[256];

int main(int argc, char *argv[])
{
	int count = 0;
	bool screendirty = false;

	initfake86();

	if (argc == 2)
	{
		ImagedDisk* disk = new ImagedDisk(argv[1]);
		if (disk->getSize() > 0)
		{
			insertdisk(128, disk);
		}
		else delete disk;
	}

	while (simulatefake86()) 
	{
		screendirty |= drawfake86(blitbuffer);

		for (int n = 8; n < 256; n++)
		{
			UINT state = GetAsyncKeyState(n);
			bool pressed = (state & (1u << 31));
			UINT scancode = MapVirtualKeyA(n, MAPVK_VK_TO_VSC);
			char charcode = MapVirtualKeyA(n, MAPVK_VK_TO_CHAR);

			if(!pressedKeys[n] && pressed)
			{
				//printf("Pressed %x [%c] V: %x\n", scancode, charcode, n);
				handlekeydownraw(scancode);
			}
			else if (pressedKeys[n] && !pressed)
			{
				//printf("Released %x [%c] V: %x\n", scancode, charcode, n);
				handlekeyupraw(scancode);
			}
			pressedKeys[n] = pressed ? 1 : 0;
		}
		if (_kbhit()) 
		{
			char c = _getch();
		//	handlekeydown(c);
		}
		
		count++;

		if(count > 10 && screendirty)
		{
			count = 0;
			screendirty = false;
			dumpscreen();
			dumpscreenpng();
		}
	}

	return (0);
}

void log(const char* message, ...)
{
	va_list myargs;
	va_start(myargs, message);
	vprintf(message, myargs);
	va_end(myargs);
}

#if 1
uint64_t gethostfreq()
{
	return 1000;
}

uint64_t gettick()
{
	static uint64_t tick = 0;
	return tick++;
}
#else
LARGE_INTEGER queryperf;

uint64_t gethostfreq()
{
	QueryPerformanceFrequency(&queryperf);
	return queryperf.QuadPart;
}

uint64_t gettick()
{
	QueryPerformanceCounter(&queryperf);
	return queryperf.QuadPart;
}
#endif
