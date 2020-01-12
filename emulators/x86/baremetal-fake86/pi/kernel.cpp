//
// kernel.cpp
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2014-2015  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include "kernel.h"
#include <circle/usb/usbkeyboard.h>
#include <circle/string.h>
#include "../src/fake86/fake86.h"
#include "../src/fake86/video.h"
#include "keymap.h"
#include "mmcdisk.h"

#define PARTITION	"emmc1-1"
#define FILENAME	"circle.txt"

static const char FromKernel[] = "kernel";
CKernel *CKernel::s_pThis = 0;
uint64_t CurrentTick = 0;
uint64_t LastTimerSample = 0;

CKernel::CKernel (void)
:	m_Screen (256, 256, true),//(m_Options.GetWidth (), m_Options.GetHeight (), true),
	m_Timer (&m_Interrupt),
	m_Logger (m_Options.GetLogLevel (), &m_Timer),
	m_DWHCI (&m_Interrupt, &m_Timer),
	m_EMMC (&m_Interrupt, &m_Timer, &m_ActLED),
	m_LastModifiers(0),
	m_InputBufferPos(0),
	m_InputBufferSize(0)	
{
	s_pThis = this;
	m_ActLED.Blink (5);	// show we are alive
}

CKernel::~CKernel (void)
{
}

void log(const char* message, ...)
{
	va_list myargs;
	va_start(myargs, message);
	CLogger::Get()->WriteV(FromKernel, LogNotice, message, myargs);
	va_end(myargs);
}

uint64_t gethostfreq()
{
	return CLOCKHZ;
}

uint64_t gettick()
{
	uint32_t timerSample = CTimer::GetClockTicks();
	uint32_t delta = timerSample >= LastTimerSample ? (timerSample - LastTimerSample) : (0xffffffff - LastTimerSample) + timerSample;
	LastTimerSample = timerSample;
	CurrentTick += delta;
	return CurrentTick;
}

extern uint32_t palettecga[16];

boolean CKernel::Initialize (void)
{
	boolean bOK = TRUE;
	
	m_pFrameBuffer = new CBcmFrameBuffer (OUTPUT_DISPLAY_WIDTH, OUTPUT_DISPLAY_HEIGHT, 8);
    
	for(int n = 0; n < 16; n++)
	{
		m_pFrameBuffer->SetPalette32 (n, palettecga[n]);  
	}
	//m_pFrameBuffer->SetPalette (0, 0x0000);  // black
	//m_pFrameBuffer->SetPalette (1, 0x0010);  // blue
	//m_pFrameBuffer->SetPalette (2, 0x8000);  // red
	//m_pFrameBuffer->SetPalette (3, 0x8010);  // magenta
	//m_pFrameBuffer->SetPalette (4, 0x0400);  // green
	//m_pFrameBuffer->SetPalette (5, 0x0410);  // cyan
	//m_pFrameBuffer->SetPalette (6, 0x8400);  // yellow
	//m_pFrameBuffer->SetPalette (7, 0x8410);  // white
	//m_pFrameBuffer->SetPalette (8, 0x0000);  // black
	//m_pFrameBuffer->SetPalette (9, 0x001F);  // bright blue
	//m_pFrameBuffer->SetPalette (10, 0xF800); // bright red
	//m_pFrameBuffer->SetPalette (11, 0xF81F); // bright magenta
	//m_pFrameBuffer->SetPalette (12, 0x07E0); // bright green
	//m_pFrameBuffer->SetPalette (13, 0x07FF); // bright cyan
	//m_pFrameBuffer->SetPalette (14, 0xFFE0); // bright yellow
	//m_pFrameBuffer->SetPalette (15, 0xFFFF); // bright white

	if (!m_pFrameBuffer->Initialize()) {
		return FALSE;
	}
	
	if (bOK)
	{
		bOK = m_Screen.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Serial.Initialize (115200);
	}

	if (bOK)
	{
		CDevice *pTarget = m_DeviceNameService.GetDevice (m_Options.GetLogDevice (), FALSE);
		if (pTarget == 0)
		{
			pTarget = &m_Screen;
		}

		bOK = m_Logger.Initialize (pTarget);
	}

	if (bOK)
	{
		bOK = m_Interrupt.Initialize ();
	}

	if (bOK)
	{
		bOK = m_Timer.Initialize ();
	}

	if (bOK)
	{
		bOK = m_DWHCI.Initialize ();
	}

	if (bOK)
	{
		bOK = m_EMMC.Initialize ();
	}

	LastTimerSample = CTimer::GetClockTicks();
	

	return bOK;
}

TShutdownMode CKernel::Run (void)
{
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);
	/*
	m_Logger.Write (FromKernel, LogNotice, "Compile time: " __DATE__ " " __TIME__);

	// Mount file system
	CDevice *pPartition = m_DeviceNameService.GetDevice (PARTITION, TRUE);
	if (pPartition == 0)
	{
		m_Logger.Write (FromKernel, LogPanic, "Partition not found: %s", PARTITION);
	}

	if (!m_FileSystem.Mount (pPartition))
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot mount partition: %s", PARTITION);
	}

	// Show contents of root directory
	TDirentry Direntry;
	TFindCurrentEntry CurrentEntry;
	unsigned nEntry = m_FileSystem.RootFindFirst (&Direntry, &CurrentEntry);
	for (unsigned i = 0; nEntry != 0; i++)
	{
		if (!(Direntry.nAttributes & FS_ATTRIB_SYSTEM))
		{
			CString FileName;
			FileName.Format ("%-14s", Direntry.chTitle);

			m_Screen.Write ((const char *) FileName, FileName.GetLength ());

			if (i % 5 == 4)
			{
				m_Screen.Write ("\n", 1);
			}
		}

		nEntry = m_FileSystem.RootFindNext (&Direntry, &CurrentEntry);
	}
	m_Screen.Write ("\n", 1);

	// Create file and write to it
	unsigned hFile = m_FileSystem.FileCreate (FILENAME);
	if (hFile == 0)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot create file: %s", FILENAME);
	}

	for (unsigned nLine = 1; nLine <= 5; nLine++)
	{
		CString Msg;
		Msg.Format ("Hello File! (Line %u)\n", nLine);

		if (m_FileSystem.FileWrite (hFile, (const char *) Msg, Msg.GetLength ()) != Msg.GetLength ())
		{
			m_Logger.Write (FromKernel, LogError, "Write error");
			break;
		}
	}

	if (!m_FileSystem.FileClose (hFile))
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot close file");
	}

	// Reopen file, read it and display its contents
	hFile = m_FileSystem.FileOpen (FILENAME);
	if (hFile == 0)
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot open file: %s", FILENAME);
	}

	char Buffer[100];
	unsigned nResult;
	while ((nResult = m_FileSystem.FileRead (hFile, Buffer, sizeof Buffer)) > 0)
	{
		if (nResult == FS_ERROR)
		{
			m_Logger.Write (FromKernel, LogError, "Read error");
			break;
		}

		m_Screen.Write (Buffer, nResult);
	}
	
	if (!m_FileSystem.FileClose (hFile))
	{
		m_Logger.Write (FromKernel, LogPanic, "Cannot close file");
	}*/

	initfake86();
	
	insertdisk(128, new MMCDisk(&m_EMMC));
	//CDevice *pPartition = m_DeviceNameService.GetDevice (PARTITION, TRUE);
	//if (pPartition)
	//{
	//	
	//}

	CUSBKeyboardDevice *pKeyboard = (CUSBKeyboardDevice *) m_DeviceNameService.GetDevice ("ukbd1", FALSE);
	
	if(pKeyboard)
	{
		pKeyboard->RegisterKeyStatusHandlerRaw (KeyStatusHandlerRaw);
		//	pKeyboard->RegisterKeyPressedHandler (KeyPressedHandler);
	}
	else return ShutdownHalt;
	
	char screentext[80*25+1];
	int cursorX, cursorY;
	int firstRow = 5;
	int count = 0;
	uint32_t lastRefreshTime = CTimer::GetClockTicks();
	const int refreshRate = 60;
	const int refreshDelay = CLOCKHZ / refreshRate;
	
	while(simulatefake86())
	{
		if(pKeyboard)
			pKeyboard->UpdateLEDs ();
		
		// TODO: This should really be based on screen refresh rate / vsync interrupt
		uint32_t currentTime = CTimer::GetClockTicks();
		uint32_t timeElapsedSinceRefresh = (currentTime >= lastRefreshTime) ? (currentTime - lastRefreshTime) : (0xffffffff - lastRefreshTime) + (currentTime);
		
		if(timeElapsedSinceRefresh >= refreshDelay)
		{
			uint32_t* palettePtr;
			int paletteSize;
			
			getactivepalette((uint8_t**)&palettePtr, &paletteSize);
			
			for(int n = 0; n < paletteSize; n++)
			{
				m_pFrameBuffer->SetPalette32 (n, palettePtr[n]);
			}
			m_pFrameBuffer->UpdatePalette();
			
			drawfake86((uint8_t*) m_pFrameBuffer->GetBuffer());
			lastRefreshTime = currentTime;
			//m_Timer.MsDelay (1);
		}
		//m_Screen.CursorMove(firstRow, 0);
		/*count++;
		
		if(count >= 100)
		{
			dumptextscreen(screentext, &cursorX, &cursorY);
			//m_Screen.CursorMove(firstRow + cursorY, cursorX);
			m_Logger.Write(FromKernel, LogNotice, screentext);
			count = 0;
			m_Timer.MsDelay (100);
		}*/
		
		while(m_InputBufferSize > 0)
		{
			if(m_InputBuffer[m_InputBufferPos].eventType == EventType::KeyPress)
			{
				handlekeydownraw(m_InputBuffer[m_InputBufferPos].scancode);
			}
			else
			{
				handlekeyupraw(m_InputBuffer[m_InputBufferPos].scancode);
			}
			m_InputBufferPos++;
			m_InputBufferSize --;
			if(m_InputBufferPos >= INPUT_BUFFER_SIZE)
			{
				m_InputBufferPos = 0;
			}
		}
	}

	return ShutdownHalt;
}

void CKernel::QueueEvent(EventType eventType, u16 scancode)
{
	if(m_InputBufferSize < INPUT_BUFFER_SIZE && scancode != 0)
	{
		int writePos = (m_InputBufferPos + m_InputBufferSize) % INPUT_BUFFER_SIZE;
		m_InputBuffer[writePos].eventType = eventType;
		m_InputBuffer[writePos].scancode = scancode;
		s_pThis->m_InputBufferSize ++;
	}
}


void CKernel::KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6])
{
	for(int n = 0; n < 8; n++)
	{
		int mask = 1 << n;
		bool wasPressed = (s_pThis->m_LastModifiers & mask) != 0;
		bool isPressed = (ucModifiers & mask) != 0;
		if(!wasPressed && isPressed)
		{
			s_pThis->QueueEvent(EventType::KeyPress, modifier2xtMapping[n]);
		}
		else if(wasPressed && !isPressed)
		{
			s_pThis->QueueEvent(EventType::KeyRelease, modifier2xtMapping[n]);
		}
	}
		
	for(int n = 0; n < 6; n++)
	{
		if(s_pThis->m_LastRawKeys[n] != 0)
		{
			bool inNewBuffer = false;
			
			for(int i = 0; i < 6; i++)
			{
				if(s_pThis->m_LastRawKeys[n] == RawKeys[i])
				{
					inNewBuffer = true;
					break;
				}
			}
			
			if(!inNewBuffer && s_pThis->m_InputBufferSize < INPUT_BUFFER_SIZE)
			{
				s_pThis->QueueEvent(EventType::KeyRelease, usb2xtMapping[s_pThis->m_LastRawKeys[n]]);
			}
		}
	}

	for(int n = 0; n < 6; n++)
	{
		if(RawKeys[n] != 0)
		{
			bool inLastBuffer = false;
			
			for(int i = 0; i < 6; i++)
			{
				if(s_pThis->m_LastRawKeys[i] == RawKeys[n])
				{
					inLastBuffer = true;
					break;
				}
			}
			
			if(!inLastBuffer && s_pThis->m_InputBufferSize < INPUT_BUFFER_SIZE)
			{
				s_pThis->QueueEvent(EventType::KeyPress, usb2xtMapping[RawKeys[n]]);
			}
		}
	}

	for(int n = 0; n < 6; n++)
	{
		s_pThis->m_LastRawKeys[n] = RawKeys[n];
	}

	s_pThis->m_LastModifiers = ucModifiers;

//	s_InputBuffer[n] = RawKeys[n];
	
	//assert (s_pThis != 0);
    //
	//CString Message;
	//Message.Format ("Key status (modifiers %02X)", (unsigned) ucModifiers);
    //
	//for (unsigned i = 0; i < 6; i++)
	//{
	//	if (RawKeys[i] != 0)
	//	{
	//		CString KeyCode;
	//		KeyCode.Format (" %02X", (unsigned) RawKeys[i]);
    //
	//		Message.Append (KeyCode);
	//	}
	//}

}

void CKernel::KeyPressedHandler (const char *pString)
{
	for(int n = 0; n < INPUT_BUFFER_SIZE && s_pThis->m_InputBufferSize < INPUT_BUFFER_SIZE; n++)
	{
		if(pString[n])
		{
			u8 keycode = pString[n];
			
			if(keycode == '\n')
				keycode = 13;
			s_pThis->QueueEvent(EventType::KeyPress, keycode);
		}
		else break;
	}
	//s_pThis->m_Screen.Write (pString, strlen (pString));
}

