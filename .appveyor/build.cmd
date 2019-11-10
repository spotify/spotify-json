@echo off

if "%PLATFORM%" == "" (
  echo Platform not defined!
  exit /b 1
)

mkdir build_%PLATFORM%
pushd build_%PLATFORM%

if "%PLATFORM%" == "Win32" (
  cmake -G "Visual Studio 16 2019" -A Win32 ..
) else if "%PLATFORM%" == "x64" (
  cmake -G "Visual Studio 16 2019" -A x64 ..
) else (
  echo Unknown platform!
  exit /b 2
)

popd
