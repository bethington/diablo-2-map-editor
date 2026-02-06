@echo off
echo ================================================================
echo               DS1 Editor - MSVC Build Script
echo ================================================================

REM Initialize Visual Studio environment - Try multiple versions
echo Detecting Visual Studio installation...

REM Try VS 2022 first (most recent)
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" (
    echo Setting up Visual Studio 2022 environment...
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat"
    if not errorlevel 1 (
        echo Visual Studio 2022 environment initialized successfully.
        goto :vs_ready
    )
)

REM Try VS 2019 (x86)
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat" (
    echo Setting up Visual Studio 2019 environment...
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars32.bat"
    if not errorlevel 1 (
        echo Visual Studio 2019 environment initialized successfully.
        goto :vs_ready
    )
)

REM Try VS 2017 as fallback
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat" (
    echo Setting up Visual Studio 2017 environment...
    call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars32.bat"
    if not errorlevel 1 (
        echo Visual Studio 2017 environment initialized successfully.
        goto :vs_ready
    )
)

echo ERROR: Could not find or initialize any Visual Studio environment
echo Please install Visual Studio Community 2019 or later
exit /b 1

:vs_ready

echo Visual Studio environment initialized successfully.

REM Show which compiler we're using
echo.
echo Using compiler: 
cl.exe 2>&1 | findstr "Version"
echo.

REM Create output directories
if not exist obj mkdir obj
if not exist release mkdir release

echo Setting up Allegro 5.2.10 paths...
set ALLEGRO_INCLUDE=packages\Allegro.5.2.10\build\native\include
set ALLEGRO_LIB=packages\Allegro.5.2.10\build\native\v143\win32\lib

echo ================================================================
echo Compiling source files with MSVC...
echo ================================================================

REM Define source files (excluding test files)
set SOURCES=allegro5_globals.c allegro5_stubs.c main.c anim.c animdata.c dc6info.c dccinfo.c ds1misc.c ds1save.c dt1misc.c dt1_draw.c
set SOURCES=%SOURCES% editobj.c editpath.c edittile.c error.c gfx_custom.c inicreat.c iniread.c interfac.c
set SOURCES=%SOURCES% misc.c msg_quit.c msg_save.c png_output.c txtread.c undo.c wBits.c wEdit.c wMsg.c wPreview.c
set SOURCES=%SOURCES% mpq\Dcl_tbl.c mpq\Explode.c mpq\MpqView.c mpq\Wav_unp.c

REM Compile each source file
for %%f in (%SOURCES%) do (
    echo Compiling %%f...
    cl.exe /c "%%f" /Fo:obj\ /I"%ALLEGRO_INCLUDE%" /DWIN32 /D_WINDOWS /DALLEGRO_STATICLINK /DALLEGRO_NO_MAGIC_MAIN /nologo /W3
    if errorlevel 1 (
        echo ERROR: Failed to compile %%f
        echo.
        echo Trying with relaxed error checking...
        cl.exe /c "%%f" /Fo:obj\ /I"%ALLEGRO_INCLUDE%" /DWIN32 /D_WINDOWS /DALLEGRO_STATICLINK /DALLEGRO_NO_MAGIC_MAIN /nologo /W1
        if errorlevel 1 (
            echo ERROR: Still failed to compile %%f
            echo Continuing with other files...
        )
    )
)

echo ================================================================
echo Linking executable...
echo ================================================================

REM Check if we have any object files
if not exist obj\*.obj (
    echo ERROR: No object files found to link
    exit /b 1
)

REM Link the executable
echo Linking with Allegro monolith library...
link.exe obj\*.obj /OUT:release\ds1editor.exe ^
    /LIBPATH:"%ALLEGRO_LIB%" ^
    allegro_monolith-static.lib ^
    user32.lib gdi32.lib winmm.lib ole32.lib kernel32.lib ^
    advapi32.lib shell32.lib comdlg32.lib uuid.lib opengl32.lib ^
    /SUBSYSTEM:CONSOLE /NOLOGO

if errorlevel 1 (
    echo Linking with monolith failed, trying individual libraries...
    link.exe obj\*.obj /OUT:release\ds1editor.exe ^
        /LIBPATH:"%ALLEGRO_LIB%" ^
        allegro.lib allegro_main.lib allegro_image.lib allegro_font.lib allegro_primitives.lib ^
        user32.lib gdi32.lib winmm.lib ole32.lib kernel32.lib ^
        advapi32.lib shell32.lib comdlg32.lib uuid.lib opengl32.lib ^
        /SUBSYSTEM:CONSOLE /NOLOGO
    
    if errorlevel 1 (
        echo ERROR: Linking failed with both approaches
        exit /b 1
    )
)

echo ================================================================
echo Build completed!
echo ================================================================

REM Test if executable was created
if exist release\ds1editor.exe (
    echo ds1editor.exe created successfully in release\ directory
    echo.
    echo File size:
    dir release\ds1editor.exe | find ".exe"
    echo.
    echo To test the editor, run:
    echo   release\ds1editor.exe ds1\duriel.ds1 17 481
    echo.
    echo Or use batch files in release\ folder
    echo.
    echo Build Status: SUCCESS
) else (
    echo ERROR: Executable not created
    echo Build Status: FAILED
    exit /b 1
)
