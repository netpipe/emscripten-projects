/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_edit.h
 *
 * DESCRIPTION:
 * This module defines the SDLEdit Control class.
 * This class is derived from the SDLControl class and defines an edit box
 * control for use in a SDLControlList.
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

#ifndef __SDL_EDIT_H
#define __SDL_EDIT_H

#include "sdl_control.h"

class SDLEdit_t : public SDLControl_t
{
public:

SDLEdit_t(int ControlId, struct SDLControlData_t &Data, int BufferLen);
~SDLEdit_t();

// FUNCTION: Draw
//
// DESCRIPTION:
// Draws the SDLEdit_t control.
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
//   Event
//
// RETURNS:
//
//   bool : true if this event triggers an event from this control.
//
virtual bool HandleEvent(SDL_Event &Event);

void SetText(const char *Text);
void GetText(char *Text, int *Length);

protected:

bool HasFocus;    // Does this control have focus?
int CursorPos;    // The cursor position within the edit text
int CursorTicks;  // Tick counter when to next change the cursor
bool CursorBlinkOn; // The current cursor blink state
SDL_TimerID BlinkTimerId;

int TextBufferLen;
int TextLen;
char *TextBuffer;

bool IsCursorIBEAM;
SDL_Cursor *Cursor_IBEAM;
SDL_Cursor *Cursor_System;

};

#endif
