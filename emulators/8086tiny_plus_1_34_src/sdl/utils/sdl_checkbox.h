/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_checkbox.h
 *
 * DESCRIPTION:
 * This class defines the SDLCheckBox control class
 * This class is derived from the SDLControl class and defines a check box
 * control for use in a SDlControlList.
 * This class also implements exclusive sets so that radio button functionality
 * is also provided.
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

#ifndef __SDL_CHECKBOX_H
#define __SDL_CHECKBOX_H

#include "sdl_control.h"

class SDLCheckBox_t : public SDLControl_t
{
public:

// FUNCTION: Constructor
//
// DESCRIPTION:
// Constructs the check box control.
// This sets the behaviour and appearance of the check box control on creation.
//
// PARAMETERS:
//
//   ControlId  : The id for this control.
//
//   Data       : The general appearance data for the control.
//
//   Exclusive  : Set this to true if the check box is part of an exclusive set
//                (ie. only one control in the set can be checked at a time).
//
//   NextInSet  : If Exclusive is true then this points to the next Check Box
//                control in the exclusive set.
//                If Exclusive is false then this is not used.
//
SDLCheckBox_t(int ControlId, struct SDLControlData_t Data, bool Exclusive, SDLCheckBox_t *NextInSet);

~SDLCheckBox_t();

virtual void Draw(SDL_Surface *Surf);

// FUNCTION: HandleEvent
//
// DESCRIPTION:
// This function handles windows events for the control.
//
// PARAMETERS:
//
//   Event : The event to process
//
// RETURNS:
//
//   bool : true if this windows event triggers an event from this control.
//
virtual bool HandleEvent(SDL_Event &Event);

// FUNCTION: SetChecked
//
// DESCRIPTION:
// Sets the checked status of the check box.
//
// PARAMETERS:
//
//   Checked : true indicates checked, false indicates unchecked.
//
// RETURNS:
//
//   None.
//
void SetChecked(bool Checked);

// FUNCTION: GetCheck
//
// DESCRIPTION:
// Retrieves the check status of the control.
//
// PARAMETERS:
//
//   None.
//
// RETURNS:
//
//   bool : true if the checkbox is currently checked, otherwise false.
//
bool GetCheck(void);

protected:

//
// Checkbox state information
//
bool ButtonPressed;
bool IsChecked;
bool IsExclusive;

//
// Check box exclusive set pointers
//
SDLCheckBox_t *ExclusiveSetStart;
SDLCheckBox_t *ExclusiveSetNext;

};

#endif
