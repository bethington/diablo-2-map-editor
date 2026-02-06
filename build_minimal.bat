@echo off
echo Building DS1 Editor - Minimal Approach...

REM Set up Visual Studio 2019 environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
if errorlevel 1 (
    echo ERROR: Could not set up Visual Studio environment
    exit /b 1
)

echo Compiling source files...

REM Core source files (excluding problematic ones for now)
set SOURCE_FILES=^
    allegro_stubs.c ^
    main.c ^
    mpq\Dcl_tbl.c ^
    mpq\Explode.c ^
    mpq\MpqView.c ^
    mpq\Wav_unp.c

REM Compile each source file
for %%f in (%SOURCE_FILES%) do (
    echo Compiling %%f...
    cl.exe /c "%%f" /Fo:obj\ /DWIN32 /D_WINDOWS /DALLEGRO_STATICLINK ^
        /I"packages\Allegro.5.2.10\build\native\include" ^
        /I"packages\AllegroDeps.1.15.0\build\native\include"
    if errorlevel 1 (
        echo ERROR: Failed to compile %%f
        pause
        exit /b 1
    )
)

echo Linking executable...
link.exe obj\*.obj ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_acodec-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_audio-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_color-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_dialog-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_font-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_image-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_main-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_memfile-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_physfs-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_primitives-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_ttf-5.2.lib" ^
    "packages\Allegro.5.2.10\build\native\lib\Win32\allegro_video-5.2.lib" ^
    user32.lib gdi32.lib winmm.lib ole32.lib shell32.lib ^
    /OUT:release\ds1editor_minimal.exe /SUBSYSTEM:CONSOLE

if errorlevel 1 (
    echo ERROR: Linking failed
    pause
    exit /b 1
) else (
    echo SUCCESS: Minimal build complete - ds1editor_minimal.exe created
)

pause
