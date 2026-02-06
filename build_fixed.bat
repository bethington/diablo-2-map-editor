@echo off
echo Building DS1 Editor with Fixed Build Environment...
echo.

REM Set up Visual Studio environment
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"

if %ERRORLEVEL% NEQ 0 (
    echo Failed to set up Visual Studio environment
    pause
    exit /b 1
)

REM Create build directories
if not exist "build\obj" mkdir "build\obj"
if not exist "release" mkdir "release"

echo.
echo Compiling source files...
echo.

REM List of source files to compile
set SOURCE_FILES=allegro_stubs.c main.c ds1misc.c ds1save.c editobj.c editpath.c edittile.c ^
interfac.c anim.c animdata.c dc6info.c dccinfo.c dt1misc.c dt1_draw.c ^
error.c gfx_custom.c inicreat.c iniread.c misc.c msg_quit.c msg_save.c ^
txtread.c undo.c wBits.c wEdit.c wMsg.c wPreview.c ^
mpq\Dcl_tbl.c mpq\Explode.c mpq\MpqView.c mpq\Wav_unp.c

REM Compile each source file
for %%f in (%SOURCE_FILES%) do (
    echo Compiling %%f...
    cl.exe /c "%%f" /Fo:build\obj\ /DWIN32 /D_WINDOWS /DALLEGRO_STATICLINK /I"." /I"packages\Allegro.5.2.10\build\native\include" /EHsc
    if %ERRORLEVEL% NEQ 0 (
        echo Failed to compile %%f
        pause
        exit /b 1
    )
)

echo.
echo Linking executable...
echo.

REM Link the executable
link.exe build\obj\*.obj ^
    "packages\Allegro.5.2.10\build\native\v142\win32\lib\allegro.lib" ^
    "packages\Allegro.5.2.10\build\native\v142\win32\lib\allegro_primitives.lib" ^
    "packages\Allegro.5.2.10\build\native\v142\win32\lib\allegro_image.lib" ^
    "packages\Allegro.5.2.10\build\native\v142\win32\lib\allegro_font.lib" ^
    "packages\Allegro.5.2.10\build\native\v142\win32\lib\allegro_main.lib" ^
    user32.lib gdi32.lib winmm.lib ole32.lib kernel32.lib ^
    /OUT:release\ds1editor.exe /SUBSYSTEM:CONSOLE

if %ERRORLEVEL% EQU 0 (
    echo.
    echo Build successful! ds1editor.exe created in release\ folder.
) else (
    echo.
    echo Linking failed. Check the errors above.
    echo.
    echo Common issues:
    echo 1. Missing Allegro libraries - check packages folder
    echo 2. Unresolved external symbols - missing function implementations
    echo 3. Compatibility layer issues - check allegro5_compat.h
)

echo.
pause
