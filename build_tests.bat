@echo off
echo ================================================================
echo               DS1 Editor - Build Tests Script  
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

REM Create test build directory
if not exist tests\cpp\bin mkdir tests\cpp\bin

echo Building Google Test framework tests...
echo.

REM Set up paths
set GTEST_ROOT=C:\vcpkg\installed\x86-windows
set GTEST_INCLUDE=%GTEST_ROOT%\include
set GTEST_LIB=%GTEST_ROOT%\lib

REM Alternative: Use NuGet package paths if vcpkg not available
if not exist "%GTEST_INCLUDE%" (
    echo vcpkg Google Test not found, checking for NuGet packages...
    set GTEST_INCLUDE=packages\googletest\build\native\include
    set GTEST_LIB=packages\googletest\build\native\lib\Win32\v143\Release
)

echo Google Test Include: %GTEST_INCLUDE%
echo Google Test Lib: %GTEST_LIB%
echo.

REM Compile test files
echo Compiling unit tests...

REM Test source files
set TEST_SOURCES=tests\cpp\unit\test_ds1_misc.cpp

REM Project source files needed for testing (subset to avoid linking issues)
set PROJECT_SOURCES=ds1misc.c misc.c error.c

echo Compiling test executable...
cl.exe /EHsc ^
    %TEST_SOURCES% %PROJECT_SOURCES% ^
    /I"%GTEST_INCLUDE%" ^
    /I"packages\Allegro.5.2.10\build\native\include" ^
    /DWIN32 /D_WINDOWS /DALLEGRO_STATICLINK /DALLEGRO_NO_MAGIC_MAIN ^
    /Fe:tests\cpp\bin\ds1_tests.exe ^
    /link ^
    /LIBPATH:"%GTEST_LIB%" ^
    /LIBPATH:"packages\Allegro.5.2.10\build\native\v143\win32\lib" ^
    gtest.lib gtest_main.lib gmock.lib ^
    allegro_monolith-static.lib ^
    user32.lib gdi32.lib winmm.lib ole32.lib kernel32.lib ^
    /SUBSYSTEM:CONSOLE

if errorlevel 1 (
    echo.
    echo Build failed. Trying with simpler approach...
    echo.
    
    REM Try building just a simple test runner
    cl.exe /EHsc ^
        tests\cpp\unit\test_ds1_misc.cpp ^
        /I"%GTEST_INCLUDE%" ^
        /DGTEST_SIMPLE_BUILD ^
        /Fe:tests\cpp\bin\simple_tests.exe ^
        /link ^
        /LIBPATH:"%GTEST_LIB%" ^
        gtest.lib gtest_main.lib ^
        /SUBSYSTEM:CONSOLE
        
    if errorlevel 1 (
        echo ERROR: Could not build tests
        echo.
        echo Please ensure Google Test is properly installed:
        echo 1. Install vcpkg: https://github.com/Microsoft/vcpkg
        echo 2. Run: vcpkg install gtest:x86-windows
        echo 3. Run: vcpkg integrate install
        pause
        exit /b 1
    ) else (
        echo Simple test build succeeded!
    )
) else (
    echo Full test build succeeded!
)

echo.
echo Test build complete!
echo Run tests with: run_tests.bat
echo.
pause
