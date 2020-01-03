#include <stdio.h>
#include <limits.h>

int main(void)
{
  short myshort;
  int myint;
  long mylong;
  long long mylonglong;
printf("%X\n", 0xAABBCCDD);
  printf("variable sizes:\n");
  printf("short: %d\n", sizeof(short));
  printf("int: %d\n", sizeof(int));
  printf("long: %d\n", sizeof(long));
  printf("long long: %d\n", sizeof(long long));

  return 0;
}

