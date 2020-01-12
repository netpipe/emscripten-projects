/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_hscroll.h
 *
 * DESCRIPTION:
 * This module defines the SDLHScroll Control class.
 * This class is derived from the SDLControl class and defines a horizontal
 * scrollbar control for use in a SDLControlList.
 *
 * =============================================================================
 * COPYRIGHT:
 *
 * Copyright (c) 2005, Julian Olds
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   . Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   . Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 *
 * =============================================================================
 */

#ifndef __SDL_HSCROLL_H
#define __SDL_HSCROLL_H

#include "sdl_control.h"
#include "sdl_button.h"

class SDLHScroll_t : public SDLControl_t
{
public:

SDLHScroll_t(int ControlId, struct SDLControlData_t &Data);
~SDLHScroll_t();

virtual void SetActive(bool Active);

// FUNCTION: Draw
//
// DESCRIPTION:
// Draws the SDLHScroll_t control.
//
// PARAMETERS:
//
//   None.
//
// RETURNS:
//
//   None.
//
virtual void Draw(SDL_Surface *Surf);

// FUNCTION: HandleEvent
//
// DESCRIPTION:
// This function handles events for the control.
//
// PARAMETERS:
//
//   Event : The event to process.
//
// RETURNS:
//
//   bool : true if this event triggers an event from this control.
//
virtual bool HandleEvent(SDL_Event &Event);

void SetThumbSize(int size);
void SetRange(int Min, int Max);
void SetValue(int Val);
void SetStep(int Step);
int GetValue(void);

protected:

// Display Parameters
int SlideTop;   // The top of the slide area in pixels
int SlideSize;  // The size of the slide area
int SlideRange; // The movement range of the slider in pixels
int ThumbPos;   // The thumb position in pixels
int ThumbSize;  // The thumb size in pixels
int StepSize;   // The amount to step when pressing the up/down buttons

//
// Keep track of the thumb pressed status.
//
bool ThumbPressed;

int MinValue;
int MaxValue;
int Range;
int CurrentValue;

SDLButton_t *UpButton;
SDLButton_t *DownButton;

// FUNCTION: IsThumbHit
//
// DESCRIPTION:
// This function checks if the coordinates are within the slider thumb.
//
// PARAMETERS:
//
//   mx, my : The coordinates to check
//
// RETURNS:
//
//   bool : true if mx, my is within the slider thumb.
//
bool IsThumbHit(int mx, int my);

int ThumbPosToValue(int mx);

int ValueToThumbPos(int Value);

};

#endif
