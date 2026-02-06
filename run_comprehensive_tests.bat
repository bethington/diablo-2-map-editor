@echo off
echo DS1 Editor Test Suite
echo =====================

REM Set the working directory to the project root
cd /d "%~dp0"

REM Check if tests directory exists
if not exist "tests" (
    echo Error: Tests directory not found
    pause
    exit /b 1
)

REM Create build directory for tests
if not exist "tests\build" mkdir "tests\build"
cd tests\build

echo.
echo Building tests with CMake...
echo.

REM Configure with CMake
cmake .. -G "Visual Studio 16 2019" -A Win32
if errorlevel 1 (
    echo Error: CMake configuration failed
    pause
    exit /b 1
)

REM Build the tests
cmake --build . --config Release
if errorlevel 1 (
    echo Error: Build failed
    pause
    exit /b 1
)

echo.
echo Running DS1 Editor Test Suite...
echo.

REM Run basic functionality tests
echo === Basic Functionality Tests ===
Release\ds1editor_test.exe
echo.

REM Run keyboard functionality tests
echo === Keyboard Functionality Tests ===
Release\ds1editor_keyboard_test.exe
echo.

REM Run file integrity tests
echo === File Integrity Tests ===
Release\ds1editor_file_test.exe
echo.

echo.
echo Test suite completed.
echo Check output above for any failures.
echo.
pause
