@echo off
echo Testing basic Allegro 5...

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
if errorlevel 1 (
    echo ERROR: Could not set up Visual Studio environment
    exit /b 1
)

cl.exe test_allegro.c /I"packages\Allegro.5.2.10\build\native\include" "packages\Allegro.5.2.10\build\native\v142\win32\lib\allegro.lib" /Fe:test_allegro.exe

if errorlevel 1 (
    echo ERROR: Compilation failed
    pause
    exit /b 1
) else (
    echo SUCCESS: Test compiled
    echo Running test...
    test_allegro.exe
)

pause
