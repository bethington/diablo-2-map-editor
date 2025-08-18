# DS1 Editor Unity Test Suite Runner (PowerShell)
# Comprehensive test execution with detailed reporting and image processing validation

param(
    [string]$TestCategory = "All",
    [switch]$GenerateReport,
    [switch]$Detailed,
    [switch]$Help
)

if ($Help) {
    Write-Host @"
DS1 Editor Unity Test Suite Runner

USAGE:
    .\RunTests.ps1 [OPTIONS]

OPTIONS:
    -TestCategory <category>  Run specific test category (All, Unit, Functional, Integration)
    -GenerateReport           Generate detailed test reports
    -Detailed                 Show detailed test output
    -Help                     Show this help message

EXAMPLES:
    .\RunTests.ps1                                # Run all tests
    .\RunTests.ps1 -TestCategory Unit             # Run unit tests only
    .\RunTests.ps1 -GenerateReport -Detailed      # Run all tests with detailed reporting
    .\RunTests.ps1 -TestCategory Integration      # Run integration tests only

TEST CATEGORIES:
    All          - Run all test suites (default)
    Unit         - Parameter validation and basic functionality
    Functional   - File operations and output verification
    Integration  - Image processing and advanced scenarios
"@
    exit 0
}

# Configuration
$TestDir = $PSScriptRoot
$ProjectFile = Join-Path $TestDir "DS1EditorTests.csproj"
$OutputDir = Join-Path $TestDir "TestResults"
$ReleaseDir = Join-Path (Split-Path $TestDir -Parent) "release"
$DS1EditorPath = Join-Path $ReleaseDir "ds1editor.exe"

Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "DS1 Editor Unity Test Suite Runner" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host

# Create output directory
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

Write-Host "Test Environment:" -ForegroundColor Green
Write-Host "- Test Project: $ProjectFile"
Write-Host "- Output Directory: $OutputDir"
Write-Host "- Release Directory: $ReleaseDir"
Write-Host "- DS1 Editor: $DS1EditorPath"
Write-Host

# Prerequisites check
Write-Host "Checking prerequisites..." -ForegroundColor Yellow

# Check .NET SDK
try {
    $dotnetVersion = dotnet --version
    Write-Host "✓ .NET SDK found: $dotnetVersion" -ForegroundColor Green
} catch {
    Write-Host "✗ .NET SDK not found. Please install .NET 6.0 SDK." -ForegroundColor Red
    Write-Host "Download from: https://dotnet.microsoft.com/download" -ForegroundColor Yellow
    exit 1
}

# Check DS1 Editor
if (-not (Test-Path $DS1EditorPath)) {
    Write-Host "✗ DS1 Editor executable not found at: $DS1EditorPath" -ForegroundColor Red
    Write-Host "Please ensure the DS1 editor is built and available." -ForegroundColor Yellow
    exit 1
}
Write-Host "✓ DS1 Editor executable found" -ForegroundColor Green

# Check test DS1 files
$DS1Dir = Join-Path $ReleaseDir "ds1"
if (-not (Test-Path $DS1Dir)) {
    Write-Host "⚠ Test DS1 directory not found. Some tests may be skipped." -ForegroundColor Yellow
} else {
    $ds1Files = Get-ChildItem $DS1Dir -Filter "*.ds1" | Measure-Object
    Write-Host "✓ Test DS1 files directory found ($($ds1Files.Count) DS1 files)" -ForegroundColor Green
}

Write-Host
Write-Host "Prerequisites check completed!" -ForegroundColor Green
Write-Host

# Configure test parameters
$TestFilter = ""
$Verbosity = if ($Detailed) { "detailed" } else { "normal" }

switch ($TestCategory) {
    "Unit" { $TestFilter = "--filter ClassName=DS1EditorUnitTests" }
    "Functional" { $TestFilter = "--filter ClassName=DS1EditorFunctionalTests" }
    "Integration" { $TestFilter = "--filter ClassName=DS1EditorImageIntegrationTests" }
    "All" { $TestFilter = "" }
    default { 
        Write-Host "Invalid test category: $TestCategory" -ForegroundColor Red
        Write-Host "Valid categories: All, Unit, Functional, Integration" -ForegroundColor Yellow
        exit 1
    }
}

Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Test Configuration" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Category: $TestCategory"
Write-Host "Filter: $TestFilter"
Write-Host "Verbosity: $Verbosity"
Write-Host "Generate Report: $GenerateReport"
Write-Host

# Restore packages
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Restoring NuGet Packages" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host

$restoreResult = dotnet restore $ProjectFile
if ($LASTEXITCODE -ne 0) {
    Write-Host "✗ Failed to restore NuGet packages" -ForegroundColor Red
    exit 1
}
Write-Host "✓ NuGet packages restored successfully" -ForegroundColor Green

# Build project
Write-Host
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Building Test Project" -ForegroundColor Cyan  
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host

$buildResult = dotnet build $ProjectFile --configuration Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "✗ Failed to build test project" -ForegroundColor Red
    exit 1
}
Write-Host "✓ Test project built successfully" -ForegroundColor Green

# Execute tests
Write-Host
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Executing Tests" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host

$testArgs = @(
    "test", $ProjectFile,
    "--configuration", "Release",
    "--verbosity", $Verbosity
)

if ($TestFilter) {
    $testArgs += $TestFilter.Split()
}

if ($GenerateReport) {
    $testArgs += "--logger", "trx;LogFileName=DS1EditorTestResults.trx"
    $testArgs += "--results-directory", $OutputDir
    $testArgs += "--collect", "Code Coverage"
}

Write-Host "Executing: dotnet $($testArgs -join ' ')" -ForegroundColor Yellow
Write-Host

$testStartTime = Get-Date
& dotnet @testArgs
$testExitCode = $LASTEXITCODE
$testEndTime = Get-Date
$testDuration = $testEndTime - $testStartTime

Write-Host
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Test Execution Summary" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host

Write-Host "Test Duration: $($testDuration.TotalSeconds.ToString('F2')) seconds"

if ($testExitCode -eq 0) {
    Write-Host "✓ All tests completed successfully!" -ForegroundColor Green
    Write-Host "Test Status: PASSED" -ForegroundColor Green
} else {
    Write-Host "✗ Some tests failed or encountered errors" -ForegroundColor Red
    Write-Host "Test Status: FAILED" -ForegroundColor Red
    Write-Host "Exit Code: $testExitCode" -ForegroundColor Red
}

# Generate reports
if ($GenerateReport) {
    Write-Host
    Write-Host "================================================================" -ForegroundColor Cyan
    Write-Host "Generating Test Reports" -ForegroundColor Cyan
    Write-Host "================================================================" -ForegroundColor Cyan
    Write-Host

    $trxFile = Join-Path $OutputDir "DS1EditorTestResults.trx"
    if (Test-Path $trxFile) {
        Write-Host "✓ Test results file generated: $trxFile" -ForegroundColor Green
        Write-Host "  Open in Visual Studio for detailed analysis" -ForegroundColor Gray
    } else {
        Write-Host "✗ Test results file not generated" -ForegroundColor Red
    }

    # Generate summary report
    $summaryFile = Join-Path $OutputDir "TestSummary.md"
    $summaryContent = @"
# DS1 Editor Test Execution Summary

## Test Information
- **Date**: $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')
- **Duration**: $($testDuration.TotalSeconds.ToString('F2')) seconds
- **Category**: $TestCategory
- **Exit Code**: $testExitCode
- **Status**: $(if ($testExitCode -eq 0) { 'PASSED' } else { 'FAILED' })

## Test Configuration
- **Project**: $ProjectFile
- **Filter**: $TestFilter
- **Verbosity**: $Verbosity
- **Output Directory**: $OutputDir

## Test Categories Executed
$(if ($TestCategory -eq 'All' -or $TestCategory -eq 'Unit') { '- ✓ Unit Tests (Parameter validation, debug flags, etc.)' })
$(if ($TestCategory -eq 'All' -or $TestCategory -eq 'Functional') { '- ✓ Functional Tests (File operations, output verification)' })
$(if ($TestCategory -eq 'All' -or $TestCategory -eq 'Integration') { '- ✓ Integration Tests (Image processing, PNG export)' })

## Manual Testing Required
Some functionality requires manual verification:

### Screenshot Generation
1. Run: ``$DS1EditorPath Duriel.ds1 17 481``
2. Press **P** to generate normal screenshots
3. Press **Shift+P** to generate full map screenshots
4. Verify screenshot-*.pcx files are created

### Floor Tile PNG Export
1. Load DS1 with floor patterns: ``$DS1EditorPath TownWest.ds1 29 863``
2. Use **F1/F2** to show only floor layers
3. Use **+** key to zoom in on tiles
4. Press **P** to capture floor tiles
5. Convert: ``convert screenshot-*.pcx floor-tiles.png``

### Map as PNG Export
1. Load any DS1 file
2. Press **Shift+P** to export full map
3. Convert: ``convert screenshot-*.pcx fullmap.png``
4. Verify entire map is captured

## Generated Files
- Test Results: ``$OutputDir``
- Screenshots: ``$ReleaseDir\screenshot-*.pcx``
- Debug Files: ``$ReleaseDir\Debug\``

## Recommendations
$(if ($testExitCode -eq 0) {
'- All automated tests passed successfully
- Proceed with manual testing scenarios
- Consider running integration tests if not already done'
} else {
'- Review failed tests and fix issues
- Check DS1 editor executable and test data
- Verify test environment setup'
})
"@

    Set-Content -Path $summaryFile -Value $summaryContent -Encoding UTF8
    Write-Host "✓ Summary report generated: $summaryFile" -ForegroundColor Green
}

# Display manual testing instructions
Write-Host
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Manual Testing Instructions" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host

$manualTests = @(
    @{
        Name = "Screenshot Generation Test"
        Command = "$DS1EditorPath Duriel.ds1 17 481"
        Steps = @(
            "Press P to generate normal screenshot",
            "Press Shift+P to generate full map screenshot", 
            "Verify screenshot-*.pcx files are created",
            "Check that images are viewable"
        )
    },
    @{
        Name = "Floor Tile PNG Export Test"
        Command = "$DS1EditorPath TownWest.ds1 29 863"
        Steps = @(
            "Press F1 and F2 to show only floor layers",
            "Use + key to zoom in on floor tile details",
            "Press P to capture floor tile screenshots",
            "Convert PCX to PNG: convert screenshot-*.pcx floor-tiles.png",
            "Verify PNG shows clear floor tile patterns"
        )
    },
    @{
        Name = "Map as PNG Export Test"
        Command = "$DS1EditorPath Fortress.ds1 26 797"
        Steps = @(
            "Press Shift+P to export complete map",
            "Wait for large file generation",
            "Convert to PNG: convert screenshot-*.pcx fullmap.png",
            "Verify PNG contains entire map area",
            "Check all map elements are visible"
        )
    }
)

foreach ($test in $manualTests) {
    Write-Host "$($test.Name):" -ForegroundColor Yellow
    Write-Host "  Command: $($test.Command)" -ForegroundColor Gray
    foreach ($step in $test.Steps) {
        Write-Host "  - $step" -ForegroundColor Gray
    }
    Write-Host
}

Write-Host "PNG Conversion Tools:" -ForegroundColor Yellow
Write-Host "  • ImageMagick: convert input.pcx output.png" -ForegroundColor Gray
Write-Host "  • GIMP: File → Export As → PNG" -ForegroundColor Gray
Write-Host "  • IrfanView with PCX plugin" -ForegroundColor Gray
Write-Host "  • Paint.NET with PCX plugin" -ForegroundColor Gray

# Open results if report generated
if ($GenerateReport -and (Test-Path $OutputDir)) {
    Write-Host
    Write-Host "Opening test results directory..." -ForegroundColor Green
    if ($IsWindows -or $PSVersionTable.PSVersion.Major -lt 6) {
        Start-Process $OutputDir
    } else {
        # Cross-platform approach
        if (Get-Command xdg-open -ErrorAction SilentlyContinue) {
            xdg-open $OutputDir
        } elseif (Get-Command open -ErrorAction SilentlyContinue) {
            open $OutputDir
        }
    }
}

Write-Host
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Test Execution Completed" -ForegroundColor Cyan
Write-Host "================================================================" -ForegroundColor Cyan
Write-Host "Total Duration: $($testDuration.TotalSeconds.ToString('F2')) seconds" -ForegroundColor $(if ($testExitCode -eq 0) { 'Green' } else { 'Red' })
Write-Host "Final Status: $(if ($testExitCode -eq 0) { 'PASSED' } else { 'FAILED' })" -ForegroundColor $(if ($testExitCode -eq 0) { 'Green' } else { 'Red' })

exit $testExitCode
