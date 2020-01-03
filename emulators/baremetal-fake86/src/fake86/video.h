#pragma once

void dumptextscreen(char* outbuffer, int* outcursorx, int* outcursory);

extern uint32_t palettecga[16], palettevga[256], palettemono[2];
extern uint32_t* activepalette;
