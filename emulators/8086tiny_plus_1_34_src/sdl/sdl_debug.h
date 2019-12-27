// =============================================================================
// File: sdl_debug.h
//
// Description:
// SDL debug dialog.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#ifndef __SDL_DEBUG_H
#define __SDL_DEBUG_H

#include <SDL2/SDL.h>

enum DebugState_t
{
  DEBUG_NONE,    // Debug dialog is not open
  DEBUG_STOPPED, // Debug dialog open and processing is stopped
  DEBUG_STEP     // Debug dialog is open and a single instruction is to be executed.
};

// The number of breakpoints currently set.
// Make this a global for efficiency so a function call is not needed to check
// break points if no break points are set.
extern int BreakpointCount;

// =============================================================================
// Function: DEBUG_Initialise
//
// Description:
// Initialise the debug module.
//
// Parameters:
//
//   mem_in : A pointer the the 8086 emulation RAM.
//
// Returns:
//
//   None.
//
void DEBUG_Initialise(unsigned char *mem_in);

// =============================================================================
// Function: DEBUG_CreateDialog
//
// Description:
// Create the debug dialog.
//
// Parameters:
//
//   None.
//
// Returns:
//
//   None.
//
void DEBUG_CreateDialog(void);

// =============================================================================
// Function: DEBUG_HandleEvent
//
// Description:
// Handle an event in the debug dialog.
//
// Parameters:
//
//   Event : the event to process.
//
// Returns:
//
//   None.
//
void DEBUG_HandleEvent(SDL_Event &Event);

// =============================================================================
// Function: DEBUG_Update
//
// Description:
// Update the debug dialog state.
//
// Parameters:
//
//   None.
//
// Returns:
//
//   None.
//
void DEBUG_Update(void);

// =============================================================================
// Function: DEBUG_GetState
//
// Description:
// Get the current debug state.
//
// Parameters:
//
//   None.
//
// Returns:
//
//   DebugState_t : the current debug state.
//
DebugState_t DEBUG_GetState(void);

// =============================================================================
// Function: DEBUG_CheckBreak
//
// Description:
// Check if a break point has been hit, and if so run the debug dialog
// until step or continue.
//
// Parameters:
//
//   None.
//
// Returns:
//
//   None.
//
void DEBUG_CheckBreak(void);

#endif // __SDL_DEBUG_H
