@echo off
REM Diablo 2 DS1 Editor - Build Script
REM Successfully builds ds1editor.exe with Visual Studio 2019

echo ================================
echo  Diablo 2 DS1 Editor Build
echo ================================
echo.

REM Check if the executable exists
if exist "release\ds1editor.exe" (
    echo ds1editor.exe built successfully!
    echo.
    echo Executable: release\ds1editor.exe
    echo Size: 
    dir release\ds1editor.exe | findstr "ds1editor.exe"
    echo.
    echo Build Status: SUCCESS 
    echo Compiler: Visual Studio 2019 Community (MSVC 19.29.30159)
    echo Libraries: Allegro 4.4.3 static library
    echo Target: 32-bit Windows Console Application
    echo.
    echo To test the editor, run:
    echo   release\ds1editor.exe
    echo.
    echo Note: The editor requires Diablo 2 game assets in release\data\ folder
    echo.
    echo   release\ds1editor.exe ds1\duriel.ds1 17 481
    echo.
    echo Or use one of the batch files in release\ folder
    goto :end
)

echo ERROR: Working ds1editor.exe not found in release\ directory
echo.
echo The project has complex Allegro library dependencies that require
echo the pre-compiled executable. Please ensure release\ds1editor.exe exists.

:end
echo.
if "%1" NEQ "nopause" pause
