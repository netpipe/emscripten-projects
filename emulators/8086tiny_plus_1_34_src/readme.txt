8086 Tiny Plus
==============

This is a modified version of 8086 tiny.

The original 8086 tiny was developed by Adrian Cable.

This version was developed from 8086 tiny by Julian Olds.

Version 1.34 changes
====================
Improved MCGA compatibility. 
This involved intercepting every access to video memory, so some changes to the CPU
emulation were required. 
However, this is a useful stepping stone on the way to VGA support.
Microsoft Word 5.5 now works correctly in mode 11.
Support for CGA palette modification via MCGA palette registers added.
Various BIOS bug fixes.


Version 1.32 changes
====================
Added implementation of the INS and OUTS instructions.
Improved CGA emulation to support CRTC page offset in 320x200 graphics modes.
Improved BIOS compatibility.
BIOS bug fixes.
Improved PIT 8253 support - added mode 0 support for timer 0.
Implemented dummy port 0x0201 to make the system behave correctly for no joystick present.
Fixed SDL debug break on interrupt entry point handling.

Version 1.30
============
Initial release



MODIFICATIONS TO CPU EMULATION
==============================

Warning fixes:
--------------
The code has been cleaned up a bit to remove as many warnings as possible.

NOTE: 
When compiling using gcc with optimisation, make sure you use 
  -fno-strict-aliasing.
The code makes extensive use of type-punned pointers. Without this flag the compiler
assumes strict aliasing and optimises accordingly. This may generate incorrect
code when type-punning is used.

Other changes:
--------------
Exit via CS:IP = 0000:0000 has been removed. An exit flag is now used instead.
This is required because I found a program that carefully disables interrupts,
copies a small function over the interrupt vector table and then calls 0000:0000
to execute the function. It then restores the interrupt table and enables interrupts.
Why? I have no idea, but that is what it does!
To make this program work the exit on CS:IP = 0000:0000 cannot be used.

The CPU emulation has been extended to support some of the new instructions
in the 80186 and NEC V20/V30 CPUs.
The new instructions supported are:
  PUSH imm  : Push immediate
  PUSHA     : Push all registers
  POPA      : Pop all registers
  ENTER     : Format stack for procedure entry 
  LEAVE     : Restore stack on procedure exit
  INS, OUTS : I/O string operations
The instructions not currently supported are:
  BOUND    : This instruction is not useful on a PC
             On a bounds error INT 5 is triggered. On a PC this is the 
             print screen vector.
  IMUL imm : It could be useful, but I don't understand Adrian's MUL_MACRO
             well enough to have a stab at implementng it yet.
             It is low priority as I am yet to see a program actually use this.
             
This version is modified to separate the hardware emulation code from the 
CPU emulation. 
The H/W emulation and host system interface is provided by an interface class. 
Different H/W and host operating systems can be supported by replacing the
implemention of the interface class.

Currently the the following H/W interface classes are provided:
  . MS Windows API,
  . SDL 2

BIOS
====
Also provided is a BIOS that supports a CGA/MCGA graphics card.
This BIOS provides a far more complete support for INT 10h than the 
original 8086tiny BIOS.
It also provies support for INT 14h (serial port).
This BIOS is for a more accurate H/W emulation than the original 8086 tiny
and is NOT compatible with the original 8086 tiny.
This BIOS only uses video memory for video output (no text mode via ANSI).

MS Windows Implementation
=========================
The MS Windows implementation has been tested on Windows 7 and Windows 8.
Clicking in the emulation window will lock mouse input to the emulation.
To release the mouse either press F12 or Alt-Tab.

The MS Windows implementation of the interface class provides support for the 
following:

Graphics: 
  CGA/MCGA
  Graphics modes supported:
    0, 1 : 40x25 text
    2, 3 : 80x25 text
    4, 5 : 320x200, 4 colour CGA mode
    6    : 640x200 B/W CGA mode
    11   : 640x480 B/W MCGA
    13   : 320x200, 256 colours

Sound:
  PC Speaker

Serial Ports:
  Microsoft 2 button serial mouse emulation using system mouse
  Linking of emulation serial ports to host system serial ports.
  TCP/IP serial port emulation over network.

In addition the following features are supported:
  Emulator reset (cold reboot)
  Load/save of configuration files
  On the fly floppy disk swapping
  CPU speed control
  Serial port configuration
  Sound Configuration
  Display scaling (1x or 2x)
  Switch between CGA 8x8 or VGA 8x16 text font
  Built in debugger:
    . register display, 
    . disassembly, 
    . memory display, 
    . single step,
    . break points,
    . CPU tracing - dump last 64K instructions executed.


Limitations:
============
Currently, only boot from floppy disk image is supported.



SDL Implementation
==================
The SDL implementation has been tested on Windows 7 and Windows 8.
Clicking in the emulation window will lock mouse input to the emulation.
To release the mouse either press F12 or Alt-Tab.

Features are the same as for the Windows implementation.

Portability issues:
  1) Serial port hardware interface is currently only available for windows.
  2) The network code for serial over TCP was written for winsock.
     There may be compatability issues under linux.



Configuration File Format
=========================
When the emulation starts the initial configuration is read from default.cfg

The configuration file is in the following format:

[BIOS]
<bios filename>
[FD]
<floppy disk image file name>
[HD]
<hard disk image file name or NIL for no HD>
[CPU_SPEED]
<CPU speed in Hz>
[COM1]
<commport specifier>
[COM2]
<commport specifier>
[COM3]
<commport specifier>
[COM4]
<commport specifier>
[SOUND_ENABLE]
<enable flag,  0 = off, 1 = on>
[SOUND_SAMPLE_RATE]
<sample rate in Hz>
[SOUND_VOLUME]
<volume % (0 .. 100)>


The <comport specifier> can be:
  UNUSED  
    - com port is not in use
  MOUSE   
    - emulated microsoft serial mouse using the system pointing device
  TCP_SERVER:<port>
    - emulated over TCP/IP stream socket as server, connection port is <port>
      TCP data stream is the raw bytes sent/received on the serial port.
  TCP_CLIENT:<address>:<port>
    - emulated over TCP/IP stream socket as a client. 
      Connects to another 8086tiny running a TCP_SERVER serial port running on 
      host at <address>, connection port <port>.
      TCP data stream is the raw bytes sent/received on the serial port.
  COM:<port name> 
   - Link the COMM port to <port name> system serial port

NOTES: 
TCP/IP COM port emulation ignores baud rate (the baud rates at the two ends 
do not have to match for data to be sent/received)
It also makes the serial control lines act like a null modem cable when connected.

There is a bug in the windows sound system under windows 7.
As a result you must match the H/W sample rate in the sound emulation for sounds to 
be played correctly.


TESTED PROGRAMS
===============
OS versions:
  FreeDos (8086 build)
  DOS 3.3
  DOS 4.0
  DOS 5.0
  ELKS 1.5

Driver/TSRs:
  trumouse
    Works, but if the mouse is on COM3 or COM4 you must specify the 
    port on the command line (eg. For COM3: trumouse /C3)
  ctmouse
    Works automatically with mouse on any COM port.
  nnansi

Apps:
  MS-DOS EDIT (DOS 5.0) - requires loadfix
  QBASIC (DOS 5.0) - requires loadfix
  Microsoft Word 5.5

Games:
  Alley Cat
  Apple Panic
  Bard's Tale 1
  Bureaucracy
    Requires an ANSI driver loaded.
  Carrier Command
  Defender
    Needs a slow CPU. Also has the worst keyboard controls ever.
  Digger
    Run with the /C command line parameter for CGA.
  Elite
  Elite+ 
    Use MCGA 256 colours
  Eye of the Beholder
    Need to directly run start1.exe.
    Works with both CGA and MCGA/VGA. Strongly recommend MCGA/VGA.
  Faery Tale
  Heroes of Might and Magic 1
  Jump Man Lives (APOGEE)
  Legend
  Monuments of Mars
  Nord and Bert couldn't make head nor tail of it.
    Requires an ANSI driver loaded.
  Overkill
  Pango
    Needs a really slow CPU speed. Try 3 MHz.
  Pharoah's Tomb
  Populous
  Powermonger
    Requires loadfix.
    Needs a faster CPU to be playable, try 20 Mhz.
    Needs lots of RAM free. Use DOS 3.3 as it uses less RAM.
  Rogue 
  Sim City
  Starflight
  Starflight 2 
    Select VGA 256 colour - uses mode 13, so it is MCGA compatible.
  Starglider
  Starglider II
  Super Boulder Dash
  The Fool's Errand 
    Select VGA - uses mode 13, so it is MCGA compatible.
    Prologue requires loadfix and a CPU speed of at least 8 MHz to run correctly.
    Load/Save to hard disk is problematic, looks like a bug in the code.
    Trying to edit the filename When C: is selected locks up on DOSBOX and 
    causes graphic corruption in 8086 tiny.
    Save to A: is OK
  Ultima 1
  Xfighter