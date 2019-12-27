/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_control.h
 *
 * DESCRIPTION:
 * This module define the virtual base class used for SDL GUI controls.
 *
 * All controls may be in one of the following states:
 *   Inactive       : Will not respond to mouse events
 *   Active_Normal  : Will respond to mouse events, but the pointer is not
 *                    currently over the control.
 *   Active_Hilight : Will respond to mouse events and the pointer is currently
 *                    over the control.
 *
 *   Active_Pressed : Will respond to mouse events, the pointer is over the
 *                    control and the button is pressed.
 *
 * Different image and colour data is associated with each of these states.
 *
 * Each control also provides the capability to link the controls together into
 * a linked list.
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

#ifndef __SDL_CONTROL_H
#define __SDL_CONTROL_H

#include <SDL2/SDL.h>

//
// This type enumerates the possible control states.
//
enum SDLControlState_t
{
  SDLCONTROL_STATE_INACTIVE,
  SDLCONTROL_STATE_ACTIVE_NORMAL,
  SDLCONTROL_STATE_ACTIVE_HILIGHT,
  SDLCONTROL_STATE_ACTIVE_PRESSED,
  SDLCONTROL_STATE_COUNT
};

inline SDLControlState_t& operator++(SDLControlState_t &rs, int)
{
  return rs = (SDLControlState_t) (rs + 1);
}


struct SDLControlImageData_t
{
  unsigned int FaceColour;  // The RGBA colour to use for the control face
  unsigned int HiColour;    // The RGBA colour to use for 3D bright
  unsigned int LoColour;    // The RGBA colour to use for 3D dark
  unsigned int TextColour;  // The RGBA colour to use for text/detail
};

struct SDLControlData_t
{
  SDL_Rect Rect;
  const char *Text;
  struct SDLControlImageData_t ImageData[SDLCONTROL_STATE_COUNT];
};

// =============================================================================
// Exported Functions
//

void Draw3DRect(SDL_Surface *Surf, SDL_Rect &Rect, unsigned int HiColour, unsigned int LoColour);

// =============================================================================
// Class: SDLControl_t
//
// Description:
// Pure virtual base class for all controls.
// This class defines the interface to all controls and provides
// some common/default processing.
// As a minimum a control must override the pure virtual Draw() function.
//
class SDLControl_t
{
public:

SDLControl_t(int ControlId, struct SDLControlData_t &Data);

virtual ~SDLControl_t();

// FUNCTION: Reset
//
// DESCRIPTION:
// Resets this control to its initial state ready for input.
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

// FUNCTION: GetId
//
// DESCRIPTION:
// returns the Id for this control.
//
// PARAMETERS:
//
//   None.
//
// RETURNS:
//
//   int : The Id of this controls.
//
int GetId(void);

// FUNCTION: Insert
//
// DESCRIPTION:
// Inserts this control into a linked list using the Next and Prev members.
//
// PARAMETERS:
//
//   NextCtrl : A pointer to the next control in the list into which this
//              control is to be inserted.
//
// RETURNS:
//
//   None.
//
void Insert(SDLControl_t *NextCtrl);

// FUNCTION: Remove
//
// DESCRIPTION:
// Removes this control from the linked list.
//
// PARAMETERS:
//
//   None.
//
// RETURNS:
//
//   None.
//
void Remove(void);

// FUNCTION: GetNext
//
// DESCRIPTION:
// Returns the next control in the linked list of controls specified by the
// Next and Prev members.
//
// PARAMETERS:
//
//   None.
//
// RETURNS:
//
//   SDLControl_t * : A pointer to the next control in the linked list.
//
SDLControl_t *GetNext(void);

// FUNCTION: SetPosition
//
// DESCRIPTION:
// Sets the position in which this control is to be drawn.
//
// PARAMETERS:
//
//   Rect : The new position for the control.
//
// RETURNS:
//
//   None.
//
void SetPosition(const SDL_Rect &Rect);

// FUNCTION: SetImage
//
// DESCRIPTION:
// Sets the image data for the control in the specified state.
//
// PARAMETERS:
//
//   State : The control state to have its image data set.
//
//   Data  : The new image data.
//
// RETURNS:
//
//   None.
//
void SetImage(SDLControlState_t State, struct SDLControlImageData_t *Data);

// FUNCTION: SetActive
//
// DESCRIPTION:
// Sets whether this control is active (responds to mouse events).
//
// PARAMETERS:
//
//   Active : The new active state for the control.
//
// RETURNS:
//
//   Mone.
//
virtual void SetActive(bool Active);

// FUNCTION: Draw
//
// DESCRIPTION:
// Draws the control onto the screen.
// This is a pure virtual function to be overridden by the classes for each
// control type.
//
// PARAMETERS:
//
//   None.
//
// RETURNS:
//
//   None.
//
virtual void Draw(SDL_Surface *Surf) = 0;

// FUNCTION: HandleEvent
//
// DESCRIPTION:
// This function handles windows events for the control.
//
// PARAMETERS:
//
//   event : The SDL event
//
// RETURNS:
//
//   bool : true if this windows event triggers an event from this control.
//
virtual bool HandleEvent(SDL_Event &Event);

protected:

//
// A pointer to the next and previous control in the control list.
//
SDLControl_t *Next;
SDLControl_t *Prev;

//
// The number used to identify this control
//
int Id;

//
// The current control state.
//
bool IsActive;

//
// The current state of the control.
//
SDLControlState_t State;

//
// The drawing data for the control.
//
struct SDLControlData_t ControlData;

// FUNCTION: IsHit
//
// DESCRIPTION:
// This function checks if the specified coordinate is over the control.
//
// PARAMETERS:
//
//   x : The X coordinate to check.
//
//   y : The Y coordinate to check.
//
// RETURNS:
//
//   bool : True if the coordinate is within the control drawing area,
//          otherwise false.
//
bool IsHit(int x, int y);

};

#endif


