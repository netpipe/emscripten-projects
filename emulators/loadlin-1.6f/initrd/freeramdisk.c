#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <errno.h>

main(int argc, char **argv)
{
  char rname[256]="/dev/ram";
  char b[256];
  int f;
  if (argv[1]) strcpy(rname,argv[1]);

#if 0  /* don't use on a living system, /etc/mtab is not updated */ 
  if (umount(rname) && (errno != ENOENT)) {
    sprintf(b,"freeramdisk: cannot umount %s %d",rname,errno);
    perror(b);
    exit(1);
  }
#endif

  if ((f=open(rname,O_RDWR)) == -1) {
    sprintf(b,"freeramdisk: cannot open %s",rname);
    perror(b);
    exit(1);
  }
  ioctl(f,BLKFLSBUF);
  close(f);
}
