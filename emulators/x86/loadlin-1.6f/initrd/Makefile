CC = gcc
CFLAGS = -O2  -fomit-frame-pointer

all: initdisk freeramdisk Makefile

linuxrc: linuxrc.c
	$(CC) $(CFLAGS) -static -o linuxrc linuxrc.c

initdisk: linuxrc Makefile freeramdisk mkdisk
	./mkdisk linuxrc

freeramdisk: freeramdisk.c
	$(CC) $(CFLAGS) -o freeramdisk freeramdisk.c

clean:
	rm -f hello initdisk freeramdisk *~ *.o linuxrc
