@echo off

if "%PLATFORM%" == "" (
  echo Platform not defined!
  exit /b 1
)

if "%PLATFORM%" == "Win32" (
  set BITS=32
) else if "%PLATFORM%" == "x64" (
  set BITS=64
) else (
  echo Unknown platform!
  exit /b 2
)
