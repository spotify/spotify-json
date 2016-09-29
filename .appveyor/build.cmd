@echo off

if "%PLATFORM%" == "" (
  echo Platform not defined!
  exit /b 1
)

mkdir build_%PLATFORM%
pushd build_%PLATFORM%

set BOOST_ROOT=%APPVEYOR_BUILD_FOLDER%\boost_1_61_0

echo Boost location is '%BOOST_ROOT%'

if "%PLATFORM%" == "Win32" (
  cmake -G "Visual Studio 14 2015" ..
) else if "%PLATFORM%" == "x64" (
  cmake -G "Visual Studio 14 2015 Win64" ..
) else (
  echo Unknown platform!
  exit /b 2
)

popd
