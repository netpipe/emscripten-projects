/* =============================================================================
 * PROGRAM:  library
 * FILENAME: gl_control_list.h
 *
 * DESCRIPTION:
 * This module define the class used for OpenGL GUI control lists.
 * Note: All controls managed by a control list will be deleted when the
 *       control list is destroyed.
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

#ifndef __GL_CONTROL_LIST_H
#define __GL_CONTROL_LIST_H

#include "sdl_control.h"

// Check that an event is for the specified window id.
bool IsEventForWindow(SDL_Event &Event, Uint32 WindowId);

class SDLControlList_t
{
public:

SDLControlList_t();
~SDLControlList_t();

// FUNCTION: AddControl
//
// DESCRIPTION:
// Adds a control to this control list.
//
// PARAMETERS:
//
//   Control : A pointer to the control to be added.
//
// RETURNS:
//
//   None.
//
void AddControl(SDLControl_t *Control);

// FUNCTION: RemoveControl
//
// DESCRIPTION:
// Removes a control from this control list.
//
// PARAMETERS:
//
//   Control : A pointer to the control to be removed.
//
// RETURNS:
//
//   None.
//
void RemoveControl(SDLControl_t *Control);

// FUNCTION: Reset
//
// DESCRIPTION:
// Resets all controls to their initial default state.
//
// PARAMETERS:
//
//   None.
//
// RETURNS:
//
//   None.
//
void Reset(void);

// FUNCTION: GetControl
//
// DESCRIPTION:
// Finds the control in this control list with the specified Id and
// returns a pointer to that control.
//
// PARAMETERS:
//
//   Id : The Id of the control to find.
//
// RETURNS:
//
//   glControl * : A pointer to the control with the specified Id or NULL
//                 if a control with that Id cannot be found in this
//                 control list.
//
SDLControl_t *GetControl(int Id);

// FUNCTION: HandleEvent
//
// DESCRIPTION:
// Handles the control processing for the specified event.
//
// PARAMETERS:
//
//   Event : the event to process.
//
// RETURNS:
//
//   int : If a control in this control list triggers an event from
//         processing the windows event then the Id of the control is returned,
//         otherwise 0 is returned.
//
int HandleEvent(SDL_Event &Event);

// FUNCTION: Draw
//
// DESCRIPTION:
// Draws the controls in this control list.
//
// PARAMETERS:
//
//   None.
//
// RETURNS:
//
//   None.
//
void Draw(SDL_Surface *Surf);

private:

//
// A pointer to the first control in the control list.
//
SDLControl_t *Head;

};

#endif
