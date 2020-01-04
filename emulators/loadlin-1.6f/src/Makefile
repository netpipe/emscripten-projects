# Makefile for LOADLIN-1.6 (C) 1994..1995 Hans Lermen <lermen@elserv.ffm.fgan.de>
#
# To compile with Borland TASM 3.1
#
LISTING = ,
#CCFLAGS = -ml -v -1 -w-pia -f- -y -G -IC:\TC\INCLUDE -LC:\TC\LIB
CCFLAGS = -ml -1 -w-pia -f- -G -IC:\TC\INCLUDE -LC:\TC\LIB
AS	= tasm
#ASFLAGS	= /m3 /zi /DMAKE_EXE_FILE /DDEBUG
ASFLAGS	= /m3 /zi /DMAKE_EXE_FILE
#ASFLAGS	= /m3 /DMAKE_EXE_FILE
LD	= tlink
#LDFLAGS	= /m/v
#         зд need this (/f) for TLINK >3, disables FAR-optimization !!
#LDFLAGS	= /f /m
#LDFLAGS	= /f /m/v
LDFLAGS	= /m
#LDFLAGS	= /v/s/l

#ALPHA = loadlina.asm
#ALPHAD = /DALPHA

all:  loadlin.exe

loadlin.exe:	loadlin.obj makefile
	$(LD) $(LDFLAGS) loadlin.obj, loadlin.exe,,


loadlin.obj:	$(ALPHA) loadlin.asm loadlini.asm loadlinj.asm loadlinm.asm pgadjust.asm makefile
	$(AS) $(ASFLAGS) $(ALPHAD) loadlin, $(LISTING);

