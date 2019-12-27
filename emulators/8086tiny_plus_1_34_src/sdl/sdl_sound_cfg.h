// =============================================================================
// File: sdl_sound_cfg.h
//
// Description:
// SDL sound configuration settings and dialog.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#ifndef __SDL_SOUND_CFG_H
#define __SDL_SOUND_CFG_H

#include <stdio.h>

// =============================================================================
// Exported Variables
//

extern bool SoundEnabled;

extern int AudioSampleRate;

extern int VolumePercent;
extern int VolumeSample;

// =============================================================================
// Exported Functions
//

// =============================================================================
// Function: SNDCFG_Read
//
// Description:
// Read the sound configuration from the configuration file.
//
// Parameters:
//
//   fp : the file being read.
//
// Returns:
//
//   bool : true if a the sound configuration was read successfully.
//
bool SNDCFG_Read(FILE *fp);

// =============================================================================
// Function: SNDCFG_Write
//
// Description:
// Write the sound configuration to the configuration file.
//
// Parameters:
//
//   fp : the file being written.
//
// Returns:
//
//   bool : true if a the sound configuration was written successfully.
//
bool SNDCFG_Write(FILE *fp);

// =============================================================================
// Function: SNDCFG_Dialog
//
// Description:
// Run the sound configuration dialog.
//
// Parameters:
//
//   None
//
// Returns:
//
//   bool : true if a the sound configuration was changed.
//
bool SNDCFG_Dialog(void);

#endif // __SDL_SOUND_CFG_H
