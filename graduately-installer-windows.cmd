@echo off
rem Graduately installer for Windows.
rem
rem Carries no application of its own. Fastly caches branch-named URLs on
rem raw.githubusercontent.com, so this script looks up the tip of `builds`
rem and downloads that commit - not a stale zip from the last five minutes.
rem
rem Double-click the file to run it. The app updates itself from then on.

setlocal
set "REPO=pepaskopec-blip/Graduately"
set "BRANCH=builds"
set "ASSET=graduately-windows-x64.zip"
set "DEST=%LOCALAPPDATA%\Programs\Graduately"

echo Installing Graduately
echo ---------------------

set "CURL=%SystemRoot%\System32\curl.exe"
if not exist "%CURL%" set "CURL="
if not defined CURL for /f "delims=" %%I in ('where curl.exe 2^>nul') do if not defined CURL set "CURL=%%I"
if not defined CURL goto :no_tools

set "TAR=%SystemRoot%\System32\tar.exe"
if not exist "%TAR%" set "TAR="
if not defined TAR for /f "delims=" %%I in ('where tar.exe 2^>nul') do if not defined TAR set "TAR=%%I"
if not defined TAR goto :no_tools

set "TMP_DIR=%TEMP%\graduately-installer"
rmdir /s /q "%TMP_DIR%" 2>nul
mkdir "%TMP_DIR%" || goto :fail_tmp

echo Looking up the latest build...
set "SHA="
"%CURL%" -fsSL --max-time 20 -o "%TMP_DIR%\builds.atom" "https://github.com/%REPO%/commits/%BRANCH%.atom" 2>nul
if exist "%TMP_DIR%\builds.atom" (
  for /f "usebackq tokens=1,2 delims=/< " %%A in ("%TMP_DIR%\builds.atom") do (
    if "%%A"=="id>tag:github.com,2008:Grit::Commit" if not defined SHA set "SHA=%%B"
  )
)
if not defined SHA if exist "%TMP_DIR%\builds.atom" (
  for /f "usebackq delims=" %%S in (`powershell -NoProfile -ExecutionPolicy Bypass -Command "$m = [regex]::Match([IO.File]::ReadAllText(\"$env:TMP_DIR\builds.atom\"), '(?:Commit|/commit/)([0-9a-f]{40})', 'IgnoreCase'); if ($m.Success) { $m.Groups[1].Value }" 2^>nul`) do if not defined SHA set "SHA=%%S"
)

if defined SHA (
  set "REF=%SHA%"
  echo Downloading build %SHA:~0,7%...
) else (
  set "REF=refs/heads/%BRANCH%"
  echo Downloading latest build...
)
set "URL=https://raw.githubusercontent.com/%REPO%/%REF%/%ASSET%"
set "LEGACY_URL=https://raw.githubusercontent.com/%REPO%/%REF%/maturita-windows-x64.zip"

"%CURL%" -fL --retry 2 --progress-bar --max-time 1800 -o "%TMP_DIR%\%ASSET%" "%URL%"
if errorlevel 1 "%CURL%" -fL --retry 2 --progress-bar --max-time 1800 -o "%TMP_DIR%\%ASSET%" "%LEGACY_URL%" || goto :fail_download

echo Installing to %DEST%
if not exist "%DEST%" mkdir "%DEST%" || goto :fail_dest

taskkill /f /im graduately.exe >nul 2>&1
taskkill /f /im maturita.exe >nul 2>&1
rmdir /s /q "%DEST%\app" 2>nul
rmdir /s /q "%DEST%\runtime" 2>nul
del /f /q "%DEST%\graduately.exe" "%DEST%\maturita.exe" 2>nul

"%TAR%" -xf "%TMP_DIR%\%ASSET%" -C "%DEST%" 2>nul
if not exist "%DEST%\graduately.exe" (
  powershell -NoProfile -ExecutionPolicy Bypass -Command "Expand-Archive -LiteralPath \"$env:TMP_DIR\$env:ASSET\" -DestinationPath $env:DEST -Force" >nul 2>&1
)
if not exist "%DEST%\graduately.exe" goto :fail_extract

set "LNK=%APPDATA%\Microsoft\Windows\Start Menu\Programs\Graduately.lnk"
del /f /q "%APPDATA%\Microsoft\Windows\Start Menu\Programs\maturita.C.lnk" 2>nul
powershell -NoProfile -ExecutionPolicy Bypass -Command "$s=(New-Object -ComObject WScript.Shell).CreateShortcut($env:LNK);$s.TargetPath=(Join-Path $env:DEST 'graduately.exe');$s.WorkingDirectory=$env:DEST;$s.Save()" >nul 2>&1

rmdir /s /q "%TMP_DIR%" 2>nul

echo.
echo Done. Starting the app...
start "" "%DEST%\graduately.exe"
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
