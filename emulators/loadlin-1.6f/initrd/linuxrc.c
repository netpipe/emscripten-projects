#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mount.h>
#include <linux/fs.h>


void mount_proc(void)
{
  /* mkdir("/proc",0744); */
  mount(0,"/proc","proc",0,0);  
}


main(int argc, char **argv, char **env)
{
  char **a=argv, **e=env;
  char *real_root;
  FILE *f;
  fputs("\n\n\n\nHello, here ist /linuxrc printing\n",stdout);
  fputs("This is our argv[]:\n",stdout);
  while (*a) printf(">%s<\n",*a++);
  fputs("This is our env[]:\n",stdout);
  while (*e) printf(">%s<\n",*e++);
  real_root = getenv("real_root");
  if (real_root) {
    printf("we are changing root to device %s\n",real_root);
    mount_proc();
    if (! (f=fopen("/proc/sys/kernel/real-root-dev","w"))) return 0;
    fprintf(f, "%s\n",real_root);
    fclose(f);
  }
  fputs("\n\n",stdout);
  return 0;
}
