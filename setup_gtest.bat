@echo off
echo ================================================================
echo            DS1 Editor - Google Test Setup Script
echo ================================================================

REM Initialize Visual Studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" 2>nul
if errorlevel 1 (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" 2>nul
    if errorlevel 1 (
        echo ERROR: Could not initialize Visual Studio environment
        pause
        exit /b 1
    )
)

echo Setting up Google Test framework for DS1 Editor...
echo.

REM Create test directories
echo Creating test directory structure...
if not exist tests\cpp mkdir tests\cpp
if not exist tests\cpp\unit mkdir tests\cpp\unit
if not exist tests\cpp\integration mkdir tests\cpp\integration
if not exist tests\cpp\mocks mkdir tests\cpp\mocks
if not exist tests\cpp\bin mkdir tests\cpp\bin

REM Check if vcpkg is available
echo Checking for vcpkg...
where vcpkg >nul 2>&1
if errorlevel 1 (
    echo vcpkg not found in PATH. You'll need to install Google Test manually.
    echo Download from: https://github.com/google/googletest
    echo Or install vcpkg and run: vcpkg install gtest:x86-windows
) else (
    echo Found vcpkg, installing Google Test...
    vcpkg install gtest:x86-windows
    vcpkg install gmock:x86-windows
)

echo.
echo Google Test setup complete!
echo.
echo Next steps:
echo 1. Create unit tests in tests\cpp\unit\
echo 2. Build tests using build_tests.bat
echo 3. Run tests using run_tests.bat
echo.
pause
