@echo off
setlocal

REM Launch a locally built maturita.exe with the GTK4 runtime on PATH, so the
REM app can be started by double-clicking even when GTK is not installed
REM system-wide.

if not defined GTK_ROOT set "GTK_ROOT=C:\gtk"
set "PATH=%GTK_ROOT%\bin;%PATH%"

if not exist "%~dp0maturita.exe" (
    echo [error] maturita.exe not found next to this script.
    echo         Build it first with build-windows.bat.
    exit /b 1
)

start "" /D "%~dp0" "%~dp0maturita.exe"
