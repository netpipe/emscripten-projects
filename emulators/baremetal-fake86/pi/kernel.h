//
// kernel.h
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
#ifndef _kernel_h
#define _kernel_h

#include <circle/memory.h>
#include <circle/actled.h>
#include <circle/koptions.h>
#include <circle/devicenameservice.h>
#include <circle/screen.h>
#include <circle/serial.h>
#include <circle/exceptionhandler.h>
#include <circle/interrupt.h>
#include <circle/timer.h>
#include <circle/logger.h>
#include <SDCard/emmc.h>
//#include <circle/fs/fat/fatfs.h>
#include <circle/types.h>
#include <circle/usb/dwhcidevice.h>

#define INPUT_BUFFER_SIZE 16

enum TShutdownMode
{
	ShutdownNone,
	ShutdownHalt,
	ShutdownReboot
};

enum class EventType
{
	KeyPress,
	KeyRelease
};

struct InputEvent
{
	EventType eventType;
	u16 scancode;
};

class CKernel
{
public:
	CKernel (void);
	~CKernel (void);

	boolean Initialize (void);

	TShutdownMode Run (void);

	static void KeyStatusHandlerRaw (unsigned char ucModifiers, const unsigned char RawKeys[6]);
	static void KeyPressedHandler (const char *pString);

	
private:
	void QueueEvent(EventType eventType, u16 scancode);

	// do not change this order
	CMemorySystem		m_Memory;
	CActLED			m_ActLED;
	CKernelOptions		m_Options;
	CDeviceNameService	m_DeviceNameService;
	CScreenDevice		m_Screen;
	CSerialDevice		m_Serial;
	CExceptionHandler	m_ExceptionHandler;
	CInterruptSystem	m_Interrupt;
	CTimer			m_Timer;
	CLogger			m_Logger;
	CDWHCIDevice		m_DWHCI;

	CEMMCDevice		m_EMMC;
	//CFATFileSystem		m_FileSystem;

	CBcmFrameBuffer	*m_pFrameBuffer;

	static CKernel *s_pThis;
	
	u8 m_LastModifiers;
	u8 m_LastRawKeys[6];
	InputEvent m_InputBuffer[INPUT_BUFFER_SIZE];
	int m_InputBufferPos, m_InputBufferSize;
};

#endif
