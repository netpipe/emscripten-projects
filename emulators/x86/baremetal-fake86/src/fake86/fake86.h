#pragma once

#include "types.h"
#include "config.h"

void initfake86();
bool simulatefake86();
void shutdownfake86();
bool drawfake86(uint8_t* buffer);
void getactivepalette(uint8_t** palette, int* paletteSize);

void handlekeydown(uint16_t scancode);
void handlekeyup(uint16_t scancode);
void handlekeydownraw(uint16_t scancode);
void handlekeyupraw(uint16_t scancode);

extern uint64_t gethostfreq();
extern uint64_t gettick();
