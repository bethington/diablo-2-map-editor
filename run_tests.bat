@echo off
echo ================================================================
echo                DS1 Editor - Run Tests Script
echo ================================================================

REM Check if test executable exists
if exist "tests\cpp\bin\ds1_tests.exe" (
    echo Running full test suite...
    echo.
    
    REM Change to test directory for relative paths
    cd tests\cpp\bin
    
    REM Run tests with detailed output
    ds1_tests.exe --gtest_output=xml:test_results.xml --gtest_color=yes
    
    set TEST_RESULT=%ERRORLEVEL%
    
    REM Return to project root
    cd ..\..\..
    
    echo.
    if %TEST_RESULT% EQU 0 (
        echo ========================================
        echo           ALL TESTS PASSED!
        echo ========================================
    ) else (
        echo ========================================
        echo         SOME TESTS FAILED!
        echo ========================================
        echo Check tests\cpp\bin\test_results.xml for details
    )
    
) else if exist "tests\cpp\bin\simple_tests.exe" (
    echo Running simple test suite...
    echo.
    
    cd tests\cpp\bin
    simple_tests.exe
    set TEST_RESULT=%ERRORLEVEL%
    cd ..\..\..
    
    echo.
    if %TEST_RESULT% EQU 0 (
        echo Simple tests passed!
    ) else (
        echo Simple tests failed!
    )
    
) else (
    echo ERROR: No test executable found!
    echo Please run build_tests.bat first
    echo.
    pause
    exit /b 1
)

echo.
echo Running .NET integration tests...
echo.

REM Run existing NUnit tests
if exist "tests\DS1EditorTests.csproj" (
    cd tests
    dotnet test --logger:console --verbosity:normal
    cd ..
) else (
    echo .NET tests not found - skipping
)

echo.
echo ================================================================
echo                    TEST SUMMARY COMPLETE
echo ================================================================
echo.
echo Test Types Run:
echo 1. C++ Unit Tests (Google Test)
echo 2. .NET Integration Tests (NUnit)
echo.
echo For detailed results, check:
echo - tests\cpp\bin\test_results.xml (C++ tests)
echo - TestResults\ (NUnit tests)
echo.
pause
