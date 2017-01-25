@echo off

if "%PLATFORM%" == "" (
  echo Platform not defined!
  exit /b 1
)

if "%CONFIGURATION%" == "" (
  echo Configuration not defined!
  exit /b 2
)

build_%PLATFORM%\test\%CONFIGURATION%\spotify_json_test.exe
