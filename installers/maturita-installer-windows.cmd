@echo off
rem maturita.c installer for Windows.
rem
rem Carries no application of its own: it downloads the current build straight
rem from the repository's `builds` branch, which CI rewrites whenever main
rem changes. This file therefore never goes stale.
rem
rem Double-click the file to run it. The app updates itself from then on.

setlocal
set "REPO=pepaskopec-blip/maturita.c"
set "BRANCH=builds"
set "ASSET=maturita-windows-x64.zip"
set "URL=https://raw.githubusercontent.com/%REPO%/%BRANCH%/%ASSET%"
set "DEST=%LOCALAPPDATA%\Programs\Maturita"

echo Installing maturita.C
echo ---------------------

rem curl and tar both ship with Windows 10 1803 and later.
where curl >nul 2>&1 || goto :no_tools
where tar  >nul 2>&1 || goto :no_tools

set "TMP_DIR=%TEMP%\maturita-installer"
rmdir /s /q "%TMP_DIR%" 2>nul
mkdir "%TMP_DIR%" || goto :fail_tmp

echo Downloading the latest release...
curl -fL --progress-bar -o "%TMP_DIR%\%ASSET%" "%URL%" || goto :fail_download

echo Installing to %DEST%
if not exist "%DEST%" mkdir "%DEST%" || goto :fail_dest

rem Extracted over any existing install rather than replacing the folder, so
rem the progress files kept inside it survive an upgrade.
tar -xf "%TMP_DIR%\%ASSET%" -C "%DEST%" || goto :fail_extract

rem Start menu entry, so the app can be found without digging for the folder.
set "LNK=%APPDATA%\Microsoft\Windows\Start Menu\Programs\maturita.C.lnk"
powershell -NoProfile -ExecutionPolicy Bypass -Command ^
  "$s=(New-Object -ComObject WScript.Shell).CreateShortcut('%LNK%');" ^
  "$s.TargetPath='%DEST%\maturita.exe';$s.WorkingDirectory='%DEST%';$s.Save()" >nul 2>&1

rmdir /s /q "%TMP_DIR%" 2>nul

echo.
echo Done. Starting the app...
start "" "%DEST%\maturita.exe"
exit /b 0

:no_tools
echo.
echo Error: this installer needs curl and tar, which come with Windows 10
echo version 1803 and later. Please update Windows, or download the zip
echo manually from https://github.com/%REPO%/tree/%BRANCH%
pause
exit /b 1

:fail_tmp
echo.
echo Error: could not create a temporary folder in %TEMP%.
pause
exit /b 1

:fail_download
echo.
echo Error: the download failed. Check your internet connection.
pause
exit /b 1

:fail_dest
echo.
echo Error: could not create %DEST%.
pause
exit /b 1

:fail_extract
echo.
echo Error: the archive could not be unpacked.
pause
exit /b 1
