@echo off
REM Diablo 2 DS1 Editor - Package Release Script
REM Creates a release directory with all necessary files

echo ================================
echo  Diablo 2 DS1 Editor Package
echo ================================
echo.

REM Clean up old release
if exist "release" (
    echo Removing old release directory...
    rmdir /s /q "release"
)

REM Create release structure
echo Creating release directory structure...
mkdir "release"
mkdir "release\Data"
mkdir "release\Demo_data"
mkdir "release\ds1"
mkdir "release\pcx"
mkdir "release\docs"

REM Copy essential executable files
echo Copying executable and configuration...
copy "bin\ds1editor.exe" "release\" >nul
copy "bin\ds1edit.ini" "release\" >nul

REM Copy data directories
echo Copying data files...
xcopy "bin\Data" "release\Data" /e /i /q >nul 2>&1
xcopy "bin\Demo_data" "release\Demo_data" /e /i /q >nul 2>&1
xcopy "bin\ds1" "release\ds1" /e /i /q >nul 2>&1
xcopy "bin\pcx" "release\pcx" /e /i /q >nul 2>&1

REM Copy batch files
echo Copying launch scripts...
copy "bin\*.bat" "release\" >nul 2>&1

REM Copy documentation
echo Copying documentation...
copy "README FIRST.txt" "release\" >nul 2>&1
copy "README.md" "release\" >nul 2>&1

REM Copy docs folder selectively to avoid conflicts
xcopy "docs\*.html" "release\docs\" /q >nul 2>&1
xcopy "docs\*.zip" "release\docs\" /q >nul 2>&1

echo.
echo ================================
echo  Release Package Complete!
echo ================================
echo.
echo Release created in: release\
echo.
echo Package contents:
echo   ds1editor.exe     - Main executable
echo   ds1edit.ini       - Configuration file
echo   *.bat             - Launch scripts for sample maps
echo   Data\             - Core Diablo 2 data files
echo   Demo_data\        - Sample mod data
echo   ds1\              - Sample DS1 map files
echo   pcx\              - Interface graphics
echo   docs\             - Documentation and tutorials
echo   README files      - Installation and usage guides
echo.
echo To distribute: Zip the entire 'release' folder
echo.

if "%1" NEQ "nopause" pause
