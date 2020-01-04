IMAGE_OFFSET = 0x95010

KDIR = /usr/src/linux
HEAD = loadlinh.o
SYS_SIZE = -DSYS_SIZE=0xFFFF

OBJECTS = $(HEAD) pgadjust.o

CFLAGS = -D__KERNEL__ -march=i386 -Os -DSTDC_HEADERS -fomit-frame-pointer \
         -fno-strength-reduce -fPIC -ffreestanding -fno-stack-protector

CC=gcc -m32

all: ../pgadjust.asm Makefile
#all: testpg Makefile

../pgadjust.asm: pgadjust.out Makefile
	echo "; 32-bit code, linkaddress: $(IMAGE_OFFSET)" >pgadjust.as
	hexdump -v -e '" DD  " 8/4 "0%Xh," 1/4 "0%Xh\n"' pgadjust.out >>pgadjust.as
	perl -pe 's/\n/\r\n/' <pgadjust.as >../pgadjust.asm
	rm -f pgadjust.as

pgadjust.out: pgadjust Makefile
	objcopy -O binary pgadjust pgadjust.out
#	objdump -k -q  -o $(IMAGE_OFFSET) pgadjust > pgadjust.out
	
loadlinh.o: loadlinh.S
	$(CC) -traditional -c loadlinh.S

pgadjust: $(OBJECTS) Makefile
	ld -m elf_i386 -Ttext $(IMAGE_OFFSET) -N -e startup_32  -o pgadjust $(OBJECTS)
#                      this avoids page align ^^ (and holes between sections)

pgadjust.o: pgadjust.c
	$(CC) $(CFLAGS) -c pgadjust.c -o pgadjust.o

testpg: pgadjust.c
	$(CC) $(CFLAGS) -DSTANDALONE_DEBUG -o testpg pgadjust.c

clean:
	rm -f $(OBJECTS) ../pgadjust.asm pgadjust.out pgadjust

#       for final distrib leave pgadjust.asm in the DOS part
distclean:
	rm -f $(OBJECTS) pgadjust.out pgadjust *~
	(cd ..; mv -f loadlin.exe ..; rm -f *~ *.obj *.lst)
	(cd ..; tar -czf srclinux.tgz srclinux)
