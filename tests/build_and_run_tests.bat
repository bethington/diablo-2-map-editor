@echo off
echo Building DS1 Editor Test Suite...
echo.

REM Set up Visual Studio environment if not already set
if not defined VCINSTALLDIR (
    echo Setting up Visual Studio environment...
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
)

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

echo Compiling simple test...
cl.exe /EHsc /std:c++17 simple_test.cpp /Fe:build\ds1_test.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful! Running tests...
    echo.
    cd build
    ds1_test.exe
    cd ..
) else (
    echo.
    echo Build failed!
    exit /b 1
)

echo.
echo Test execution complete.
pause
