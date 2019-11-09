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

nuget install boost-vc140 -version 1.70.0 -outputdirectory boost_1_70_0 -verbosity quiet

pushd boost_1_70_0

mklink /d include boost.1.70.0\lib\native\include

mkdir lib

for /d %%i in (boost_*) do (
  for %%j in (%%~fi\lib\native\address-model-%BITS%\lib\*) do (
    mklink lib\%%~nxj %%~fj
  )
)

popd
