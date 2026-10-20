@echo off
rem Compila il gamemode su Windows. Richiede pawncc.exe (open.mp compiler 3.10.10)
rem in toolchain\pawncc\ oppure nel PATH.
setlocal
set ROOT=%~dp0
set PAWNCC=pawncc
if exist "%ROOT%toolchain\pawncc\pawncc.exe" set PAWNCC="%ROOT%toolchain\pawncc\pawncc.exe"
cd /d "%ROOT%gamemodes"
%PAWNCC% lsrp.pwn -d3 -i"%ROOT%toolchain\include\omp-stdlib" -i"%ROOT%toolchain\include\mysql" -i"%ROOT%toolchain\include\cef" -i"%ROOT%toolchain\include\bcrypt" -i"%ROOT%toolchain\include\sampvoice" -i. -o"%ROOT%gamemodes\lsrp.amx"
%PAWNCC% lsrp.pwn -d3 LSRP_NO_CEF=1 -i"%ROOT%toolchain\include\omp-stdlib" -i"%ROOT%toolchain\include\mysql" -i"%ROOT%toolchain\include\cef" -i"%ROOT%toolchain\include\bcrypt" -i"%ROOT%toolchain\include\sampvoice" -i. -o"%ROOT%gamemodes\lsrp_senza_cef.amx"
%PAWNCC% lsrp.pwn -d3 LSRP_VOICE=1 -i"%ROOT%toolchain\include\omp-stdlib" -i"%ROOT%toolchain\include\mysql" -i"%ROOT%toolchain\include\cef" -i"%ROOT%toolchain\include\bcrypt" -i"%ROOT%toolchain\include\sampvoice" -i. -o"%ROOT%gamemodes\lsrp_voce.amx"
pause
