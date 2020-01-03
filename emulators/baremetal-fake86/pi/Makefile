#
# Makefile
#

CIRCLEHOME = ../../circle

OBJS	= kernel.o main.o ../src/fake86/adlib.o ../src/fake86/ata.o ../src/fake86/audio.o ../src/fake86/blaster.o ../src/fake86/cpu.o ../src/fake86/disk.o ../src/fake86/i8237.o ../src/fake86/i8253.o ../src/fake86/i8259.o ../src/fake86/input.o ../src/fake86/fake86.o ../src/fake86/mem.o ../src/fake86/ports.o ../src/fake86/render.o ../src/fake86/sermouse.o ../src/fake86/sndsource.o ../src/fake86/speaker.o ../src/fake86/timing.o ../src/fake86/video.o
#OBJS	= ../src/fake86/main.o

#LIBS	= $(CIRCLEHOME)/lib/usb/libusb.a \
#	  $(CIRCLEHOME)/lib/input/libinput.a \
#	  $(CIRCLEHOME)/lib/fs/libfs.a \
#	  $(CIRCLEHOME)/lib/libcircle.a

LIBS	= $(CIRCLEHOME)/addon/SDCard/libsdcard.a \
	  $(CIRCLEHOME)/lib/usb/libusb.a \
	  $(CIRCLEHOME)/lib/input/libinput.a \
	  $(CIRCLEHOME)/lib/fs/libfs.a \
	  $(CIRCLEHOME)/lib/libcircle.a
	  
#LIBS	= $(CIRCLEHOME)/addon/SDCard/libsdcard.a \
#	  $(CIRCLEHOME)/lib/fs/fat/libfatfs.a \
#	  $(CIRCLEHOME)/lib/usb/libusb.a \
#	  $(CIRCLEHOME)/lib/input/libinput.a \
#	  $(CIRCLEHOME)/lib/fs/libfs.a \
#	  $(CIRCLEHOME)/lib/libcircle.a

include $(CIRCLEHOME)/sample/Rules.mk
