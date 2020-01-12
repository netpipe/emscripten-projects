/*
 * padjust.c
 * 
 * partially stolen from linux/arch/i386/boot/compressed/misc.c
 *
 */

#ifdef STANDALONE_DEBUG
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
#endif



/*
 * These are set up by the setup-routine at boot-time:
 */

struct screen_info {
	unsigned char  orig_x;
	unsigned char  orig_y;
	unsigned char  unused1[2];
	unsigned short orig_video_page;
	unsigned char  orig_video_mode;
	unsigned char  orig_video_cols;
	unsigned short unused2;
	unsigned short orig_video_ega_bx;
	unsigned short unused3;
	unsigned char  orig_video_lines;
	unsigned char  orig_video_isVGA;
};

/*
 * This is set up by the setup-routine at boot-time
 */
#define EXT_MEM_K (*(unsigned short *)0x90002)
#define DRIVE_INFO (*(struct drive_info *)0x90080)
#define SCREEN_INFO (*(struct screen_info *)0x90000)
#define RAMDISK_SIZE (*(unsigned short *)0x901F8)
#define ORIG_ROOT_DEV (*(unsigned short *)0x901FC)
#define AUX_DEVICE_INFO (*(unsigned char *)0x901FF)

/* --------------------------------------------------------------------- */

/*
 * This (also set up by the setup-routine) is our special extension
 * Here we have all params we got from 16-bit LOADLIN
 */

struct pblock {
  unsigned long taddr;    /* linear address where the block of pages
                             must be moved to */
  unsigned short start;   /* index within sources of first entry */
  unsigned short tcount;  /* number of entries for taddr in sources */
};

struct source_entry {
  unsigned long unused:12;
  unsigned long pagenum:20;
  unsigned long usedby;
  #define IS_UNUSED  0xffffffffUL
  #define IS_MOVED   0xfffffffeUL
  #define IS_PGLIST  0x80000000UL
};

struct pages_list {
  unsigned long count;     /* number of entries in 'sources' */
  long number_of_blocks;   /* number of valid blocks-items */
  void *auxbuf;            /* address of 4096 bytes auxiliary buffer */
  struct pblock blocks[4];
  struct source_entry *(sources[1]); /* list of addresses where the block of pages
                              currently _is_ located */
};

static unsigned long setup_base = 0x95000;

#ifndef STANDALONE_DEBUG
#define PAGELIST     (*(struct pages_list **)(setup_base + 8))
#else
  static struct pages_list *PAGELIST=0;
#endif

#define S(i) PAGELIST->sources[(i) / 512][(i) % 512]
#define REAL_STARTUP_32  ( *((unsigned long *)(setup_base + 0xc)) )

/* --------------------------------------------------------------------- */


static void error(char *m);
 
#ifndef STANDALONE_DEBUG

	/* ---- outb_p stolen from linux/include/asm/io.h ---- */

#define SLOW_DOWN_IO __asm__ __volatile__("outb %al,$0x80")

#define __OUT1(s,x) \
static inline void __out##s(unsigned x value, unsigned short port) {

#define __OUT2(s,s1,s2) \
__asm__ __volatile__ ("out" #s " %" s1 "0,%" s2 "1"

#define __OUT(s,s1,x) \
__OUT1(s,x) __OUT2(s,s1,"w") : : "a" (value), "d" (port)); } \
__OUT1(s##c,x) __OUT2(s,s1,"") : : "a" (value), "id" (port)); } \
__OUT1(s##_p,x) __OUT2(s,s1,"w") : : "a" (value), "d" (port)); SLOW_DOWN_IO; } \
__OUT1(s##c_p,x) __OUT2(s,s1,"") : : "a" (value), "id" (port)); SLOW_DOWN_IO; }

__OUT(b,"b",char)

#define outb_p(val,port) \
((__builtin_constant_p((port)) && (port) < 256) ? \
	__outbc_p((val),(port)) : \
	__outb_p((val),(port)))

	/* ---- end of outb_p ----- */

static int puts(const char *);
static void * memcpy(void * __dest, void * __src,
			    unsigned int __n);
  
static char *vidmem = (char *)0xb8000;
static int vidport;
static int lines, cols;

static void scroll()
{
	int i;

	memcpy ( vidmem, vidmem + cols * 2, ( lines - 1 ) * cols * 2 );
	for ( i = ( lines - 1 ) * cols * 2; i < lines * cols * 2; i += 2 )
		vidmem[i] = ' ';
}

static int puts(const char *s)
{
	int x,y,pos;
	char c;

	x = SCREEN_INFO.orig_x;
	y = SCREEN_INFO.orig_y;

	while ( ( c = *s++ ) != '\0' ) {
		if ( c == '\n' ) {
			x = 0;
			if ( ++y >= lines ) {
				scroll();
				y--;
			}
		} else {
			vidmem [ ( x + cols * y ) * 2 ] = c; 
			if ( ++x >= cols ) {
				x = 0;
				if ( ++y >= lines ) {
					scroll();
					y--;
				}
			}
		}
	}

	SCREEN_INFO.orig_x = x;
	SCREEN_INFO.orig_y = y;

	pos = (x + cols * y) * 2;	/* Update cursor position */
	outb_p(14, vidport);
	outb_p(0xff & (pos >> 9), vidport+1);
	outb_p(15, vidport);
	outb_p(0xff & (pos >> 1), vidport+1);

	return 0;
}

static void * memcpy(void * __dest, void * __src,
			    unsigned int __n)
{
	int i;
	char *d = (char *)__dest, *s = (char *)__src;

	for (i=0;i<__n;i++) d[i] = s[i];
}
#endif

static void put_hex(unsigned long v)
{
  static char table[17]="0123456789ABCDEF";
  char b[9];
  int i;
  for (i=7; i >=0; i--) {
    b[i] = table[v & 15];
    v >>=4;
  }
  b[8]=0;
  puts(b);
}


static void error(char *x)
{
	puts("\n\n");
	puts(x);
	puts("\n\n -- System halted");

	while(1);	/* Halt */
}

#ifdef STANDALONE_DEBUG
static void memcpy_page(void *t,void *s)
{
  printf("%p = %p\n",t,s);
}
#else
static void memcpy_page(void *t,void *s)
{
  memcpy(t,s,4096);
}
#endif

static int index_within_sources(unsigned long pgnum)
{
  int i;
  for (i=0; i< PAGELIST->number_of_blocks; i++) {
    unsigned long pstart=(PAGELIST->blocks[i].taddr >> 12);
    unsigned long pstop= pstart + PAGELIST->blocks[i].tcount;
    if ((pgnum >= pstart) && (pgnum < pstop)) {
      return (pgnum - pstart) + PAGELIST->blocks[i].start;
    }
  }
  return IS_UNUSED;
}

static void * address_of_target(int sindex)
{
  int i, j=0;
  for (i=0; i < PAGELIST->number_of_blocks; i++) {
    j += PAGELIST->blocks[i].tcount;
    if (sindex < j) {
      return (void *)(((PAGELIST->blocks[i].taddr >> 12) + (sindex - PAGELIST->blocks[i].start)) << 12);
    }
  }
  return 0; /* invalid */
}


static void build_references(void)
{
  int i,j;
  for (i=0; i < PAGELIST->count; i++) {
    S(i).usedby = IS_UNUSED;
  }
  for (i=0; i < (PAGELIST->count+511) / 512; i++) {
    j = index_within_sources((unsigned long)PAGELIST->sources[i] >> 12);
    if (j != IS_UNUSED)
      S(j).usedby = IS_PGLIST | i;
  }
  for (i=0; i < PAGELIST->count; i++) {
    j= index_within_sources(S(i).pagenum);
    if ( j != IS_UNUSED ) {
      if (j == i) S(i).usedby = IS_MOVED;
      else S(j).usedby = i;
    }
  }
}

static int recursion_start_index=0;


#ifdef VERBOSE
static printmove(int sindex)
{
  static int count=0;
  if (!(count++ & 3)) puts("\n");
  put_hex((long)address_of_target(sindex));
  puts(" <-- ");
  put_hex(S(sindex).pagenum << 12);
  puts("  ");
}
#endif


static void make_free(int sindex)
{
  if (S(sindex).usedby == IS_MOVED) return;
  if (S(sindex).usedby == IS_UNUSED) {
  } else
  if (S(sindex).usedby & IS_PGLIST) {
    int index = S(sindex).usedby & ~IS_PGLIST;
    void *addr = (void*) (S(recursion_start_index).pagenum << 12);
    int j;
    /* we can just exchange with recursion_start_index and fix */
    memcpy_page(PAGELIST->auxbuf, addr);
    S(recursion_start_index).pagenum = (unsigned long)(PAGELIST->auxbuf) >> 12;
    memcpy_page(addr, address_of_target(sindex));
    PAGELIST->sources[index] = addr;
    j = index_within_sources(((unsigned long) addr) >> 12);
    if (j != IS_UNUSED)
      S(j).usedby = IS_PGLIST | index;
  } else
  if (S(sindex).usedby == recursion_start_index) {
    /* we have to stop recursion and need the aux-buffer to save the page */
    memcpy_page(PAGELIST->auxbuf, address_of_target(sindex));
    S(recursion_start_index).pagenum = (unsigned long)(PAGELIST->auxbuf) >>12;
  }
  else {
    if (S(sindex).usedby != IS_UNUSED) make_free(S(sindex).usedby); /* recursion */
  }
  memcpy_page(address_of_target(sindex), (void *)(S(sindex).pagenum << 12));
  S(sindex).usedby = IS_MOVED;
#ifdef VERBOSE
  printmove(sindex);
#endif
}


static void adjust(void)
{
  int i;
  if (PAGELIST == 0) return;
  build_references();
#ifdef STANDALONE_DEBUG
  for (i=0; i < PAGELIST->count; i++) {
    printf("%3d %p source=%x usedby=%d\n",i,address_of_target(i), S(i).pagenum << 12, S(i).usedby);
  }
#endif
  for (i=0; i < PAGELIST->count; i++) {
    recursion_start_index=i;
    make_free(i);
    if (S(i).usedby != IS_MOVED) {
      memcpy_page(address_of_target(i), (void *)(S(i).pagenum << 12));
      S(i).usedby = IS_MOVED;
#ifdef VERBOSE
      printmove(i);
#endif
    }
  }
}


#ifdef STANDALONE_DEBUG

static int read_pagelist(char *fname)
{
  FILE *f;
  long v;
  int i=0, bi=0, n;
  
  f=fopen(fname, "r");
  if (!f) exit(1);
  PAGELIST = malloc(0x10000);
  while (fscanf(f, "%li", &v) == 1) {
    if (v >= 4096) {
      PAGELIST->blocks[bi].taddr=v;
      PAGELIST->blocks[bi].start=i;
      if (bi>0) PAGELIST->blocks[bi-1].tcount=i-PAGELIST->blocks[bi-1].start;
      bi++;
      PAGELIST->number_of_blocks=bi;
    }
    else {
      S[i++].pagenum = v;
    }
  }
  PAGELIST->count=i;
  PAGELIST->blocks[bi-1].tcount=i-PAGELIST->blocks[bi-1].start;
  PAGELIST->auxbuf=(void *)0x10000000;
  fclose(f);
}

int main(int argc, char** argv)
{
  read_pagelist(argv[1]);
  if (PAGELIST) {
    printf("count %d nblocks %d\n", PAGELIST->count, PAGELIST->number_of_blocks);
    adjust();
  }
  return 0;
}

#else

unsigned long page_adjustment()
{
  int i= S(0).pagenum;
#if 0
  if (SCREEN_INFO.orig_video_mode == 7) {
    vidmem = (char *) 0xb0000;
    vidport = 0x3b4;
  } else {
    vidmem = (char *) 0xb8000;
    vidport = 0x3d4;
  }

  lines = SCREEN_INFO.orig_video_lines;
  cols = SCREEN_INFO.orig_video_cols;

#else
  /* setup does not use a fixed address for video modes any more */
  lines=25;
  cols=80;
  vidmem = (char *) 0xb8000;
  vidport = 0x3d4;
  SCREEN_INFO.orig_x = 0;
  SCREEN_INFO.orig_y = 2;
#endif

  /* The setup base may not be 0x95000 if that wasn't available */
  unsigned long pc;
  asm("call 1f\n\t"
      "1: pop %0\n\t": "=r" (pc));
  setup_base = pc & ~0xfff;

#if 0 /* this won;t work any more, because 2.1.22 changed the format
         of EXT_MEM_K, see STANDARD_MEMORY_BIOS_CALL in setup.S
       */
  if (EXT_MEM_K < (3*1024)) error("Less than 4MB of memory.\n");
#endif

  if (PAGELIST) {
#ifdef VERBOSE
    puts("Adjusting high loaded pages...");
#endif
    adjust();
#ifdef VERBOSE
    puts("done.\nNow starting decompressor...\n");
#endif
  }
#ifdef VERBOSE
  put_hex( REAL_STARTUP_32);
#endif
  return REAL_STARTUP_32;
}
#endif




