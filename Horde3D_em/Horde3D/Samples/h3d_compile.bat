@echo off
call \emsdk\emsdk_env.bat
PATH = %PATH%;\mingw\mingw32\bin
call emmake make -j4 -f %1

pause
