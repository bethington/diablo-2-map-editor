@echo off
REM Unity Test Runner for DS1 Editor Comprehensive Test Suite
REM Executes all Unity NUnit tests and generates detailed reports

echo ================================================================
echo DS1 Editor Unity Test Suite Runner
echo ================================================================
echo.

set "TEST_DIR=%~dp0"
set "PROJECT_FILE=%TEST_DIR%DS1EditorTests.csproj"
set "OUTPUT_DIR=%TEST_DIR%TestResults"
set "RELEASE_DIR=%TEST_DIR%..\release"

REM Create output directory
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"

echo Test Environment:
echo - Test Project: %PROJECT_FILE%
echo - Output Directory: %OUTPUT_DIR%
echo - Release Directory: %RELEASE_DIR%
echo.

REM Verify prerequisites
echo Checking prerequisites...

REM Check if .NET SDK is installed
dotnet --version >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo ERROR: .NET SDK not found. Please install .NET 6.0 SDK.
    echo Download from: https://dotnet.microsoft.com/download
    pause
    exit /b 1
)

echo ✓ .NET SDK found: 
dotnet --version

REM Check if DS1 editor exists
if not exist "%RELEASE_DIR%\ds1editor.exe" (
    echo ERROR: DS1 Editor executable not found at %RELEASE_DIR%\ds1editor.exe
    echo Please ensure the DS1 editor is built and available.
    pause
    exit /b 1
)

echo ✓ DS1 Editor executable found

REM Check if test DS1 files exist
if not exist "%RELEASE_DIR%\ds1" (
    echo WARNING: Test DS1 directory not found. Some tests may be skipped.
) else (
    echo ✓ Test DS1 files directory found
)

echo.
echo Prerequisites check completed successfully!
echo.

REM Ask user which tests to run
echo Select test execution mode:
echo 1. Run All Tests (Unit + Functional + Integration)
echo 2. Run Unit Tests Only
echo 3. Run Functional Tests Only
echo 4. Run Integration Tests Only
echo 5. Run with Detailed Output
echo 6. Run and Generate Report
echo.
set /p "choice=Enter your choice (1-6): "

REM Set test parameters based on choice
set "TEST_FILTER="
set "VERBOSITY=normal"
set "GENERATE_REPORT=false"

if "%choice%"=="2" set "TEST_FILTER=--filter ClassName=DS1EditorUnitTests"
if "%choice%"=="3" set "TEST_FILTER=--filter ClassName=DS1EditorFunctionalTests"
if "%choice%"=="4" set "TEST_FILTER=--filter ClassName=DS1EditorImageIntegrationTests"
if "%choice%"=="5" set "VERBOSITY=detailed"
if "%choice%"=="6" (
    set "VERBOSITY=detailed"
    set "GENERATE_REPORT=true"
)

echo.
echo ================================================================
echo Restoring NuGet Packages
echo ================================================================
echo.

dotnet restore "%PROJECT_FILE%"
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to restore NuGet packages
    pause
    exit /b 1
)

echo.
echo ================================================================
echo Building Test Project
echo ================================================================
echo.

dotnet build "%PROJECT_FILE%" --configuration Release
if %ERRORLEVEL% neq 0 (
    echo ERROR: Failed to build test project
    pause
    exit /b 1
)

echo.
echo ================================================================
echo Executing Tests
echo ================================================================
echo.

REM Run tests with specified parameters
set "TEST_COMMAND=dotnet test "%PROJECT_FILE%" --configuration Release --verbosity %VERBOSITY% %TEST_FILTER%"

if "%GENERATE_REPORT%"=="true" (
    set "TEST_COMMAND=%TEST_COMMAND% --logger "trx;LogFileName=DS1EditorTestResults.trx""
    set "TEST_COMMAND=%TEST_COMMAND% --results-directory "%OUTPUT_DIR%""
)

echo Executing: %TEST_COMMAND%
echo.

%TEST_COMMAND%

set "TEST_EXIT_CODE=%ERRORLEVEL%"

echo.
echo ================================================================
echo Test Execution Summary
echo ================================================================
echo.

if %TEST_EXIT_CODE% equ 0 (
    echo ✓ All tests completed successfully!
    echo Test Status: PASSED
) else (
    echo ✗ Some tests failed or encountered errors
    echo Test Status: FAILED
    echo Exit Code: %TEST_EXIT_CODE%
)

echo.

REM Generate additional reports if requested
if "%GENERATE_REPORT%"=="true" (
    echo ================================================================
    echo Generating Test Reports
    echo ================================================================
    echo.
    
    if exist "%OUTPUT_DIR%\DS1EditorTestResults.trx" (
        echo ✓ Test results file generated: %OUTPUT_DIR%\DS1EditorTestResults.trx
        echo   Open in Visual Studio for detailed analysis
    ) else (
        echo ✗ Test results file not generated
    )
    
    REM Create summary report
    echo DS1 Editor Test Execution Summary > "%OUTPUT_DIR%\TestSummary.txt"
    echo ====================================== >> "%OUTPUT_DIR%\TestSummary.txt"
    echo Test Date: %date% %time% >> "%OUTPUT_DIR%\TestSummary.txt"
    echo Test Project: %PROJECT_FILE% >> "%OUTPUT_DIR%\TestSummary.txt"
    echo Test Filter: %TEST_FILTER% >> "%OUTPUT_DIR%\TestSummary.txt"
    echo Exit Code: %TEST_EXIT_CODE% >> "%OUTPUT_DIR%\TestSummary.txt"
    echo Status: %STATUS% >> "%OUTPUT_DIR%\TestSummary.txt"
    echo. >> "%OUTPUT_DIR%\TestSummary.txt"
    
    echo ✓ Summary report generated: %OUTPUT_DIR%\TestSummary.txt
)

echo.
echo ================================================================
echo Manual Testing Instructions
echo ================================================================
echo.
echo Some DS1 Editor functionality requires manual testing:
echo.
echo 1. Screenshot Generation:
echo    - Run: %RELEASE_DIR%\ds1editor.exe Duriel.ds1 17 481
echo    - Press P to generate screenshots
echo    - Press Shift+P for full map screenshots
echo    - Verify screenshot-*.pcx files are created
echo.
echo 2. Floor Tile PNG Export:
echo    - Load DS1 file with varied floor patterns
echo    - Use F1/F2 to show only floor layers
echo    - Zoom in with + key to see tile details
echo    - Press P to capture floor tiles
echo    - Convert PCX to PNG: convert screenshot-*.pcx floor-tiles.png
echo.
echo 3. Map as PNG Export:
echo    - Load any DS1 file
echo    - Press Shift+P to export full map
echo    - Convert to PNG: convert screenshot-*.pcx fullmap.png
echo    - Verify entire map area is captured
echo.
echo 4. Layer Isolation Tests:
echo    - Use F1-F10 to toggle different layers
echo    - Take screenshots with various layer combinations
echo    - Convert to PNG for analysis
echo.
echo 5. Palette Variation Tests:
echo    - Test with -force_pal parameter (1-5)
echo    - Compare color differences between acts
echo    - Document palette effects
echo.

echo ================================================================
echo Test Artifacts Location
echo ================================================================
echo.
echo Generated files can be found in:
echo - Test Results: %OUTPUT_DIR%
echo - Screenshots: %RELEASE_DIR%\screenshot-*.pcx
echo - Debug Files: %RELEASE_DIR%\Debug\
echo - Logs: Test output above
echo.

if "%GENERATE_REPORT%"=="true" (
    echo Opening test results directory...
    explorer "%OUTPUT_DIR%"
)

echo.
echo Test execution completed!
echo Total exit code: %TEST_EXIT_CODE%

pause
exit /b %TEST_EXIT_CODE%
