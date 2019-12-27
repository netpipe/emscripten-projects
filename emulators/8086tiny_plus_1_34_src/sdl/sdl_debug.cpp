// =============================================================================
// File: sdl_debug.cpp
//
// Description:
// SDL debug dialog.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#include <stdio.h>
#include <string.h>

#include "sdl_debug.h"
#include "debug_disasm.h"

#include "sdl_simple_text.h"
#include "sdl_control_list.h"
#include "sdl_button.h"
#include "sdl_checkbox.h"
#include "sdl_edit.h"
#include "sdl_listbox.h"

#include "file_dialog.h"

// These must match 8086tiny.

#define RAM_SIZE 0x10FFF0

#define REGS_BASE 0xF0000

// index into regs16
#define REG_AX 0
#define REG_CX 1
#define REG_DX 2
#define REG_BX 3
#define REG_SP 4
#define REG_BP 5
#define REG_SI 6
#define REG_DI 7

#define REG_ES 8
#define REG_CS 9
#define REG_SS 10
#define REG_DS 11

#define REG_IP 14

// index into regs8
#define FLAG_CF 40
#define FLAG_PF 41
#define FLAG_AF 42
#define FLAG_ZF 43
#define FLAG_SF 44
#define FLAG_TF 45
#define FLAG_IF 46
#define FLAG_DF 47
#define FLAG_OF 48

// =============================================================================
// Local Variables

static DebugState_t DbgState = DEBUG_NONE;

static unsigned char *mem;
static unsigned char *regs8;
static unsigned short *regs16;

static int disass_seg = 0;
static int disass_off = 0;

static int showmem_seg = 0;
static int showmem_off = 0;

static bool LastPosSet = false;
static int LastDialogX = 0;
static int LastDialogY = 0;

// Breakpoints
#define DISASS_NUM_LINES 32

static bool BreakPointSet[RAM_SIZE];
static int DisassLineAddr[DISASS_NUM_LINES];

// Instruction trace
#define TRACE_BUFFER_LENGTH 65536

static bool TraceEnabled = false;
static int TraceLength = 0;
static int TracePos = 0;
static int TraceCS[TRACE_BUFFER_LENGTH];
static int TraceIP[TRACE_BUFFER_LENGTH];

// Window and controls
static SDL_Window *DebugWindow = NULL;
static Uint32 DebugWindowId = 0;

#define ID_BUTTON_STEP            101
#define ID_BUTTON_CONTINUE        102

#define ID_CHKBOX_ENABLE_TRACE    201
#define ID_BUTTON_DUMP_TRACE_FILE 202

#define ID_EDIT_DISASS_ADDR       301
#define ID_LISTBOX_DISSASSEMBLY   302
#define ID_BUTTON_BREAK_SET       303
#define ID_BUTTON_BREAK_CLEAR     304
#define ID_BUTTON_BREAK_CLEAR_ALL 305

#define ID_EDIT_MEMORY_ADDR       401
#define ID_LISTBOX_MEMORY_DUMP    402

static SDLControlList_t *DebugControls = NULL;

static SDLControlData_t ButtonData_Step =
{
  { 10, 610, 80, 24 },
  "Step",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_Continue =
{
  { 770, 610, 80, 24 },
  "Continue",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ChkBoxData_EnableTrace =
{
  { 560, 36, 20, 20 },
  "Enable Trace",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_DumpTraceFile =
{
  { 560, 94, 280, 24 },
  "Dump Trace File",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_DisassAddr =
{
  { 88, 156, 134, 24 },
  "Disassembly Address",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t LBData_Disassembly =
{
  { 20, 190, 290, 370 },
  "Disassembly List",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00A0A0E0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_BreakSet =
{
  { 118, 562, 52, 24 },
  "Set",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_BreakClear =
{
  { 174, 562, 52, 24 },
  "Clear",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_BreakClearAll =
{
  { 230, 562, 80, 24 },
  "Clear All",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_MemoryAddr =
{
  { 408, 156, 134, 24 },
  "Memory Address",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t LBData_Memory =
{
  { 340, 190, 500, 386 },
  "Memory List",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00A0A0E0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};


static SDL_Rect RegDisplayPos[12] =
{
  { 60, 64, 48, 24 },  // AX
  { 260, 64, 48, 24 }, // CX
  { 360, 64, 48, 24 }, // DX
  { 160, 64, 48, 24 }, // BX
  { 60, 94, 48, 24 },  // SP
  { 160, 94, 48, 24 }, // BP
  { 260, 94, 48, 24 }, // SI
  { 360, 94, 48, 24 }, // DI
  { 360, 34, 48, 24 }, // ES
  { 60, 34, 48, 24 },  // CS
  { 160, 34, 48, 24 }, // SS
  { 260, 34, 48, 24 }  // DS
};

static const char *RegNames[12] =
{
  "AX:",
  "CX:",
  "DX:",
  "BX:",
  "SP:",
  "BP:",
  "SI:",
  "DI:",
  "ES:",
  "CS:",
  "SS:",
  "DS:"
};

// =============================================================================
// Local Functions
//

static void Breakpoint_ClearAll(void)
{
  for (int i = 0 ; i < RAM_SIZE ; i++)
  {
    BreakPointSet[i] = false;
  }

  if (TraceEnabled)
  {
    BreakpointCount = 1;
  }
  else
  {
    BreakpointCount = 0;
  }
}

static void DEBUG_SaveTrace(const char *filename)
{
  FILE *fp;
  int TraceIdx;
  char DASMBuffer[64];
  int dasm_addr;

  fp = fopen(filename, "wt");

  if (fp == NULL) return;

  if (TraceLength < TRACE_BUFFER_LENGTH)
  {
    TraceIdx = 0;
  }
  else
  {
    TraceIdx = TracePos;
  }

  for (int i = 0 ; i < TraceLength ; i++)
  {
    dasm_addr = 16 * TraceCS[TraceIdx] + TraceIP[TraceIdx];

    DasmI386(DASMBuffer, mem, dasm_addr, TraceIP[TraceIdx], false);

    fprintf(fp, "%c\t%04X:%04X   %s\n",
            BreakPointSet[dasm_addr] ? 'B' : ' ',
            TraceCS[TraceIdx], TraceIP[TraceIdx], DASMBuffer);

    TraceIdx = (TraceIdx + 1) % TRACE_BUFFER_LENGTH;
  }

  fclose(fp);
}

static void DEBUG_UpdateDisassembly(void)
{
  char RegText[16];
  char DASMBuffer[64];
  char ASMText[128];
  unsigned short reg_ip;
  unsigned int instr_size;
  int dasm_addr;

  sprintf(RegText, "%04X:%04X", disass_seg, disass_off);
  SDLEdit_t *Edit = (SDLEdit_t *) DebugControls->GetControl(ID_EDIT_DISASS_ADDR);
  Edit->SetText(RegText);

  reg_ip = disass_off;

  SDLListBox_t *ListBox = (SDLListBox_t *) DebugControls->GetControl(ID_LISTBOX_DISSASSEMBLY);
  ListBox->Clear();

  for (int i = 0 ; i < DISASS_NUM_LINES ; i++)
  {
    dasm_addr = 16 * disass_seg + reg_ip;
    DisassLineAddr[i] = dasm_addr;

    instr_size = DasmI386(DASMBuffer, mem, dasm_addr, reg_ip, false);

    sprintf(ASMText, "%c %04X:%04X  %s",
            BreakPointSet[dasm_addr] ? 'B' : ' ',
            disass_seg, reg_ip, DASMBuffer);
    ListBox->AddLine(ASMText);
    reg_ip += instr_size;
  }

}

static void DEBUG_UpdateMemDump(void)
{
  char RegText[16];
  char Buffer[128];

  sprintf(RegText, "%04X:%04X", showmem_seg, showmem_off);
  SDLEdit_t *Edit = (SDLEdit_t *) DebugControls->GetControl(ID_EDIT_MEMORY_ADDR);
  Edit->SetText(RegText);

  unsigned int memaddr = showmem_seg * 16 + showmem_off;

  SDLListBox_t *ListBox = (SDLListBox_t *) DebugControls->GetControl(ID_LISTBOX_MEMORY_DUMP);
  ListBox->Clear();

  for (int i = 0 ; i < 1024 ; i+= 16)
  {
    sprintf(
      Buffer,
      "%04X:%04x : %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",
      showmem_seg, (showmem_off + i) % 0xFFFF,
      mem[memaddr+i], mem[memaddr+i+1], mem[memaddr+i+2], mem[memaddr+i+3],
      mem[memaddr+i+4], mem[memaddr+i+5], mem[memaddr+i+6], mem[memaddr+i+7],
      mem[memaddr+i+8], mem[memaddr+i+9], mem[memaddr+i+10], mem[memaddr+i+11],
      mem[memaddr+i+12], mem[memaddr+i+13], mem[memaddr+i+14], mem[memaddr+i+15]);
    ListBox->AddLine(Buffer);
  }
}

static void DEBUG_UpdateControls(void)
{
  SDLCheckBox_t *ChkBox = (SDLCheckBox_t *) DebugControls->GetControl(ID_CHKBOX_ENABLE_TRACE);
  ChkBox->SetChecked(TraceEnabled);

  // Update disassembly
  disass_seg = regs16[REG_CS];
  disass_off = regs16[REG_IP];

  DEBUG_UpdateDisassembly();

  // update RAM display
  DEBUG_UpdateMemDump();
}

static void DrawDebug(void)
{
  if (DebugWindow != NULL)
  {
    SDL_Surface *Surf = SDL_GetWindowSurface(DebugWindow);
    SDL_Rect DRect;
    char Buffer[64];

    DRect.x = 0;
    DRect.y = 0;
    DRect.w = 860;
    DRect.h = 640;
    SDL_FillRect(Surf, &DRect, 0x00C0C0C0);

    // Draw Registers

    SDL_Rect GRect;
    GRect.x = 10;
    GRect.y = 20;
    GRect.w = 510;
    GRect.h = 112;

    Draw3DRect(Surf, GRect, 0x00808080, 0x00FFFFFF);

    GRect.x = 18;
    GRect.y = 14;
    GRect.w = 120;
    GRect.h = 16;
    SDL_FillRect(Surf, &GRect, 0x00C0C0C0);
    SDLTEXT_DrawString(Surf, 22, 14, 0x00000000, "8086 Registers");

    for (int Reg = 0 ; Reg < 12 ; Reg++)
    {
      Draw3DRect(Surf, RegDisplayPos[Reg], 0x00808080, 0x00FFFFFF);
      SDLTEXT_DrawString(Surf, RegDisplayPos[Reg].x-24, RegDisplayPos[Reg].y+4, 0x00000000, RegNames[Reg]);
      sprintf(Buffer, "%04X", regs16[Reg]);
      SDLTEXT_DrawString(Surf, RegDisplayPos[Reg].x+8, RegDisplayPos[Reg].y+4, 0x00000000, Buffer);
    }

    SDLTEXT_DrawString(Surf, 440, 40, 0x00000000, "Flags");
    Buffer[0] = 0;
    strcat(Buffer, (regs8[FLAG_CF]) ? "C" : "-");
    strcat(Buffer, (regs8[FLAG_ZF]) ? "Z" : "-");
    strcat(Buffer, (regs8[FLAG_SF]) ? "S" : "-");
    strcat(Buffer, (regs8[FLAG_OF]) ? "O" : "-");
    strcat(Buffer, (regs8[FLAG_PF]) ? "P" : "-");
    strcat(Buffer, (regs8[FLAG_AF]) ? "A" : "-");

    GRect.x = 420;
    GRect.y = 56;
    GRect.w = 80;
    GRect.h = 24;
    Draw3DRect(Surf, GRect, 0x00808080, 0x00FFFFFF);

    SDLTEXT_DrawString(Surf, 436, 60, 0x00000000, Buffer);

    // Draw Trace

    GRect.x = 550;
    GRect.y = 20;
    GRect.w = 300;
    GRect.h = 112;

    Draw3DRect(Surf, GRect, 0x00808080, 0x00FFFFFF);

    GRect.x = 558;
    GRect.y = 14;
    GRect.w = 48;
    GRect.h = 16;
    SDL_FillRect(Surf, &GRect, 0x00C0C0C0);
    SDLTEXT_DrawString(Surf, 562, 14, 0x00000000, "Trace");

    // Draw Disassembly

    GRect.x = 10;
    GRect.y = 140;
    GRect.w = 310;
    GRect.h = 450;

    Draw3DRect(Surf, GRect, 0x00808080, 0x00FFFFFF);

    GRect.x = 18;
    GRect.y = 134;
    GRect.w = 96;
    GRect.h = 16;
    SDL_FillRect(Surf, &GRect, 0x00C0C0C0);
    SDLTEXT_DrawString(Surf, 22, 134, 0x00000000, "Disassembly");

    SDLTEXT_DrawString(Surf, 22, 160, 0x00000000, "Address:");

    SDLTEXT_DrawString(Surf, 22, 566, 0x00000000, "Breakpoint:");

    // Draw Memory Dump

    GRect.x = 330;
    GRect.y = 140;
    GRect.w = 520;
    GRect.h = 450;

    Draw3DRect(Surf, GRect, 0x00808080, 0x00FFFFFF);

    GRect.x = 338;
    GRect.y = 134;
    GRect.w = 96;
    GRect.h = 16;
    SDL_FillRect(Surf, &GRect, 0x00C0C0C0);
    SDLTEXT_DrawString(Surf, 342, 134, 0x00000000, "Memory Dump");

    SDLTEXT_DrawString(Surf, 342, 160, 0x00000000, "Address:");

    // Draw controls

    DebugControls->Draw(Surf);

    SDL_UpdateWindowSurface(DebugWindow);
  }
}

// =============================================================================
// Exported Variables
//

int BreakpointCount = 0;

// =============================================================================
// Exported Functions
//

void DEBUG_Initialise(unsigned char *mem_in)
{
  mem = mem_in;
  regs8 = mem + REGS_BASE;
  regs16 = (unsigned short *)(mem + REGS_BASE);

  Breakpoint_ClearAll();

  DebugControls = new SDLControlList_t;
  DebugControls->AddControl(new SDLButton_t(ID_BUTTON_STEP, ButtonData_Step));
  DebugControls->AddControl(new SDLButton_t(ID_BUTTON_CONTINUE, ButtonData_Continue));

  DebugControls->AddControl(new SDLCheckBox_t(ID_CHKBOX_ENABLE_TRACE, ChkBoxData_EnableTrace, false, NULL));
  DebugControls->AddControl(new SDLButton_t(ID_BUTTON_DUMP_TRACE_FILE, ButtonData_DumpTraceFile));

  DebugControls->AddControl(new SDLEdit_t(ID_EDIT_DISASS_ADDR, EditData_DisassAddr, 16));
  DebugControls->AddControl(new SDLListBox_t(ID_LISTBOX_DISSASSEMBLY, LBData_Disassembly));
  DebugControls->AddControl(new SDLButton_t(ID_BUTTON_BREAK_SET, ButtonData_BreakSet));
  DebugControls->AddControl(new SDLButton_t(ID_BUTTON_BREAK_CLEAR, ButtonData_BreakClear));
  DebugControls->AddControl(new SDLButton_t(ID_BUTTON_BREAK_CLEAR_ALL, ButtonData_BreakClearAll));

  DebugControls->AddControl(new SDLEdit_t(ID_EDIT_MEMORY_ADDR, EditData_MemoryAddr, 16));
  DebugControls->AddControl(new SDLListBox_t(ID_LISTBOX_MEMORY_DUMP, LBData_Memory));

  DebugControls->GetControl(ID_BUTTON_STEP)->SetActive(true);
  DebugControls->GetControl(ID_BUTTON_CONTINUE)->SetActive(true);
  DebugControls->GetControl(ID_CHKBOX_ENABLE_TRACE)->SetActive(true);
  DebugControls->GetControl(ID_BUTTON_DUMP_TRACE_FILE)->SetActive(true);
  DebugControls->GetControl(ID_EDIT_DISASS_ADDR)->SetActive(true);
  DebugControls->GetControl(ID_LISTBOX_DISSASSEMBLY)->SetActive(true);
  DebugControls->GetControl(ID_BUTTON_BREAK_SET)->SetActive(true);
  DebugControls->GetControl(ID_BUTTON_BREAK_CLEAR)->SetActive(true);
  DebugControls->GetControl(ID_BUTTON_BREAK_CLEAR_ALL)->SetActive(true);
  DebugControls->GetControl(ID_EDIT_MEMORY_ADDR)->SetActive(true);
  DebugControls->GetControl(ID_LISTBOX_MEMORY_DUMP)->SetActive(true);
}

void DEBUG_CreateDialog(void)
{
  if (DebugWindow == NULL)
  {
    if (LastPosSet)
    {
      DebugWindow = SDL_CreateWindow(
        "Debug",
        LastDialogX, LastDialogY,
        860, 640,
        0);
    }
    else
    {
      DebugWindow = SDL_CreateWindow(
        "Debug",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        860, 640,
        0);
    }
    DebugWindowId = SDL_GetWindowID(DebugWindow);
    DbgState = DEBUG_STOPPED;

    DEBUG_UpdateControls();

    DrawDebug();
  }
}

void DEBUG_HandleEvent(SDL_Event &Event)
{
  char Buffer[256];
  int BufferLen;
  int ControlId;
  SDLEdit_t *Edit;
  SDLListBox_t *Listbox;
  SDLCheckBox_t *Checkbox;
  int DasmLine;

  // Check that the event is for this dialog

  if (!IsEventForWindow(Event, DebugWindowId)) return;

  // Process the event

  switch (Event.type)
  {
    case SDL_WINDOWEVENT:
    {
      switch (Event.window.event)
      {
        case SDL_WINDOWEVENT_MOVED:
          LastPosSet = true;
          LastDialogX = Event.window.data1;
          LastDialogY = Event.window.data2;
          break;

        default:
          break;
      }
      break;
    }

    default:
      ControlId = DebugControls->HandleEvent(Event);

      switch (ControlId)
      {
        case ID_CHKBOX_ENABLE_TRACE:
        {
          Checkbox = (SDLCheckBox_t *) DebugControls->GetControl(ID_CHKBOX_ENABLE_TRACE);
          bool NewTraceEnabled = Checkbox->GetCheck();

          if (NewTraceEnabled != TraceEnabled)
          {
            TraceEnabled = NewTraceEnabled;
            if (TraceEnabled)
            {
              BreakpointCount++;
            }
            else
            {
              BreakpointCount--;
            }
          }
          break;
        }

        case ID_BUTTON_DUMP_TRACE_FILE:
        {
          char ofnBuffer[1024];
          if (SaveFileDialog("Save trace file...", ofnBuffer, 1024, "trace file\0*.txt\0\0"))
          {
            DEBUG_SaveTrace(ofnBuffer);
          }
          break;
        }

        case ID_EDIT_DISASS_ADDR:
          Edit = (SDLEdit_t *) DebugControls->GetControl(ID_EDIT_DISASS_ADDR);
          BufferLen = 256;
          Edit->GetText(Buffer, &BufferLen);

          printf("Disassembly Addr changed to %s\n", Buffer);
          sscanf(Buffer, "%X:%X", (unsigned int *) &disass_seg, (unsigned int *) &disass_off);

          // update disassembly display
          DEBUG_UpdateDisassembly();
          break;

        case ID_BUTTON_BREAK_SET:
          Listbox = (SDLListBox_t *) DebugControls->GetControl(ID_LISTBOX_DISSASSEMBLY);
          DasmLine = Listbox->GetSelIdx();
          if (DasmLine != -1)
          {
            if (!BreakPointSet[DisassLineAddr[DasmLine]])
            {
              BreakpointCount++;
              BreakPointSet[DisassLineAddr[DasmLine]] = true;
              DEBUG_UpdateDisassembly();
            }
          }
          break;

        case ID_BUTTON_BREAK_CLEAR:
          Listbox = (SDLListBox_t *) DebugControls->GetControl(ID_LISTBOX_DISSASSEMBLY);
          DasmLine = Listbox->GetSelIdx();
          if (DasmLine != -1)
          {
            if (BreakPointSet[DisassLineAddr[DasmLine]])
            {
              BreakpointCount--;
              BreakPointSet[DisassLineAddr[DasmLine]] = false;
              DEBUG_UpdateDisassembly();
            }
          }
          break;

        case ID_BUTTON_BREAK_CLEAR_ALL:
          Breakpoint_ClearAll();
          DEBUG_UpdateDisassembly();
          break;

        case ID_EDIT_MEMORY_ADDR:
          Edit = (SDLEdit_t *) DebugControls->GetControl(ID_EDIT_MEMORY_ADDR);
          BufferLen = 256;
          Edit->GetText(Buffer, &BufferLen);

          printf("MemAddr changed to %s\n", Buffer);
          sscanf(Buffer, "%X:%X", (unsigned int *) &showmem_seg, (unsigned int *) &showmem_off);

          // update RAM display
          DEBUG_UpdateMemDump();
          break;

        case ID_BUTTON_STEP:
          DbgState = DEBUG_STEP;
          break;

        case ID_BUTTON_CONTINUE:
          DbgState = DEBUG_NONE;
          SDL_DestroyWindow(DebugWindow);
          DebugWindow = NULL;
          DebugWindowId = 0;
          break;
      }
      break;

  }

  DrawDebug();
}

void DEBUG_Update(void)
{
  DEBUG_UpdateControls();
  DrawDebug();
}

DebugState_t DEBUG_GetState(void)
{
  if (DbgState == DEBUG_STEP)
  {
    DbgState = DEBUG_STOPPED;
    return DEBUG_STEP;
  }
  return DbgState;
}

void DEBUG_CheckBreak(void)
{
  if (BreakpointCount == 0)
  {
    return;
  }

  unsigned short reg_cs = regs16[REG_CS];
  unsigned short reg_ip = regs16[REG_IP];

  if (TraceEnabled)
  {
    TraceCS[TracePos] = reg_cs;
    TraceIP[TracePos] = reg_ip;

    if (TraceLength < TRACE_BUFFER_LENGTH) TraceLength++;
    TracePos = (TracePos + 1) % TRACE_BUFFER_LENGTH;
  }

  int Address = reg_cs * 16 + reg_ip;

  if (BreakPointSet[Address])
  {
    DbgState = DEBUG_STOPPED;
  }

}
