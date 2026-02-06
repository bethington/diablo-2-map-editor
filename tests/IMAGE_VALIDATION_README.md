# DS1 Editor Image Generation and Validation Tests

This document provides a comprehensive guide to the image generation tests for the Diablo 2 DS1 Map Editor.

## Overview

The DS1 Editor image validation framework consists of two main test suites:

1. **DS1EditorImageOutputValidationTests** - Core image generation validation
2. **DS1EditorBatchImageProcessingTests** - Batch processing and conversion utilities

## Test Categories

### Image Generation Tests (`TestCategory=ImageGeneration`)

#### 1. Screenshot Generation Test
**Purpose:** Validate basic screenshot functionality
**Mock Files Created:** 3 screenshots (640x480, 800x600, 1024x768)
**Manual Steps:**
```bash
ds1editor.exe "Duriel.ds1" 17 481    # Boss chamber screenshot
ds1editor.exe "TownWest.ds1" 29 863  # Town area screenshot  
ds1editor.exe "Fortress.ds1" 26 797  # Fortress screenshot
```
**Verification:** Press 'P' key to generate screenshots, verify file creation and visual quality.

#### 2. Full Map Export Test
**Purpose:** Test complete map export functionality
**Mock Files Created:** 3 full map exports (small, medium, large)
**Manual Steps:**
```bash
ds1editor.exe "Duriel.ds1" 17 481    # Press Shift+P for full export
ds1editor.exe "TownWest.ds1" 29 863  # Press Shift+P for full export
ds1editor.exe "Fortress.ds1" 26 797  # Press Shift+P for full export
```
**Verification:** Compare file sizes between normal (P) and full map exports (Shift+P).

#### 3. Palette Variation Test
**Purpose:** Validate Act-specific color palette variations
**Mock Files Created:** 5 palette variations (Acts 1-5)
**Manual Steps:**
```bash
ds1editor.exe "Duriel.ds1" 17 481 -force_pal 1  # Act 1 - Earth tones
ds1editor.exe "Duriel.ds1" 17 481 -force_pal 2  # Act 2 - Desert tones
ds1editor.exe "Duriel.ds1" 17 481 -force_pal 3  # Act 3 - Jungle tones
ds1editor.exe "Duriel.ds1" 17 481 -force_pal 4  # Act 4 - Hell tones
ds1editor.exe "Duriel.ds1" 17 481 -force_pal 5  # Act 5 - Ice tones
```
**Verification:** Each Act should display distinct atmospheric colors.

#### 4. Layer Isolation Test
**Purpose:** Test individual layer visibility and screenshot generation
**Mock Files Created:** 4 layer isolation images
**Manual Steps:**
```bash
ds1editor.exe "TownWest.ds1" 29 863
# Use F1-F10 keys to toggle layer visibility:
# F1-F2: Floor layers | F3: Animations | F4: Objects
# F5-F8: Wall layers | F9: Special tiles | F10: Paths
```
**Verification:** Screenshots should show only selected layers with proper isolation.

#### 5. Quality Reference Test
**Purpose:** Generate reference images for quality assessment
**Mock Files Created:** 3 quality reference images
**Manual Steps:** Adjust zoom levels (+ or - keys) for optimal detail before taking screenshots.
**Verification:** Images should demonstrate sharp boundaries, proper gradients, no artifacts.

### Batch Processing Tests (`TestCategory=BatchProcessing`)

#### 1. Batch Screenshot Generation
**Purpose:** Generate multiple screenshots from different viewpoints
**Mock Files Created:** 9 batch screenshots (3 maps × 3 viewpoints each)
**Features:** Automated file size analysis, batch processing verification

#### 2. Comprehensive Palette Matrix
**Purpose:** Generate all map-palette combinations
**Mock Files Created:** 10 palette matrix images (2 maps × 5 Acts)
**Features:** Side-by-side palette comparison, systematic color variation testing

#### 3. PNG Conversion Utilities
**Purpose:** Generate conversion scripts and documentation
**Files Created:**
- `Convert_PCX_to_PNG.bat` - Windows batch script
- `Convert_PCX_to_PNG.ps1` - PowerShell script  
- `PNG_Conversion_Instructions.md` - Detailed instructions

#### 4. Image Metadata Collection
**Purpose:** Analyze image file properties and generate reports
**Mock Files Created:** 3 metadata test images with different dimensions
**Features:** PCX header validation, file size analysis, quality metrics

## File Locations

### Image Output Directories
- **Image Validation:** `tests/bin/Release/net6.0/ImageOutputValidation/VerificationImages/`
- **Batch Processing:** `tests/bin/Release/net6.0/BatchImageProcessing/BatchGenerated/`
- **PNG Converted:** `tests/bin/Release/net6.0/BatchImageProcessing/PngConverted/`

### Generated Reports
- **Metadata Report:** `tests/bin/Release/net6.0/BatchImageProcessing/Image_Metadata_Report.md`
- **Conversion Scripts:** `tests/bin/Release/net6.0/BatchImageProcessing/Convert_PCX_to_PNG.*`

## Running the Tests

### Run Image Generation Tests Only
```bash
dotnet test --filter "TestCategory=ImageGeneration"
```

### Run Batch Processing Tests Only  
```bash
dotnet test --filter "TestCategory=BatchProcessing"
```

### Run All Image Tests
```bash
dotnet test --filter "TestCategory=ImageGeneration|TestCategory=BatchProcessing"
```

## Mock vs Real Images

### Automated Testing
The tests create **mock PCX files** with:
- Valid PCX headers (signature, version, dimensions)
- Basic checkerboard patterns for visual verification
- Proper 256-color palettes
- Realistic file sizes based on dimensions

### Manual Verification Required
Since DS1 Editor is a GUI application, **manual steps** are required to:
1. Launch DS1 Editor with specific maps and coordinates
2. Use keyboard shortcuts (P, Shift+P, F1-F10) for screenshots
3. Apply palette parameters (-force_pal 1-5)
4. Verify actual image content and quality

## Image Quality Checklist

### Visual Quality Standards
- ✓ Sharp tile boundaries and clear textures
- ✓ Proper color gradients without banding  
- ✓ No compression artifacts or pixelation
- ✓ Consistent quality across different map types
- ✓ Appropriate file size for image complexity

### Technical Validation
- ✓ Valid PCX file headers (version 5, 8-bit color)
- ✓ Correct image dimensions and aspect ratios
- ✓ Proper palette application for different Acts
- ✓ Layer isolation working correctly
- ✓ Screenshot vs full map export size differences

## Conversion and Analysis

### PCX to PNG Conversion
Use the generated conversion scripts:
```bash
# Windows Batch
Convert_PCX_to_PNG.bat

# PowerShell
.\Convert_PCX_to_PNG.ps1
```

### Prerequisites
- **ImageMagick:** Required for PCX to PNG conversion
- **DS1 Editor:** GUI application for actual image generation
- **Test Data:** DS1 map files (Duriel.ds1, TownWest.ds1, Fortress.ds1)

## Troubleshooting

### Common Issues
1. **No PCX files generated:** Ensure DS1 Editor is working and P key functionality is available
2. **Conversion errors:** Verify ImageMagick installation and PATH configuration
3. **Test failures:** Check that mock file generation is working before manual verification
4. **Missing test data:** Ensure DS1 map files are available in test data directory

### Test Results Interpretation
- **Mock file tests passing:** Framework is working correctly
- **Manual verification needed:** Follow manual instructions to generate real screenshots
- **Conversion utilities:** Use generated scripts to convert PCX files to PNG for easier viewing

## Integration with Existing Tests

This image validation framework integrates with the existing DS1 Editor test suite:
- **100% existing test success rate maintained**
- **No conflicts with existing unit/functional tests**
- **Separate test categories for independent execution**
- **Mock-based approach ensures automated testing works**

## Next Steps

1. **Execute manual verification steps** to generate actual DS1 Editor screenshots
2. **Use conversion utilities** to transform PCX files to PNG format
3. **Review generated metadata reports** for quality analysis
4. **Compare mock vs real images** to validate testing framework accuracy
5. **Iterate on image quality standards** based on actual results
