// =============================================================================
// File: sdl_8086tiny_interface.cpp
//
// Description:
// Win32 implementation of the 8086tiny interface class.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#include "8086tiny_interface.h"

//#include <Windows.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <math.h>

#include "file_dialog.h"

#include "sdl_cga.h"
#include "sdl_sound_cfg.h"
#include "serial_emulation.h"

#include "sdl_simple_menu.h"
//#include "sdl_menu_def.i"


// SDL Simple Menu

#define MENU_EMULATION_RESET          101
#define MENU_EMULATION_QUIT           102

#define MENU_CONFIG_SET_FD_IMAGE      201
#define MENU_CONFIG_SET_HD_IMAGE      202
#define MENU_CONFIG_SET_BIOS_IMAGE    203
#define MENU_CONFIG_SET_SPEED         204
#define MENU_CONFIG_SCALE1X           205
#define MENU_CONFIG_SCALE2X           206
#define MENU_CONFIG_CGA_FONT          207
#define MENU_CONFIG_VGA_FONT          208
#define MENU_CONFIG_SERIAL            209
#define MENU_CONFIG_SOUND             210
#define MENU_CONFIG_SAVE_DEFAULT      211
#define MENU_CONFIG_SAVE              212
#define MENU_CONFIG_LOAD              213

#define MENU_DEBUG_START              301

/* debug menu definitions */

struct SDLMENU_Item Debug_Start_Item =
{
  "Start Debugger...",  MENU_DEBUG_START, SDLMENU_UNCHECKED, NULL
};

struct SDLMENU_Menu Debug_Menu =
{
  "Debug",
  NULL,
  &Debug_Start_Item,
  0, 0, 0, 0
};

/* Configuration menu definitions */

struct SDLMENU_Item Config_Load_Item = /* unused */
{
  "Load Configuration...",  MENU_CONFIG_LOAD, SDLMENU_UNCHECKED, NULL
};

struct SDLMENU_Item Config_Save_Item =
{
  "Save Configuration...",  MENU_CONFIG_SAVE, SDLMENU_UNCHECKED, &Config_Load_Item
};

struct SDLMENU_Item Config_Save_Default_Item =
{
  "Save Default Configuration",  MENU_CONFIG_SAVE_DEFAULT, SDLMENU_UNCHECKED, &Config_Save_Item
};

struct SDLMENU_Item Config_Separator4_Item =
{
  NULL,  SDLMENU_SEPARATOR, SDLMENU_UNCHECKED, &Config_Save_Default_Item
};

struct SDLMENU_Item Config_UseVGAFont_Item =
{
  "Use VGA Font",  MENU_CONFIG_VGA_FONT, SDLMENU_UNCHECKED, &Config_Separator4_Item
};

struct SDLMENU_Item Config_UseCGAFont_Item =
{
  "Use CGA Font",  MENU_CONFIG_CGA_FONT, SDLMENU_UNCHECKED, &Config_UseVGAFont_Item
};

struct SDLMENU_Item Config_Separator3_Item =
{
  NULL,  SDLMENU_SEPARATOR, SDLMENU_UNCHECKED, &Config_UseCGAFont_Item
};

struct SDLMENU_Item Config_Scale2X_Item =
{
  "Scale x2",  MENU_CONFIG_SCALE2X, SDLMENU_UNCHECKED, &Config_Separator3_Item
};

struct SDLMENU_Item Config_Scale1X_Item =
{
  "Scale x1",  MENU_CONFIG_SCALE1X, SDLMENU_UNCHECKED, &Config_Scale2X_Item
};

struct SDLMENU_Item Config_Separator2_Item =
{
  NULL,  SDLMENU_SEPARATOR, SDLMENU_UNCHECKED, &Config_Scale1X_Item
};

struct SDLMENU_Item Config_Sound_Item =
{
  "Configure sound...",  MENU_CONFIG_SOUND, SDLMENU_UNCHECKED, &Config_Separator2_Item
};

struct SDLMENU_Item Config_Serial_Item =
{
  "Set Serial Ports...",  MENU_CONFIG_SERIAL, SDLMENU_UNCHECKED, &Config_Sound_Item
};

struct SDLMENU_Item Config_Speed_Item =
{
  "CPU Speed...",  MENU_CONFIG_SET_SPEED, SDLMENU_UNCHECKED, &Config_Serial_Item
};

struct SDLMENU_Item Config_Separator1_Item =
{
  NULL,  SDLMENU_SEPARATOR, SDLMENU_UNCHECKED, &Config_Speed_Item
};

struct SDLMENU_Item Config_BIOSImage_Item =
{
  "Set BIOS Image...",  MENU_CONFIG_SET_BIOS_IMAGE, SDLMENU_UNCHECKED, &Config_Separator1_Item
};

struct SDLMENU_Item Config_HDImage_Item =
{
  "Set HD Image...",  MENU_CONFIG_SET_HD_IMAGE, SDLMENU_UNCHECKED, &Config_BIOSImage_Item
};

struct SDLMENU_Item Config_FDImage_Item =
{
  "Set FD Image...",  MENU_CONFIG_SET_FD_IMAGE, SDLMENU_UNCHECKED, &Config_HDImage_Item
};


struct SDLMENU_Menu Config_Menu =
{
  "Configuration",
  &Debug_Menu,
  &Config_FDImage_Item,
  0, 0, 0, 0
};


/* Emulation menu definitions */

struct SDLMENU_Item Emulation_Quit_Item =
{
  "Quit",  MENU_EMULATION_QUIT, SDLMENU_UNCHECKED, NULL
};

struct SDLMENU_Item Emulation_Reset_Item =
{
  "Reset", MENU_EMULATION_RESET, SDLMENU_UNCHECKED, &Emulation_Quit_Item
};

struct SDLMENU_Menu Emulation_Menu =
{
  "Emulation",
  &Config_Menu,
  &Emulation_Reset_Item,
  0, 0, 0, 0
};


#include "sdl_cpu_speed_dialog.h"
#include "sdl_serial_cfg_dialog.h"
#include "sdl_debug.h"

/* This is the handle for our window */
static SDL_Window *hwndMain = NULL;
static Uint32 MainWindowId = 0;
static int CurrentDispW = 0;
static int CurrentDispH = 0;
static int WindowScaling = 1;
static int MenuHeight = 0;
//static TextDisplay_t WindowTextDisplay = TD_VGA_8x16;

// emulation state control flags
static bool EmulationExitFlag = false;
static bool ResetPending = false;
static bool FDImageChanged = false;

// disk and bios image file names
static char BiosFilename[1024];
static char HDFilename[1024];
static char FDFilename[1024];

int CPU_Clock_Hz = 4770000;
const int PIT_Clock_Hz = 1193181;

int CPU_Counter = 0;
int CPU_Frame = 0;
int PIT_Counter = 0;

// timing control variables
static int INT8_PERIOD_MS = 55;
static int NextInt8Time = 0;
static int Int8Pending = 0;

static unsigned int NextSlowdownTime = 0;

// Mouse state variables

static bool HaveCapture = false;
static bool MouseLButtonDown = false;
static bool MouseRButtonDown = false;

// Sound emulation data
int CurrentSampleRate;

static bool SpkrData = false;
static bool SpkrT2Gate = false;
static bool SpkrT2Out = false;
static bool SpkrT2US = false;  // Is T2 rate ultrasonic? Some games use this
                               // instead of silence!

static SDL_AudioSpec sdl_audio = {48000, AUDIO_U8, 1, 0, 128};
static unsigned char SndBuffer[2048];
static int SND_Counter = 0;
static int SndReadPos = 0;
static int SndWritePos = 0;

// =============================================================================
// PIC 8259 stuff
//

int PIC_OCW_Idx = 0;
unsigned char PIC_OCW[3] = { 0, 0, 0 };
int PIC_ICW_Idx = 0;
unsigned char PIC_ICW[4] = { 0, 0, 0, 0 };

// =============================================================================
// PIT 8253 stuff
//

struct TimerData_t
{
  bool BCD;             // BCD mode
  int Mode;             // Timer mode
  int RLMode;           // Read/Load mode
  int ResetHolding;     // Holding area for timer reset count
  int ResetCount;       // Reload value when count = 0
  int Count;            // Current timer counter
  int Latch;            // Latched timer count: -1 = not latched
  bool LSBToggle;       // Read load LSB (true) /MSB(false) next?
};

const TimerData_t PIT_Channel0Default = { false, 2, 3, 0, 0, 0, -1 , true};
const TimerData_t PIT_Channel1Default = { false, 2, 3, 1024, 1024, 1024, -1, true };
const TimerData_t PIT_Channel2Default = { false, 3, 3, 1024, 1024, 1024, -1, true };


TimerData_t PIT_Channel0 = PIT_Channel0Default;
TimerData_t PIT_Channel1 = PIT_Channel1Default;
TimerData_t PIT_Channel2 = PIT_Channel2Default;

void ResetPIT(void)
{
  PIT_Channel0 = PIT_Channel0Default;
  PIT_Channel1 = PIT_Channel1Default;
  PIT_Channel2 = PIT_Channel2Default;

  INT8_PERIOD_MS = 55;
  NextInt8Time = 0;
}

void PIT_UpdateTimers(int Ticks)
{
  PIT_Channel0.Count -= Ticks;
  while (PIT_Channel0.Count <= 0)
  {
    if (PIT_Channel0.ResetCount == 0)
    {
      PIT_Channel0.Count += 65536;
    }
    else
    {
      PIT_Channel0.Count += PIT_Channel0.ResetCount;
    }

    Int8Pending++;
  }

  // PIT Channel 1 is only used for DRAM refresh.
  // Don't bother update this timer.
  //PIT_Channel1.Count -= Ticks;
  //while (PIT_Channel1.Count <= 0)
  //{
  //  if (PIT_Channel1.ResetCount == 0)
  //  {
  //    PIT_Channel1.Count += 65536;
  //  }
  //  else
  //  {
  //    PIT_Channel1.Count += PIT_Channel1.ResetCount;
  //  }
  //}

  PIT_Channel2.Count -= Ticks;
  if (PIT_Channel2.Mode == 2)
  {
    SpkrT2Out = false;

    if (PIT_Channel2.Count <= 0)
    {
      if (PIT_Channel2.ResetCount == 0)
      {
        PIT_Channel2.Count += 65536;
      }
      else
      {
        PIT_Channel2.Count += PIT_Channel2.ResetCount;
      }
      SpkrT2Out = true;
    }
  }
  else if (PIT_Channel2.Mode == 3)
  {
    if (PIT_Channel2.Count <= 0)
    {
      if (PIT_Channel2.ResetCount == 0)
      {
        PIT_Channel2.Count += 65536;
      }
      else
      {
        PIT_Channel2.Count += PIT_Channel2.ResetCount;
      }
    }
    SpkrT2Out = (PIT_Channel2.Count >= (PIT_Channel2.ResetCount / 2));
  }
}

void PIT_WriteTimer(int T, unsigned char Val)
{
  TimerData_t *Timer;
  if (T == 0)
    Timer = &PIT_Channel0;
  else if (T == 1)
    Timer = &PIT_Channel1;
  else
    Timer = &PIT_Channel2;

  bool WriteLSB = false;

  if (Timer->RLMode == 1)
  {
    WriteLSB = true;
  }
  else if (Timer->RLMode == 3)
  {
    WriteLSB = Timer->LSBToggle;
    Timer->LSBToggle = !Timer->LSBToggle;
  }

  if (WriteLSB)
  {
    Timer->ResetHolding = (Timer->ResetHolding & 0xFF00) | Val;
  }
  else
  {
    Timer->ResetHolding = (Timer->ResetHolding & 0x00FF) | (((int) Val) << 8);
    Timer->ResetCount = Timer->ResetHolding;

    if (Timer->Mode == 0)
    {
      Timer->Count = Timer->ResetCount;
    }

    if (T == 0)
    {
      INT8_PERIOD_MS = (Timer->ResetCount * 1000) / PIT_Clock_Hz;
      if (INT8_PERIOD_MS == 0) INT8_PERIOD_MS = 1;
    }
    else if (T == 2)
    {
      // Is T2 frequency ultrasonic (> 15 kHz)
      SpkrT2US = (Timer->ResetCount < 80);
    }
  }
}

unsigned char PIT_ReadTimer(int T)
{
  TimerData_t *Timer;
  int ReadValue;
  bool ReadLSB = false;
  unsigned char Val;

  if (T == 0)
    Timer = &PIT_Channel0;
  else if (T == 1)
    Timer = &PIT_Channel1;
  else
    Timer = &PIT_Channel2;


  if (Timer->Latch != -1)
  {
    ReadValue = Timer->Latch;
  }
  else
  {
    ReadValue = Timer->Count;
  }

  if (Timer->RLMode == 1)
  {
    ReadLSB = true;
  }
  else if (Timer->RLMode == 3)
  {
    ReadLSB = Timer->LSBToggle;
    Timer->LSBToggle = !Timer->LSBToggle;
  }

  if (ReadLSB)
  {
    Val = (unsigned char)(ReadValue & 0xFF);
  }
  else
  {
    Val = (unsigned char)((ReadValue >> 8) & 0xFF);
    Timer->Latch = -1;
  }

  return Val;
}

void PIT_WriteControl(unsigned char Val)
{
  int T = (Val >> 6) & 0x03;
  TimerData_t *Timer;

  if (T == 0)
    Timer = &PIT_Channel0;
  else if (T == 1)
    Timer = &PIT_Channel1;
  else
    Timer = &PIT_Channel2;

  int RLMode = (Val >> 4) & 0x03;
  if (RLMode == 0)
  {
    Timer->Latch = Timer->Count;
    Timer->LSBToggle = true;
  }
  else
  {
    Timer->RLMode = RLMode;
    if (RLMode == 3) Timer->LSBToggle = true;
  }

  int Mode = (Val >> 1) & 0x07;
  Timer->Mode = Mode;

  Timer->BCD = (Val & 1) == 1;
}

void audio_callback(void *data, unsigned char *stream, int len)
{
	for (int i = 0; i < len; i++)
  {
    if (SndReadPos == SndWritePos)
    {
      stream[i] = sdl_audio.silence;
    }
    else
    {
      stream[i] = SndBuffer[SndReadPos];
      SndReadPos = (SndReadPos + 1) & 2047;
    }
  }
}

// =============================================================================
// Keyboard stuff
//

#define KEYBUFFER_LEN 64
static int KeyBufferHead = 0;
static int KeyBufferTail = 0;
static int KeyBufferCount = 0;
static unsigned char KeyBuffer[KEYBUFFER_LEN];

static unsigned char KeyInputBuffer = 0;
static bool KeyInputFull = false;


// Convert windows virtual key to set 1 scan code for alpha keys
static unsigned char VKAlphaToSet1[26] =
{
  0x1E, 0x30, 0x2E, 0x20, 0x12, 0x21, 0x22, 0x23, 0x17, 0x24, // 'A' .. 'J'
  0x25, 0x26, 0x32, 0x31, 0x18, 0x19, 0x10, 0x13, 0x1f, 0x14, // 'K' .. 'T'
  0x16, 0x2F, 0x11, 0x2D, 0x15, 0x2C                          // 'U' .. 'Z'
};

// Convert windows virtual key to set 1 scan code for digit keys
static unsigned char VKDigitToSet1[10] =
{
  0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B  // '1' .. '0'
};

static unsigned int VKtoSet1Code(SDL_Scancode sc)
{
  // Translate SDL scan codes to Set 1 scan codes

  if ((sc >= SDL_SCANCODE_A) && (sc <= SDL_SCANCODE_Z))
  {
    return VKAlphaToSet1[sc - SDL_SCANCODE_A];
  }

  if ((sc >= SDL_SCANCODE_1) && (sc <= SDL_SCANCODE_0))
  {
    return VKDigitToSet1[sc - SDL_SCANCODE_1];
  }

  if ((sc >= SDL_SCANCODE_F1) && (sc <= SDL_SCANCODE_F10))
  {
    return 0x3B + (sc - SDL_SCANCODE_F1);
  }

  switch (sc)
  {
    case SDL_SCANCODE_RETURN:
      return 0x1C;

    case SDL_SCANCODE_ESCAPE:
      return 0x01;

    case SDL_SCANCODE_SPACE:
      return 0x39;

    case SDL_SCANCODE_LSHIFT:
      return 0x2A;

    case SDL_SCANCODE_RSHIFT:
      return 0x36;

    case SDL_SCANCODE_LCTRL:
      return 0x1D;
    case SDL_SCANCODE_RCTRL:
      return 0x1D;

    case SDL_SCANCODE_LALT: // Alt
      return 0x38;

    case SDL_SCANCODE_LEFT:
      return 0x4B;
    case SDL_SCANCODE_UP:
      return 0x48;
    case SDL_SCANCODE_RIGHT:
      return 0x4D;
    case SDL_SCANCODE_DOWN:
      return 0x50;

    case SDL_SCANCODE_BACKSPACE:
      return 0x0E;

    case SDL_SCANCODE_TAB:
      return 0x0F;

    case SDL_SCANCODE_CAPSLOCK:
      return 0x3A;
    case SDL_SCANCODE_NUMLOCKCLEAR:
      return 0x45;
    case SDL_SCANCODE_SCROLLLOCK:
      return 0x46;

    case SDL_SCANCODE_KP_0:
      return 0x52;
    case SDL_SCANCODE_KP_1:
      return 0x4F;
    case SDL_SCANCODE_KP_2:
      return 0x50;
    case SDL_SCANCODE_KP_3:
      return 0x51;
    case SDL_SCANCODE_KP_4:
      return 0x4B;
    case SDL_SCANCODE_KP_5:
      return 0x4C;
    case SDL_SCANCODE_KP_6:
      return 0x4D;
    case SDL_SCANCODE_KP_7:
      return 0x47;
    case SDL_SCANCODE_KP_8:
      return 0x48;
    case SDL_SCANCODE_KP_9:
      return 0x49;
    case SDL_SCANCODE_KP_MULTIPLY:
      return 0x37;
    case SDL_SCANCODE_KP_MINUS:
      return 0x4A;
    case SDL_SCANCODE_KP_PLUS:
      return 0x4E;
    case SDL_SCANCODE_KP_DIVIDE:
      return 0x35;
    case SDL_SCANCODE_KP_PERIOD:
       return 0x53;

    case SDL_SCANCODE_SEMICOLON: // ';'
      return 0x27;

    case SDL_SCANCODE_EQUALS:
      return 0x0D;

    case SDL_SCANCODE_COMMA:
      return 0x33;

    case SDL_SCANCODE_MINUS:
      return 0x0C;

    case SDL_SCANCODE_PERIOD:
      return 0x34;

    case SDL_SCANCODE_SLASH: // '/'
      return 0x35;

    case SDL_SCANCODE_GRAVE: // '~'
      return 0x29;

    case SDL_SCANCODE_LEFTBRACKET: // '['
      return 0x1A;

    case SDL_SCANCODE_BACKSLASH: // '\'
      return 0x2B;

    case SDL_SCANCODE_RIGHTBRACKET: // ']'
      return 0x1B;

    case SDL_SCANCODE_APOSTROPHE: // '''
      return 0x28;

    case SDL_SCANCODE_F11:

      // F11 is not available on the XT keyboard so use this to test
      // instruction execution (trace on and off)
      return 0x7e;

    case SDL_SCANCODE_F12:
      // F12 is not available on the XT keyboard
      if (HaveCapture)
      {
        HaveCapture = false;
///        SDL_SetRelativeMouseMode(SDL_FALSE);
      }
      return 0xfF;

    default:
      printf("Unhandled key code 0x%02x\n", sc);
      break;

  }

  return 0xFF;
}

static inline void AddKeyEvent(unsigned char code)
{
  if (KeyBufferCount < KEYBUFFER_LEN)
  {
    KeyBuffer[KeyBufferTail] = code;
    KeyBufferTail = (KeyBufferTail + 1) % KEYBUFFER_LEN;
    KeyBufferCount++;
  }
}

static inline  bool IsKeyEventAvailable(void)
{
  return (KeyBufferCount > 0);
}

static inline unsigned char GetKeyEvent(void)
{
  unsigned char code = 0xff;

  if (KeyBufferCount > 0)
  {
    code = KeyBuffer[KeyBufferHead];
  }

  return code;
}

static inline unsigned char NextKeyEvent(void)
{
  unsigned char code = 0xff;

  if (KeyBufferCount > 0)
  {
    code = KeyBuffer[KeyBufferHead];
    KeyBufferHead = (KeyBufferHead + 1) % KEYBUFFER_LEN;
    KeyBufferCount--;
  }

  return code;
}

// ============================================================================
// Configuration read/write stuff
//

int ReadConfig(const char *Filename)
{
  FILE *fp;
  char Line[1024];
  int len;

  fp = fopen(Filename, "r");
  if (fp == NULL)
  {
    /*
     * .cfg file doesn't exist yet
     */
    return 0;
  }

  // Read BIOS image file
  fgets(Line, 256, fp);
  if (strncmp(Line, "[BIOS]", 6) != 0) return 0;

  fgets(Line, 256, fp);
  len = strlen(Line)-1;
  while ((len > 0) && (!isprint(Line[len]))) Line[len--] = 0;
  if (strncmp(Line, "NIL", 3) == 0)
  {
    BiosFilename[0] = 0;
  }
  else
  {
    strncpy(BiosFilename, Line, 1024);
  }

  printf("BIOS = %s\n", BiosFilename);

  // Read floppy image name
  fgets(Line, 256, fp);
  if (strncmp(Line, "[FD]", 4) != 0) return 0;

  fgets(Line, 256, fp);
  len = strlen(Line)-1;
  while ((len > 0) && (!isprint(Line[len]))) Line[len--] = 0;
  if (strncmp(Line, "NIL", 3) == 0)
  {
    FDFilename[0] = 0;
  }
  else
  {
    strncpy(FDFilename, Line, 1024);
  }

  printf("FD = %s\n", FDFilename);

  // Read HD image name
  fgets(Line, 256, fp);
  if (strncmp(Line, "[HD]", 4) != 0) return 0;

  fgets(Line, 256, fp);
  len = strlen(Line)-1;
  while ((len > 0) && (!isprint(Line[len]))) Line[len--] = 0;
  if (strncmp(Line, "NIL", 3) == 0)
  {
    HDFilename[0] = 0;
  }
  else
  {
    strncpy(HDFilename, Line, 1024);
  }

  printf("HD = %s\n", HDFilename);

  // Read CPU speed
  fgets(Line, 256, fp);
  if (strncmp(Line, "[CPU_SPEED]", 11) != 0) return 0;

  fgets(Line, 256, fp);
  sscanf(Line, "%d\n", &CPU_Clock_Hz);

  // Read serial port configuration.
///  SERIAL_ReadConfig(fp);

  // Read sound configuration
  SNDCFG_Read(fp);

  fclose(fp);

  return 1;
}

int WriteConfigFile(const char *Filename)
{
  FILE *fp = fopen(Filename, "wt");
  if (fp == NULL) return 0;

  fprintf(fp, "[BIOS]\n");
  fprintf(fp, "%s\n", BiosFilename[0] ? BiosFilename : "NIL");
  fprintf(fp, "[FD]\n");
  fprintf(fp, "%s\n", FDFilename[0] ? FDFilename : "NIL");
  fprintf(fp, "[HD]\n");
  fprintf(fp, "%s\n", HDFilename[0] ? HDFilename : "NIL");
  fprintf(fp, "[CPU_SPEED]\n");
  fprintf(fp, "%d\n", CPU_Clock_Hz);

///  SERIAL_WriteConfig(fp);
  SNDCFG_Write(fp);

  fclose(fp);
  return 1;
}

static unsigned char *RepaintMem = NULL;

static void RepaintWindow(void)
{
  CGA_DrawScreen(hwndMain, RepaintMem);
}

void HandleEvent(SDL_Event &event)
{
  unsigned int KeyCode;
  int MenuId;
  char Filename[1024];

  // Check that this event id for the main window

  switch (event.type)
  {
    case SDL_WINDOWEVENT:
    {
      if (event.window.windowID != MainWindowId) return;
      break;
    }

    case SDL_KEYDOWN:
    case SDL_KEYUP:
      if (event.key.windowID != MainWindowId) return;
      break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    {
      if (event.button.windowID != MainWindowId) return;
      break;
    }

    case SDL_MOUSEMOTION:
    {
      if (event.motion.windowID != MainWindowId) return;
      break;
    }

    default:
      break;
  }

  if (!HaveCapture)
  {
    MenuId = SDLMENU_HandleEvent(event);
    switch (MenuId)
    {
      case MENU_EMULATION_RESET:
        ResetPending = true;
        break;

      case MENU_EMULATION_QUIT:
        EmulationExitFlag = true;
        break;

      case MENU_CONFIG_SET_FD_IMAGE:
        FDImageChanged = OpenFileDialog(
          "Select floppy image...",
          FDFilename, 1024,
          "image file\0*.img\0\0");
        break;

      case MENU_CONFIG_SET_HD_IMAGE:
        OpenFileDialog(
          "Select hard drive image...",
          HDFilename, 1024,
          "image file\0*.img\0\0");
        break;

      case MENU_CONFIG_SET_BIOS_IMAGE:
        OpenFileDialog(
          "Select BIOS image...",
          BiosFilename, 1024,
          "image file\0*.img\0\0");
        break;

      case MENU_CONFIG_SET_SPEED:
        CPU_SPEED_Dialog(CPU_Clock_Hz);
        break;

      case MENU_CONFIG_SCALE1X:
        SDLMENU_SetCheck(MENU_CONFIG_SCALE1X, SDLMENU_CHECKED);
        SDLMENU_SetCheck(MENU_CONFIG_SCALE2X, SDLMENU_UNCHECKED);
        WindowScaling = 1;
        CGA_SetScale(WindowScaling);
        break;

      case MENU_CONFIG_SCALE2X:
        SDLMENU_SetCheck(MENU_CONFIG_SCALE1X, SDLMENU_UNCHECKED);
        SDLMENU_SetCheck(MENU_CONFIG_SCALE2X, SDLMENU_CHECKED);
        WindowScaling = 2;
        CGA_SetScale(WindowScaling);
        break;

      case MENU_CONFIG_CGA_FONT:
        SDLMENU_SetCheck(MENU_CONFIG_CGA_FONT, SDLMENU_CHECKED);
        SDLMENU_SetCheck(MENU_CONFIG_VGA_FONT, SDLMENU_UNCHECKED);
        CGA_SetTextDisplay(TD_CGA);
        break;

      case MENU_CONFIG_VGA_FONT:
        SDLMENU_SetCheck(MENU_CONFIG_CGA_FONT, SDLMENU_UNCHECKED);
        SDLMENU_SetCheck(MENU_CONFIG_VGA_FONT, SDLMENU_CHECKED);
        CGA_SetTextDisplay(TD_VGA_8x16);
        break;

      case MENU_CONFIG_SERIAL:
        SERIAL_ConfigDialog();
        break;

      case MENU_CONFIG_SOUND:
        if (SNDCFG_Dialog())
        {
          // Check if the sample rate has changed
          if (sdl_audio.freq != AudioSampleRate)
          {
            SDL_CloseAudio();

            sdl_audio.freq = AudioSampleRate;
          	if (SDL_OpenAudio(&sdl_audio, 0) < 0)
            {
              printf("Failed to open audio: %s\n", SDL_GetError());
            }

            SDL_PauseAudio(0);
          }

        }
        break;

      case MENU_CONFIG_SAVE_DEFAULT:
        WriteConfigFile("default.cfg");
        break;

      case MENU_CONFIG_SAVE:
        if (SaveFileDialog("Save configuration...", Filename, 1024, "configuration file\0*.cfg\0\0"))
        {
          WriteConfigFile(Filename);
        }
        break;

      case MENU_CONFIG_LOAD:
        if (OpenFileDialog("Load configuration...", Filename, 1024, "configuration file\0*.cfg\0\0"))
        {
          ReadConfig(Filename);
        }
        break;

      case MENU_DEBUG_START:
        DEBUG_CreateDialog();
        break;

      default:
        break;
    }
  }

  switch (event.type)
  {
    case SDL_WINDOWEVENT:
    {
      switch (event.window.event)
      {
        case SDL_WINDOWEVENT_CLOSE:
          EmulationExitFlag = true;
          break;

        default:
          break;
      }
      break;
    }

    case SDL_KEYDOWN:
      KeyCode = VKtoSet1Code(event.key.keysym.scancode);
      if ((KeyCode & 0xFF00) != 0) AddKeyEvent((KeyCode >> 8) & 0xFF);
      AddKeyEvent(KeyCode & 0xFF);
      break;

    case SDL_KEYUP:
      KeyCode = VKtoSet1Code(event.key.keysym.scancode);
      if ((KeyCode & 0xFF00) != 0) AddKeyEvent((KeyCode >> 8) & 0xFF);
      AddKeyEvent((KeyCode & 0xFF) | 0x80);
      break;

    case SDL_MOUSEBUTTONDOWN:
    {
      switch (event.button.button)
      {
        case SDL_BUTTON_LEFT:
          MouseLButtonDown = true;
          if (!HaveCapture && (event.button.y > MenuHeight))
          {
///            SDL_SetRelativeMouseMode(SDL_TRUE);
            HaveCapture = true;
          }
          break;

        case SDL_BUTTON_RIGHT:
          MouseRButtonDown = true;
          break;

        default:
          break;
      }

///      SERIAL_MouseMove(0, 0, MouseLButtonDown, MouseRButtonDown);
      break;
    }

    case SDL_MOUSEBUTTONUP:
    {
      switch (event.button.button)
      {
        case SDL_BUTTON_LEFT:
          MouseLButtonDown = false;
          break;

        case SDL_BUTTON_RIGHT:
          MouseRButtonDown = false;
          break;

        default:
          break;
      }

///      SERIAL_MouseMove(0, 0, MouseLButtonDown, MouseRButtonDown);
      break;
    }

    case SDL_MOUSEMOTION:
    {
///      SERIAL_MouseMove(event.motion.xrel, event.motion.yrel, MouseLButtonDown, MouseRButtonDown);
      break;
    }

    default:
      break;
  }
}

// =============================================================================
// Interface class.
//

T8086TinyInterface_t::T8086TinyInterface_t()
{
}

T8086TinyInterface_t::~T8086TinyInterface_t()
{
}

bool T8086TinyInterface_t::Initialise(unsigned char *mem_in)
{
  printf("8086 tiny starting\n");

  // Store a pointer to system memory
  mem = mem_in;

  // Initialise ports
  for (int i = 0 ; i < 65536 ; i++)
  {
    Port[i] = 0xff;
  }

  CurrentDispW = 640;
  CurrentDispH = 400;
  hwndMain = SDL_CreateWindow(
    "8086 tiny",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    CurrentDispW, CurrentDispH,
    0);

  MainWindowId = SDL_GetWindowID(hwndMain);

  SDLMENU_SetMenu(hwndMain, &Emulation_Menu, RepaintWindow);
  MenuHeight = SDLMENU_GetMenuHeight();
  SDLMENU_SetCheck(MENU_CONFIG_SCALE1X, SDLMENU_CHECKED);
  SDLMENU_SetCheck(MENU_CONFIG_VGA_FONT, SDLMENU_CHECKED);

  SDL_SetWindowSize(hwndMain, CurrentDispW, CurrentDispH + MenuHeight);

#if defined(__WIN32__)
  // The default windows timer resolution is not good enough for so set
  // it to 1 ms
  timeBeginPeriod(1);
#endif

  CGA_Initialise(hwndMain, MenuHeight);
///  SERIAL_Initialise();
  DEBUG_Initialise(mem_in);

  ReadConfig("default.cfg");

  SDL_Init(SDL_INIT_AUDIO);

  sdl_audio.freq = AudioSampleRate;
	sdl_audio.callback = audio_callback;
	sdl_audio.samples = 512;

	if (SDL_OpenAudio(&sdl_audio, 0) < 0)
  {
    printf("Failed to open audio: %s\n", SDL_GetError());
  }

  SDL_PauseAudio(0);

  return true;

}

void T8086TinyInterface_t::Cleanup(void)
{
#if defined(__WIN32__)
  // Restore the previous timer resolution
  timeEndPeriod(1);
#endif

  SDL_DestroyWindow(hwndMain);
  hwndMain = NULL;

  CGA_Cleanup();
///  SERIAL_Cleanup();
}

bool T8086TinyInterface_t::ExitEmulation(void)
{
  return EmulationExitFlag;
}

bool T8086TinyInterface_t::Reset(void)
{
  if (ResetPending)
  {
    CPU_Counter = 0;
    CPU_Frame = 0;
    PIT_Counter = 0;

    // Reset keyboard
    KeyBufferHead = 0;
    KeyBufferTail = 0;
    KeyBufferCount = 0;
    KeyInputBuffer = 0;
    KeyInputFull = false;

    // Reset sound emulation
    SpkrData = false;
    SpkrT2Gate = false;
    SpkrT2Out = false;
    SpkrT2US = false;

    SND_Counter = 0;
    SndWritePos = 0;
    SndReadPos  = 0;

    CGA_Reset();
///    SERIAL_Reset();
    ResetPIT();
    Int8Pending = false;
    ResetPending = false;

    for (int i = 0 ; i < 4 ; i++) PIC_ICW[i] = 0;
    for (int i = 0 ; i < 3 ; i++) PIC_OCW[i] = 0;
    PIC_ICW_Idx = 0;
    PIC_OCW_Idx = 0;

    return true;
  }
  return false;
}

char *T8086TinyInterface_t::GetBIOSFilename(void)
{
  if (BiosFilename[0] == 0)
  {
    return NULL;
  }

  return BiosFilename;
}

char *T8086TinyInterface_t::GetFDImageFilename(void)
{
  FDImageChanged = false;
  if (FDFilename[0] == 0)
  {
    return NULL;
  }

  return FDFilename;
}

char *T8086TinyInterface_t::GetHDImageFilename(void)
{
  if (HDFilename[0] == 0)
  {
    return NULL;
  }

  return HDFilename;
}

bool T8086TinyInterface_t::FDChanged(void)
{
  return FDImageChanged;
}

bool T8086TinyInterface_t::TimerTick(int nTicks)
{
  int PIT_Ticks;
  SDL_Event event;
  bool NextVideoFrame = false;

  // TODO:
  // This works OK for small values of nTicks, however, the following
  // processing will break down horribly if nTicks is ever larger ( > about 10)
  // If I ever get nTicks per instruction then this will need to change to
  // a loop. Probably safe to process 4 ticks per loop.

  // Update PIT

  PIT_Counter = PIT_Counter + PIT_Clock_Hz * nTicks;
  PIT_Ticks = PIT_Counter / CPU_Clock_Hz;
  PIT_Counter = PIT_Counter % CPU_Clock_Hz;

  PIT_UpdateTimers(PIT_Ticks);

  // Update sound output
  if (SoundEnabled)
  {
    int SoundTicks;
    SND_Counter = SND_Counter + AudioSampleRate * nTicks;
    SoundTicks = SND_Counter / CPU_Clock_Hz;
    SND_Counter = SND_Counter % CPU_Clock_Hz;
    for (int i = 0 ; i < SoundTicks ; i++)
    {
      if (SpkrT2Gate)
      {
        if (SpkrT2US)
        {
          SndBuffer[SndWritePos] = sdl_audio.silence;
        }
        else
        {
          SndBuffer[SndWritePos] = (SpkrT2Out) ? 127+VolumeSample : 127-VolumeSample;
        }
      }
      else
      {
        SndBuffer[SndWritePos] = (SpkrData) ? 127+VolumeSample : sdl_audio.silence;
      }

      SndWritePos = (SndWritePos + 1) & 2047;
    }
  }

  // Debug processing

  DebugState_t Dbg = DEBUG_GetState();
  if ((BreakpointCount != 0) && (Dbg == DEBUG_NONE))
  {
    DEBUG_CheckBreak();
    Dbg = DEBUG_GetState();
    if (Dbg != DEBUG_NONE)
    {
      DEBUG_CreateDialog();
    }
  }

  if (Dbg == DEBUG_STOPPED)
  {
    /* Update the debug dialog state */
    DEBUG_Update();

    /* Run the message loop until the debug state is no longer stopped */
    while (Dbg == DEBUG_STOPPED)
    {
      SDL_WaitEvent(&event);

      DEBUG_HandleEvent(event);

      Dbg = DEBUG_GetState();
    }

    return false;
  }

  // main update processing is every 4 ms of CPU time.

  CPU_Counter += nTicks;
  if (CPU_Counter > (CPU_Clock_Hz / 250))
  {

    CPU_Counter = 0;
    CPU_Frame++;

    if (CPU_Frame == 4)
    {
      int w, h;
      CGA_GetDisplaySize(w, h);
      if ((w != CurrentDispW) || (h != CurrentDispH))
      {
        CurrentDispW = w;
        CurrentDispH = h;

        SDL_SetWindowSize(hwndMain, CurrentDispW, CurrentDispH + MenuHeight);
      }

      RepaintMem = mem;

      SDLMENU_Redraw();
      CGA_DrawScreen(hwndMain, mem);
      NextVideoFrame = true;
      CPU_Frame = 0;

      // Run the message loop until all events have been handled
      while (SDL_PollEvent(&event))
      {
        // Handle event
        HandleEvent(event);
      }
    }

///    SERIAL_HandleSerial();

    unsigned int CurrentTime = SDL_GetTicks();
    if (CurrentTime >= NextSlowdownTime)
    {
      // No slowdown required
      NextSlowdownTime = CurrentTime + 4;
    }
    else
    {
      SDL_Delay(NextSlowdownTime - CurrentTime);
      NextSlowdownTime += 4;
    }

    if (NextVideoFrame)
    {
      CGA_VBlankStart();
    }
  }

  return NextVideoFrame;
}

void T8086TinyInterface_t::CheckBreakPoints(void)
{
  SDL_Event event;

  DebugState_t Dbg = DEBUG_GetState();
  if ((BreakpointCount != 0) && (Dbg == DEBUG_NONE))
  {
    DEBUG_CheckBreak();
    Dbg = DEBUG_GetState();
    if (Dbg != DEBUG_NONE)
    {
      DEBUG_CreateDialog();
    }
  }

  if (Dbg == DEBUG_STOPPED)
  {
    /* Update the debug dialog state */
    DEBUG_Update();

    while (Dbg == DEBUG_STOPPED)
    {
      SDL_WaitEvent(&event);

      DEBUG_HandleEvent(event);

      Dbg = DEBUG_GetState();
    }


  }
}

void T8086TinyInterface_t::WritePort(int Address, unsigned char Value)
{
//  Port[Address] = Value;
//
//  if (CGA_WritePort(Address, Value))
//  {
//    return;
//  }
//
//  if (SERIAL_WritePort(Address, Value))
//  {
//    return;
//  }
//
//  switch (Address)
//  {
//    // PIC Registers
//    case 0x20:
//      if (PIC_OCW_Idx == 0)
//      {
//        if ((Value & 0x10) != 0)
//        {
//          PIC_ICW[0] = Value;
//          PIC_ICW_Idx = 1;
//        }
//      }
//      else
//      {
//        PIC_OCW[PIC_OCW_Idx] = Value;
//        PIC_OCW_Idx++;
//        if (PIC_OCW_Idx > 2) PIC_OCW_Idx = 0;
//      }
//      break;
//    case 0x21:
//      if (PIC_ICW_Idx == 0)
//      {
//        PIC_OCW[0] = Value;
//        PIC_OCW_Idx = 1;
//      }
//      else
//      {
//        PIC_ICW[PIC_ICW_Idx] = Value;
//        PIC_ICW_Idx++;
//        if ((PIC_ICW[0] & 0x02) != 0)
//        {
//          // No ICW3 needed
//          if (PIC_ICW_Idx > 1) PIC_ICW_Idx = 0;
//        }
//        if ((PIC_ICW[0] & 0x01) == 0)
//        {
//          // No ICW 4 needed
//          if (PIC_ICW_Idx > 2) PIC_ICW_Idx = 0;
//        }
//        if (PIC_ICW_Idx > 3) PIC_ICW_Idx = 0;
//      }
//      break;
//
//    // PIT Registers
//    case 0x40:
//      PIT_WriteTimer(0, Value);
//      break;
//    case 0x41:
//      PIT_WriteTimer(1, Value);
//      break;
//    case 0x42:
//      PIT_WriteTimer(2, Value);
//      break;
//    case 0x43:
//      PIT_WriteControl(Value);
//      break;
//
//    case 0x61:
//      SpkrData = ((Value & 0x02) == 0x02);
//      SpkrT2Gate = ((Value & 0x01) == 0x01);
//      break;
//
//    default:
//      //printf("OUT %04x=%02x\n", Address, Value);
//      break;
//  }
}

unsigned char T8086TinyInterface_t::ReadPort(int Address)
{
//  // By default return the last value written to the port.
//  unsigned char retval = Port[Address];
//
//  if (CGA_ReadPort(Address, retval))
//  {
//    return retval;
//  }
//
//  if (SERIAL_ReadPort(Address, retval))
//  {
//    return retval;
//  }
//
//  // Handle specific processing for ports that do something different.
//  switch (Address)
//  {
//    case 0x0020:
//      retval = 0;
//      break;
//    case 0x0021:
//      retval = PIC_OCW[0];
//      break;
//    case 0x0040:
//      retval = PIT_ReadTimer(0);
//      break;
//    case 0x0041:
//      retval = PIT_ReadTimer(1);
//      break;
//    case 0x0042:
//      retval = PIT_ReadTimer(2);
//      break;
//    case 0x0043:
//      break;
//
//    case 0x0060:
//      retval = KeyInputBuffer;
//      KeyInputFull = false;
//      break;
//
//    case 0x0064:
//      retval = 0x14;
//      if (KeyInputFull) retval |= 0x01;
//      break;
//
//    case 0x0201:
//      // joystick is unsupported at the moment
//      retval = 0xff;
//      break;
//
//    default:
//      //printf("IN %04X\n", Address);
//      break;
//  }
//
//  return retval;
}

unsigned int T8086TinyInterface_t::VMemRead(int i_w, int addr)
{
  return CGA_VMemRead(mem, i_w, addr);
}

unsigned int T8086TinyInterface_t::VMemWrite(int i_w, int addr, unsigned int val)
{
  return CGA_VMemWrite(mem, i_w, addr, val);
}

bool T8086TinyInterface_t::IntPending(int &IntNumber)
{
  if (Int8Pending > 0)
  {
    IntNumber = 8;
    Int8Pending--;
    return true;
  }

  if (IsKeyEventAvailable() && !KeyInputFull)
  {
    KeyInputBuffer = NextKeyEvent();
    KeyInputFull = true;
    IntNumber = 9;
    return true;
  }

///  return SERIAL_IntPending(IntNumber);
}

