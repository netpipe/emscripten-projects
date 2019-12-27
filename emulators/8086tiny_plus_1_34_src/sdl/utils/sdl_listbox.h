/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_listbox.h
 *
 * DESCRIPTION:
 * This module defines the SDLListBox Control class.
 * This class is derived from the SDLControl class and defines a list box
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

#ifndef __SDL_LISTBOX_H
#define __SDL_LISTBOX_H

#include "sdl_control.h"
#include "sdl_vscroll.h"

class SDLListBox_t : public SDLControl_t
{
public:

SDLListBox_t(int ControlId, struct SDLControlData_t &Data);
~SDLListBox_t();

virtual void SetActive(bool Active);

// FUNCTION: Draw
//
// DESCRIPTION:
// Draws the SDLListBox_t control.
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

// Clear all lines from the control
void Clear(void);

// Add a line to the control
void AddLine(const char *Text);

// Set the text for the currently selected line
void SetSelText(const char *Text);

// Get the number of lines in the control
int GetLineCount(void);

// Returns true if the last event was a double click
bool IsDoubleClickEvent(void);

// Get the currently selected line index
int GetSelIdx(void);

// Set the currently selected line index
void SetSelIdx(int Idx);

// Remove the currently selected line from the control
void DeleteSel(void);

// Get the text at the selected index.
char *GetText(int Idx);

protected:

bool VScrollActive;
SDLVScroll_t *VScroll;

int LineCount;
int SelIndex;
int HilightIndex;

int  LineSelectTick;
bool LastEventIsDoubleClick;

struct LineNode_t
{
  char *Text;
  LineNode_t *Prev;
  LineNode_t *Next;
};

LineNode_t *ListHead;
LineNode_t *ListTail;

bool IsListHit(int x, int y);
int ItemUnderMouse(int x, int y);
void CalcDisplaySize(void);

};

#endif
