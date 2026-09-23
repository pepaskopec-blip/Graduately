@echo off
setlocal EnableExtensions

REM ---------------------------------------------------------------------------
REM Native Windows build with MSVC. No MSYS2 or MinGW shell required.
REM
REM Requirements:
REM   * Visual Studio 2019/2022 with the "Desktop development with C++"
REM     workload installed.
REM   * A GTK4 build for MSVC, for example the gvsbuild prebuilt package
REM     unzipped to C:\gtk (see README.md).
REM
REM If GTK lives somewhere other than C:\gtk, set GTK_ROOT first, e.g.
REM     set GTK_ROOT=D:\gtk
REM ---------------------------------------------------------------------------

if not defined GTK_ROOT set "GTK_ROOT=C:\gtk"

REM --- 1. Locate Visual Studio and enter the 64-bit MSVC environment --------
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
if not exist "%VSWHERE%" (
    echo [error] vswhere.exe not found.
    echo         Install Visual Studio with the "Desktop development with C++"
    echo         workload, then run this script again.
    exit /b 1
)

set "VSPATH="
for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSPATH=%%i"
if not defined VSPATH (
    echo [error] No Visual Studio C++ installation found.
    exit /b 1
)

call "%VSPATH%\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
    echo [error] Failed to initialise the MSVC environment.
    exit /b 1
)

REM --- 2. Point the build at the GTK4 installation --------------------------
set "PATH=%GTK_ROOT%\bin;%PATH%"
set "PKG_CONFIG_PATH=%GTK_ROOT%\lib\pkgconfig;%PKG_CONFIG_PATH%"

set "PKG_CONFIG=pkg-config"
where pkg-config >nul 2>nul
if errorlevel 1 (
    where pkgconf >nul 2>nul
    if errorlevel 1 (
        echo [error] pkg-config/pkgconf was not found.
        echo         Make sure GTK_ROOT points at a GTK4 install that contains
        echo         bin\pkg-config.exe. Current GTK_ROOT: %GTK_ROOT%
        exit /b 1
    )
    set "PKG_CONFIG=pkgconf"
)

REM --- 3. Collect the GTK4 flags in MSVC syntax -----------------------------
set "GTK_CFLAGS="
set "GTK_LIBS="
for /f "usebackq delims=" %%c in (`%PKG_CONFIG% --cflags gtk4 --msvc-syntax`) do set "GTK_CFLAGS=%%c"
for /f "usebackq delims=" %%l in (`%PKG_CONFIG% --libs gtk4 --msvc-syntax`) do set "GTK_LIBS=%%l"

if not defined GTK_CFLAGS (
    echo [error] pkg-config could not find gtk4.
    echo         Check that "%PKG_CONFIG_PATH%" contains gtk4.pc.
    exit /b 1
)

if not exist "assets\app-icon.ico" (
    echo [error] assets\app-icon.ico is missing.
    echo         Generate it with: python scripts\gen-icons.py
    exit /b 1
)

REM --- 4. Compile resources + sources ---------------------------------------
echo Building graduately.exe with MSVC...
if not exist build mkdir build
rc /nologo /fo build\maturita.res data\windows\maturita.rc
if errorlevel 1 (
    echo [error] Resource compile failed.
    exit /b 1
)

cl /nologo /O2 /MD /W3 /std:c11 /I src /D_CRT_SECURE_NO_WARNINGS ^
   %GTK_CFLAGS% src\*.c build\maturita.res ^
   /Fe:graduately.exe /Fo:build\ ^
   /link /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup %GTK_LIBS%

if errorlevel 1 (
    echo [error] Build failed.
    exit /b 1
)

if not exist icons\hicolor\512x512\apps mkdir icons\hicolor\512x512\apps
copy /Y assets\app-icon.png icons\hicolor\512x512\apps\maturita.png >nul

echo.
echo Built graduately.exe ^(icon embedded from assets\app-icon.ico^)
echo Run it from this terminal with:  graduately.exe
echo Or use run-windows.bat / double-click graduately.exe.
