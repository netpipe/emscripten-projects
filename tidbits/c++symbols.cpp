> Hi,
>
> I am trying to write a shared library which exports only some of its
> functions.
>
> In windows this is easily done by creating a DLL and using
> __declspec(import) or (export).
>
> Any ideas how this is done in Linux? are there different solutions for a.out
> and ELF libraries?
>
> Thanks,
> Noam
>
>
>
You should use a version script:

slib1.h -------------------------------------------------

extern int slib_i;

int slib00();
int slib01();
int slib02();  

slib1.c --------------------------------------------------

#include <stdio.h>

int slib_i=5;

int slib00() { printf("slib00\n"); }
int slib01() { printf("slib01\n"); }
int slib02() { printf("slib02\n"); }

main.c -----------------------------------------------------

#include <stdio.h>

#include "slib1.h"

int main()
{
    int i = slib_i;
    slib00();
    slib01();
    slib02();
}                  

libslib1.ver ------------------------------------------------

#
# Export file for libslib1
#
libslib1.so.1.0 {
    global:
        slib_i;
        slib00;
        slib01;
        slib02;
    local:
        *;
};      

Makefile ---------------------------------------------------

main: main.c libslib1.so.1.0
        gcc -o main main.c -L. -lslib1

libslib1.so.1.0 : slib1.c libslib1.ver
        gcc -c -fPIC slib1.c
        gcc -shared -Wl,-soname,libslib1.so.1,-version-script=libslib1.ver -o libslib1.so.1.0 slib1.o
        ln -sf libslib1.so.1.0 libslib1.so.1
        ln -sf libslib1.so.1.0 libslib1.so

clean:
        rm -f *.o *.a *.so *.so.* *~ main

-------------------------------------------------------------

To demonstrate, try removing a symbol from the version script and rebuilding.
