rem  Sample DOS batch file to boot Linux.

rem  First, ensure any unwritten disk buffers are flushed:
smartdrv /C

rem  Start the LOADLIN process:
c:\loadlin\loadlin  c:\loadlin\zimage  root=/dev/hdb2  ro  vga=3
