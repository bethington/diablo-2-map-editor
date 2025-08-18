# DS1 Editor Unity Test Suite

## Overview
This comprehensive Unity test suite validates all functionality of the Diablo II DS1 Map Editor, including command line parameters, file outputs, image generation, and advanced scenarios like floor tile extraction and PNG export.

## Test Architecture

### Test Classes
1. **DS1EditorUnitTests.cs** - Unit tests for parameter validation and basic functionality
2. **DS1EditorFunctionalTests.cs** - Functional tests for file operations and output verification
3. **DS1EditorImageIntegrationTests.cs** - Integration tests for image processing and advanced scenarios
4. **DS1EditorTestRunner.cs** - Test orchestration and reporting

### Test Framework
- **Framework**: NUnit 3.13.3
- **Target**: .NET 6.0
- **Dependencies**: System.Drawing.Common for image processing

## Setup Instructions

### Prerequisites
1. DS1 Editor executable (`ds1editor.exe`) in the `release` directory
2. Test DS1 files in `release\ds1\` directory
3. .NET 6.0 SDK installed
4. Visual Studio or VS Code with C# extension
5. Optional: ImageMagick for PCX to PNG conversion

### Installation
```bash
# Navigate to the tests directory
cd "Diablo 2 Map Editor\tests"

# Restore NuGet packages
dotnet restore

# Build the test project
dotnet build

# Run all tests
dotnet test
```

## Test Categories

### 1. Unit Tests (DS1EditorUnitTests)

#### Parameter Validation Tests
- ✅ Valid basic parameters (DS1 file + LvlType + LvlPrest)
- ✅ Invalid file extensions
- ✅ Missing parameters
- ✅ Non-numeric LvlType ID
- ✅ Non-numeric LvlPrest DEF

#### Debug Flag Tests
- ✅ `-debug` flag functionality
- ✅ `-no_vis_debug` flag functionality  
- ✅ Combined debug flags
- ✅ Debug file creation verification

#### Force Palette Tests
- ✅ Valid palette acts (1-5)
- ✅ Invalid palette acts (0, 6, -1)
- ✅ Non-numeric palette values
- ✅ Missing palette value

#### Resize Parameter Tests
- ✅ Valid resize parameters
- ✅ Non-numeric width/height
- ✅ Missing height parameter

#### File Handling Tests
- ✅ Valid INI file loading
- ✅ Different DS1 file types
- ✅ Complex parameter combinations

### 2. Functional Tests (DS1EditorFunctionalTests)

#### File Output Tests
- ✅ Debug file generation and content validation
- ✅ Screenshot file naming and structure
- ✅ Temporary file cleanup verification
- ✅ Data directory integrity
- ✅ Configuration file handling

#### Image Format Tests
- ✅ PCX file format validation
- ✅ Screenshot file structure
- ✅ Image quality verification
- ✅ Floor tile visualization scenarios
- ✅ Map export functionality

### 3. Integration Tests (DS1EditorImageIntegrationTests)

#### PCX File Format Tests
- ✅ PCX header structure validation
- ✅ File dimension extraction
- ✅ Screenshot PCX validation

#### Image Conversion Tests
- ✅ ImageMagick integration
- ✅ PCX to PNG conversion
- ✅ Batch conversion processing

#### Floor Tile Extraction Tests
- ✅ Layer isolation scenarios
- ✅ Floor tile image analysis
- ✅ Multi-layer visualization

#### Full Map Export Tests
- ✅ Complete map capture
- ✅ Quality metrics validation
- ✅ Performance testing

## Command Line Parameters Tested

### Basic Syntax
```bash
# Single DS1 file
ds1editor.exe <file.ds1> <lvltype_id> <lvlprest_def> [options]

# INI file batch processing
ds1editor.exe <file.ini>
```

### Optional Parameters
| Parameter | Description | Test Coverage |
|-----------|-------------|---------------|
| `-debug` | Enable debug mode, create debug files | ✅ Tested |
| `-no_vis_debug` | Hide visual debug information | ✅ Tested |
| `-force_pal <1-5>` | Force specific act palette | ✅ All acts tested |
| `-resize <w> <h>` | Resize map dimensions | ✅ Valid/invalid inputs |
| `-no_check_act` | Skip act validation | ✅ Tested |
| `-force_dt1 <files>` | Force DT1 tile files | ⚠️ Basic validation only |

## Image Output Testing

### Screenshot Generation
The tests cover comprehensive screenshot functionality:

#### Normal Screenshots (P key)
- File naming convention: `screenshot-00000.pcx`, `screenshot-00001.pcx`, etc.
- PCX format validation
- File size and quality checks
- Layer visibility reflection

#### Full Map Screenshots (Shift+P key)
- Complete map area capture
- Larger file sizes for complex maps
- Boundary completeness verification
- Performance timing validation

### Floor Tile Extraction

#### Test Scenarios
1. **Town Floor Tiles** (`TownWest.ds1`)
   - Cobblestone patterns
   - Layer F1/F2 isolation
   - Zoom level testing

2. **Fortress Floor Tiles** (`Fortress.ds1`)
   - Stone floor patterns
   - Indoor tile varieties
   - Layer combination testing

3. **Desert Floor Tiles** (`Duriel.ds1`)
   - Temple floor patterns
   - Special surface types
   - Layer transparency testing

#### Floor Tile PNG Export Process
```bash
# 1. Load DS1 with varied floor patterns
ds1editor.exe TownWest.ds1 29 863

# 2. Manual steps (tested via documentation):
#    - Press F1/F2 to show floor layers only
#    - Use + key to zoom in on tiles
#    - Press P to screenshot floor tiles
#    - Convert PCX to PNG

# 3. Conversion command
convert screenshot-*.pcx floor-tiles.png
```

### Map as PNG Export

#### Full Map Export Process
```bash
# 1. Load any DS1 file
ds1editor.exe Fortress.ds1 26 797

# 2. Export full map
#    - Press Shift+P for complete map capture
#    - Wait for large file generation

# 3. Convert to PNG
convert screenshot-*.pcx fullmap.png

# 4. Validation
#    - Verify entire map boundaries
#    - Check all elements visible
#    - Confirm proper scaling
```

#### Map Size Expectations
| Map Type | Expected Size | Example File |
|----------|---------------|--------------|
| Boss Chamber | Small (800x600) | Duriel.ds1 |
| Town Area | Large (1600x1200+) | TownWest.ds1 |
| Fortress | Medium (1200x900) | Fortress.ds1 |

## PNG Conversion Tools

### Recommended Tools
1. **ImageMagick** (Primary recommendation)
   ```bash
   # Single file conversion
   convert input.pcx output.png
   
   # Batch conversion
   convert screenshot-*.pcx screenshot-%d.png
   ```

2. **GIMP** (GUI option)
   - File → Open → Select PCX file
   - File → Export As → Choose PNG format

3. **IrfanView** (Windows)
   - Install PCX plugin
   - Batch conversion available

4. **Paint.NET** (Windows)
   - Requires PCX plugin
   - Good for single file conversion

### Quality Considerations
- PCX files maintain original quality
- PNG conversion should preserve image fidelity
- Compression settings affect file size
- Transparency handling varies by tool

## Manual Testing Requirements

### Interactive Tests
Some functionality requires manual interaction that cannot be fully automated:

#### Screenshot Generation
1. Run DS1 editor with test parameters
2. Press `P` for normal screenshots
3. Press `Shift+P` for full map screenshots
4. Verify files are created and viewable

#### Layer Visibility Testing
1. Use function keys F1-F10 to toggle layers
2. Take screenshots with different combinations
3. Verify layer isolation works correctly
4. Document visual differences

#### Floor Tile Extraction
1. Load DS1 with varied floor patterns
2. Use F1/F2 to isolate floor layers
3. Zoom in with + key for tile detail
4. Capture screenshots of tile patterns
5. Convert to PNG and analyze

## Running the Tests

### Full Test Suite
```bash
# Run all tests with detailed output
dotnet test --verbosity normal

# Run specific test class
dotnet test --filter "ClassName=DS1EditorUnitTests"

# Run tests with specific category
dotnet test --filter "Category=Integration"
```

### Test Categories
```bash
# Unit tests only
dotnet test --filter "TestCategory=Unit"

# Functional tests only  
dotnet test --filter "TestCategory=Functional"

# Integration tests only
dotnet test --filter "TestCategory=Integration"

# Manual tests documentation
dotnet test --filter "TestCategory=Manual"
```

## Test Results Interpretation

### Success Criteria
- ✅ Exit code 0 for valid parameters
- ✅ Exit code != 0 for invalid parameters
- ✅ Debug files created with `-debug` flag
- ✅ Screenshot files generated
- ✅ No temporary files remain after execution
- ✅ Data files remain intact

### Failure Investigation
- Check DS1 editor executable path
- Verify test DS1 files exist
- Review parameter syntax
- Check file permissions
- Monitor for hanging processes

### Performance Benchmarks
- Normal screenshots: < 1 second
- Full map exports: < 10 seconds (varies by complexity)
- Parameter validation: < 100ms
- File cleanup: Complete after execution

## Extending the Tests

### Adding New Test Cases
1. Create test method in appropriate test class
2. Follow naming convention: `Test[Functionality]_Should[ExpectedResult]`
3. Use appropriate `[TestCase]` attributes for parameterized tests
4. Add to relevant test category

### Adding New DS1 Files
1. Place DS1 file in `release\ds1\` directory
2. Add test case with appropriate LvlType/LvlPrest values
3. Update documentation with file characteristics
4. Test with various parameter combinations

### Image Processing Extensions
1. Add new image format support tests
2. Extend quality validation metrics
3. Add performance benchmarking
4. Include error handling scenarios

## Troubleshooting

### Common Issues
1. **DS1 Editor Not Found**: Verify executable exists in release directory
2. **Test DS1 Files Missing**: Ensure DS1 files are in `release\ds1\`
3. **Permission Errors**: Run tests with appropriate file system access
4. **Hanging Processes**: Tests include timeout and cleanup mechanisms
5. **Image Conversion Failures**: Install ImageMagick for PCX to PNG conversion

### Debug Information
- Test output includes detailed logging
- Failed tests show command line and output
- File paths and sizes are logged
- Process exit codes are captured

## Integration with CI/CD

### Automated Testing
```yaml
# Example GitHub Actions workflow
name: DS1 Editor Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: windows-latest
    steps:
    - uses: actions/checkout@v3
    - uses: actions/setup-dotnet@v3
      with:
        dotnet-version: '6.0.x'
    - name: Run Tests
      run: dotnet test tests/DS1EditorTests.csproj --verbosity normal
```

### Manual Test Automation
- Create batch scripts for manual test scenarios
- Document expected outcomes
- Provide validation checklists
- Include image comparison tools

This comprehensive test suite ensures the DS1 Editor functions correctly across all supported parameters and use cases, with particular emphasis on image output functionality including floor tile extraction and PNG export capabilities.
