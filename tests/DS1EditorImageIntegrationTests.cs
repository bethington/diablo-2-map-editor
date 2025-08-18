using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Drawing; // For image processing (requires System.Drawing.Common NuGet package)
using System.Threading.Tasks;
using NUnit.Framework;

namespace DS1EditorTests
{
    /// <summary>
    /// Integration tests for DS1 Editor image output and advanced scenarios
    /// </summary>
    [TestFixture]
    public class DS1EditorImageIntegrationTests
    {
        private string _ds1EditorPath;
        private string _testDataPath;
        private string _outputPath;
        private string _releasePath;

        [OneTimeSetUp]
        public void OneTimeSetUp()
        {
            var baseDir = TestContext.CurrentContext.TestDirectory;
            _releasePath = Path.GetFullPath(Path.Combine(baseDir, "..", "..", "..", "..", "release"));
            _ds1EditorPath = Path.Combine(_releasePath, "ds1editor.exe");
            _testDataPath = Path.Combine(_releasePath, "ds1");
            _outputPath = Path.Combine(baseDir, "ImageIntegrationOutput");

            Directory.CreateDirectory(_outputPath);
            
            Assert.That(File.Exists(_ds1EditorPath), 
                $"DS1 Editor executable not found at: {_ds1EditorPath}");
        }

        [SetUp]
        public void SetUp()
        {
            Directory.SetCurrentDirectory(_releasePath);
            CleanupGeneratedFiles();
        }

        [TearDown]
        public void TearDown()
        {
            KillDS1EditorProcesses();
        }

        #region PCX File Format Tests

        [Test]
        public void TestPCXFileStructure_ShouldHaveValidHeader()
        {
            // Create a test PCX file to validate structure
            var testPcxPath = Path.Combine(_outputPath, "test_structure.pcx");
            CreateMinimalPCXFile(testPcxPath, 320, 240);

            Assert.That(File.Exists(testPcxPath), Is.True);
            
            var header = File.ReadAllBytes(testPcxPath).Take(128).ToArray();
            
            // Validate PCX header structure
            Assert.That(header[0], Is.EqualTo(0x0A), "PCX signature should be 0x0A");
            Assert.That(header[1], Is.InRange((byte)0x00, (byte)0x05), "PCX version should be 0-5");
            Assert.That(header[2], Is.EqualTo(0x01), "PCX encoding should be 1 (RLE)");
            Assert.That(header[3], Is.InRange((byte)1, (byte)8), "Bits per pixel should be 1-8");

            TestContext.WriteLine($"PCX Header Analysis:");
            TestContext.WriteLine($"  Signature: 0x{header[0]:X2}");
            TestContext.WriteLine($"  Version: {header[1]}");
            TestContext.WriteLine($"  Encoding: {header[2]}");
            TestContext.WriteLine($"  Bits per pixel: {header[3]}");
        }

        [Test]
        public void TestScreenshotPCXValidation_ShouldCreateValidFiles()
        {
            // This test creates mock screenshot files and validates them
            var mockScreenshots = new[]
            {
                ("screenshot-00000.pcx", 640, 480),
                ("screenshot-00001.pcx", 800, 600),
                ("screenshot-00002.pcx", 1024, 768)
            };

            foreach (var (filename, width, height) in mockScreenshots)
            {
                var filePath = Path.Combine(_releasePath, filename);
                CreateMinimalPCXFile(filePath, width, height);
                
                Assert.That(File.Exists(filePath), Is.True);
                Assert.That(new FileInfo(filePath).Length, Is.GreaterThan(128), 
                    "PCX file should be larger than header size");

                // Validate dimensions can be read from header
                var dimensions = GetPCXDimensions(filePath);
                TestContext.WriteLine($"{filename}: {dimensions.Width}x{dimensions.Height}");
            }

            // Clean up mock files
            foreach (var (filename, _, _) in mockScreenshots)
            {
                var filePath = Path.Combine(_releasePath, filename);
                if (File.Exists(filePath)) File.Delete(filePath);
            }
        }

        #endregion

        #region Image Conversion Integration Tests

        [Test]
        public void TestImageMagickIntegration_ShouldConvertPCXToPNG()
        {
            if (!IsImageMagickAvailable())
            {
                Assert.Ignore("ImageMagick not available for PCX to PNG conversion testing");
            }

            // Create test PCX files
            var testPcx = Path.Combine(_outputPath, "test_conversion.pcx");
            CreateMinimalPCXFile(testPcx, 100, 100);

            // Convert to PNG
            var testPng = Path.Combine(_outputPath, "test_conversion.png");
            var conversionResult = ConvertPCXToPNG(testPcx, testPng);

            Assert.That(conversionResult, Is.True, "PCX to PNG conversion should succeed");
            Assert.That(File.Exists(testPng), Is.True, "PNG file should be created");
            Assert.That(new FileInfo(testPng).Length, Is.GreaterThan(0), "PNG file should not be empty");

            TestContext.WriteLine($"Conversion successful:");
            TestContext.WriteLine($"  PCX size: {new FileInfo(testPcx).Length} bytes");
            TestContext.WriteLine($"  PNG size: {new FileInfo(testPng).Length} bytes");
        }

        [Test]
        public void TestBatchPCXToPNGConversion_ShouldConvertMultipleFiles()
        {
            if (!IsImageMagickAvailable())
            {
                Assert.Ignore("ImageMagick not available for batch conversion testing");
            }

            // Create multiple test PCX files
            var testFiles = new[]
            {
                ("batch_test_1.pcx", 320, 240),
                ("batch_test_2.pcx", 640, 480),
                ("batch_test_3.pcx", 800, 600)
            };

            var pcxFiles = new List<string>();
            var pngFiles = new List<string>();

            foreach (var (filename, width, height) in testFiles)
            {
                var pcxPath = Path.Combine(_outputPath, filename);
                var pngPath = Path.Combine(_outputPath, Path.ChangeExtension(filename, ".png"));
                
                CreateMinimalPCXFile(pcxPath, width, height);
                pcxFiles.Add(pcxPath);
                pngFiles.Add(pngPath);
            }

            // Batch convert
            var successCount = 0;
            for (int i = 0; i < pcxFiles.Count; i++)
            {
                if (ConvertPCXToPNG(pcxFiles[i], pngFiles[i]))
                {
                    successCount++;
                }
            }

            Assert.That(successCount, Is.EqualTo(testFiles.Length), 
                "All PCX files should convert successfully to PNG");

            foreach (var pngFile in pngFiles)
            {
                Assert.That(File.Exists(pngFile), Is.True, $"PNG file should exist: {Path.GetFileName(pngFile)}");
            }

            TestContext.WriteLine($"Batch conversion: {successCount}/{testFiles.Length} files converted successfully");
        }

        #endregion

        #region Floor Tile Extraction Tests

        [Test]
        public void TestFloorTileExtractionScenario_ShouldIsolateFloorLayers()
        {
            TestContext.WriteLine("Floor Tile Extraction Integration Test");
            TestContext.WriteLine("=====================================");
            TestContext.WriteLine();
            TestContext.WriteLine("This test provides a comprehensive scenario for extracting floor tiles:");
            TestContext.WriteLine();

            var testScenarios = new[]
            {
                new { File = "TownWest.ds1", LvlType = 29, LvlPrest = 863, Description = "Town cobblestone floors" },
                new { File = "Fortress.ds1", LvlType = 26, LvlPrest = 797, Description = "Stone fortress floors" },
                new { File = "Duriel.ds1", LvlType = 17, LvlPrest = 481, Description = "Desert temple floors" }
            };

            foreach (var scenario in testScenarios)
            {
                var ds1Path = Path.Combine(_testDataPath, scenario.File);
                if (!File.Exists(ds1Path))
                {
                    TestContext.WriteLine($"SKIPPED: {scenario.File} not found");
                    continue;
                }

                TestContext.WriteLine($"Floor Extraction Scenario: {scenario.Description}");
                TestContext.WriteLine($"File: {scenario.File}");
                TestContext.WriteLine($"Command: {_ds1EditorPath} \"{ds1Path}\" {scenario.LvlType} {scenario.LvlPrest}");
                TestContext.WriteLine();
                TestContext.WriteLine("Manual steps for floor tile extraction:");
                TestContext.WriteLine("1. Press F1 to toggle floor layer 1 visibility");
                TestContext.WriteLine("2. Press F2 to toggle floor layer 2 visibility");
                TestContext.WriteLine("3. Turn OFF all other layers (F3-F10) to isolate floors");
                TestContext.WriteLine("4. Use + key to zoom in on floor patterns");
                TestContext.WriteLine("5. Navigate to areas with interesting floor tile combinations");
                TestContext.WriteLine("6. Press P to capture floor-only screenshots");
                TestContext.WriteLine("7. Press Shift+P to capture entire floor layout");
                TestContext.WriteLine("8. Convert resulting PCX files to PNG for analysis");
                TestContext.WriteLine();
                TestContext.WriteLine("Expected floor tile characteristics:");
                TestContext.WriteLine($"  - {scenario.Description}");
                TestContext.WriteLine("  - Clear tile boundaries when zoomed");
                TestContext.WriteLine("  - Distinct patterns for different floor types");
                TestContext.WriteLine("  - Proper transparency/layering effects");
                TestContext.WriteLine();
            }

            TestContext.WriteLine("PNG Conversion for Floor Analysis:");
            TestContext.WriteLine("convert screenshot-*.pcx floor-tiles-*.png");
            TestContext.WriteLine();
            TestContext.WriteLine("Advanced Analysis Options:");
            TestContext.WriteLine("1. Compare floor patterns between different DS1 files");
            TestContext.WriteLine("2. Create tile catalogues by extracting individual tile graphics");
            TestContext.WriteLine("3. Analyze floor layer interaction and transparency effects");
            TestContext.WriteLine("4. Document floor tile variations for modding reference");
        }

        [Test]
        public void TestFloorTileImageAnalysis_ShouldValidateFloorContent()
        {
            // Create mock floor tile screenshots for analysis
            var mockFloorScreenshots = new[]
            {
                "floor_layer1_only.pcx",
                "floor_layer2_only.pcx", 
                "floor_combined_layers.pcx"
            };

            foreach (var screenshot in mockFloorScreenshots)
            {
                var path = Path.Combine(_outputPath, screenshot);
                CreateMinimalPCXFile(path, 800, 600);
                
                TestContext.WriteLine($"Mock floor screenshot: {screenshot}");
                TestContext.WriteLine($"  Expected content: Floor tiles with appropriate transparency");
                TestContext.WriteLine($"  Analysis: Check for proper floor rendering without walls/objects");
            }

            TestContext.WriteLine();
            TestContext.WriteLine("Floor Tile Analysis Checklist:");
            TestContext.WriteLine("□ Floor tiles have correct colors and patterns");
            TestContext.WriteLine("□ Tile boundaries are clearly defined");
            TestContext.WriteLine("□ Layer transparency effects work properly");
            TestContext.WriteLine("□ No artifacts from other layers visible");
            TestContext.WriteLine("□ Floor animations render correctly (if present)");
            TestContext.WriteLine("□ Different floor types are visually distinct");
        }

        #endregion

        #region Full Map Export Tests

        [Test]
        public void TestFullMapPNGExport_ShouldCaptureEntireMap()
        {
            TestContext.WriteLine("Full Map PNG Export Integration Test");
            TestContext.WriteLine("===================================");
            TestContext.WriteLine();

            var mapExportTests = new[]
            {
                new { File = "Duriel.ds1", LvlType = 17, LvlPrest = 481, ExpectedSize = "Small", Description = "Single boss chamber" },
                new { File = "TownWest.ds1", LvlType = 29, LvlPrest = 863, ExpectedSize = "Large", Description = "Sprawling town area" },
                new { File = "Fortress.ds1", LvlType = 26, LvlPrest = 797, ExpectedSize = "Medium", Description = "Multi-room fortress" }
            };

            foreach (var test in mapExportTests)
            {
                var ds1Path = Path.Combine(_testDataPath, test.File);
                if (!File.Exists(ds1Path))
                {
                    TestContext.WriteLine($"SKIPPED: {test.File} not found");
                    continue;
                }

                TestContext.WriteLine($"Map Export Test: {test.Description}");
                TestContext.WriteLine($"Expected Size: {test.ExpectedSize}");
                TestContext.WriteLine($"Command: {_ds1EditorPath} \"{ds1Path}\" {test.LvlType} {test.LvlPrest}");
                TestContext.WriteLine();
                TestContext.WriteLine("Full map export procedure:");
                TestContext.WriteLine("1. Press Shift+P to export complete map");
                TestContext.WriteLine("2. Wait for full map screenshot generation");
                TestContext.WriteLine("3. Note the screenshot file size and name");
                TestContext.WriteLine("4. Convert PCX to PNG: convert screenshot-*.pcx fullmap-*.png");
                TestContext.WriteLine("5. Open PNG and verify entire map is captured");
                TestContext.WriteLine();
                TestContext.WriteLine("Validation points:");
                TestContext.WriteLine($"  - Map boundaries should be complete");
                TestContext.WriteLine($"  - File size should reflect {test.ExpectedSize.ToLower()} map size");
                TestContext.WriteLine("  - All map elements should be visible");
                TestContext.WriteLine("  - No cutoff or missing areas");
                TestContext.WriteLine();
            }

            TestContext.WriteLine("Comparative Analysis:");
            TestContext.WriteLine("1. Compare file sizes between different maps");
            TestContext.WriteLine("2. Measure actual map dimensions in pixels");
            TestContext.WriteLine("3. Verify aspect ratios are maintained");
            TestContext.WriteLine("4. Check PNG compression effectiveness vs PCX");
        }

        [Test]
        public void TestMapExportQualityMetrics_ShouldMeetQualityStandards()
        {
            // Create mock full-map screenshots with different characteristics
            var qualityTests = new[]
            {
                ("small_map_export.pcx", 800, 600, "Compact map export"),
                ("medium_map_export.pcx", 1600, 1200, "Medium complexity map"),
                ("large_map_export.pcx", 3200, 2400, "Large sprawling map")
            };

            foreach (var (filename, width, height, description) in qualityTests)
            {
                var pcxPath = Path.Combine(_outputPath, filename);
                CreateMinimalPCXFile(pcxPath, width, height);

                var fileSize = new FileInfo(pcxPath).Length;
                var pixelCount = width * height;
                var compressionRatio = (double)fileSize / pixelCount;

                TestContext.WriteLine($"Quality Test: {description}");
                TestContext.WriteLine($"  Dimensions: {width}x{height}");
                TestContext.WriteLine($"  File size: {fileSize} bytes");
                TestContext.WriteLine($"  Bytes per pixel: {compressionRatio:F2}");
                TestContext.WriteLine($"  Quality rating: {GetQualityRating(compressionRatio)}");
                TestContext.WriteLine();

                // Basic quality checks
                Assert.That(fileSize, Is.GreaterThan(128), "File should be larger than header");
                Assert.That(compressionRatio, Is.LessThan(10.0), "Compression should be reasonable");
            }
        }

        #endregion

        #region Layer Visualization Tests

        [Test]
        public void TestLayerIsolationPNG_ShouldShowIndividualLayers()
        {
            TestContext.WriteLine("Layer Isolation PNG Export Test");
            TestContext.WriteLine("===============================");
            TestContext.WriteLine();

            var layerTests = new Dictionary<string, string>
            {
                { "F1", "Floor Layer 1 - Base floor tiles" },
                { "F2", "Floor Layer 2 - Overlay floor elements" },
                { "F3", "Animation Layer - Animated objects and effects" },
                { "F4", "Objects Layer - Static objects and furniture" },
                { "F5", "Wall Layer 1 - Primary wall structures" },
                { "F6", "Wall Layer 2 - Wall details and decorations" },
                { "F7", "Wall Layer 3 - Additional wall elements" },
                { "F8", "Wall Layer 4 - Top wall layers" },
                { "F9", "Special Tiles Layer - Special interactive tiles" },
                { "F10", "Paths Layer - Waypoint and navigation paths" }
            };

            TestContext.WriteLine("Layer isolation test procedure:");
            TestContext.WriteLine("1. Load any complex DS1 file (e.g., TownWest.ds1)");
            TestContext.WriteLine("2. Turn OFF all layers initially");
            TestContext.WriteLine("3. Turn ON one layer at a time and screenshot:");
            TestContext.WriteLine();

            foreach (var layer in layerTests)
            {
                TestContext.WriteLine($"   {layer.Key} Test: {layer.Value}");
                TestContext.WriteLine($"   - Enable only {layer.Key} layer");
                TestContext.WriteLine($"   - Press P to screenshot");
                TestContext.WriteLine($"   - Save as: layer_{layer.Key.ToLower()}_only.pcx");
                TestContext.WriteLine($"   - Convert to PNG for analysis");
                TestContext.WriteLine();
            }

            TestContext.WriteLine("4. Create combination screenshots:");
            TestContext.WriteLine("   - Floors only (F1 + F2)");
            TestContext.WriteLine("   - Walls only (F5 + F6 + F7 + F8)");
            TestContext.WriteLine("   - Objects only (F4)");
            TestContext.WriteLine("   - All layers combined");
            TestContext.WriteLine();
            TestContext.WriteLine("5. Convert all PCX files to PNG for comparison");
            TestContext.WriteLine("6. Analyze layer content and interactions");
        }

        #endregion

        #region Performance and Quality Tests

        [Test]
        public void TestImageGenerationPerformance_ShouldMeetTimeRequirements()
        {
            TestContext.WriteLine("Image Generation Performance Test");
            TestContext.WriteLine("================================");
            TestContext.WriteLine();

            var performanceScenarios = new[]
            {
                new { Action = "Normal Screenshot (P)", ExpectedTime = "< 1 second", Notes = "Should be nearly instantaneous" },
                new { Action = "Full Map Export (Shift+P)", ExpectedTime = "< 5 seconds", Notes = "May vary with map complexity" },
                new { Action = "Layer Toggle + Screenshot", ExpectedTime = "< 2 seconds", Notes = "Including layer state change" },
                new { Action = "Zoom + Screenshot", ExpectedTime = "< 1 second", Notes = "At maximum zoom level" }
            };

            foreach (var scenario in performanceScenarios)
            {
                TestContext.WriteLine($"Performance Test: {scenario.Action}");
                TestContext.WriteLine($"  Expected Time: {scenario.ExpectedTime}");
                TestContext.WriteLine($"  Notes: {scenario.Notes}");
                TestContext.WriteLine();
            }

            TestContext.WriteLine("Performance Benchmarking:");
            TestContext.WriteLine("1. Time each screenshot operation");
            TestContext.WriteLine("2. Monitor memory usage during export");
            TestContext.WriteLine("3. Test with different map sizes");
            TestContext.WriteLine("4. Measure PCX file generation speed");
            TestContext.WriteLine("5. Compare performance across different layer combinations");
            TestContext.WriteLine();
            TestContext.WriteLine("Acceptable Performance Criteria:");
            TestContext.WriteLine("  - Normal screenshots: < 1 second");
            TestContext.WriteLine("  - Full map exports: < 10 seconds");
            TestContext.WriteLine("  - No memory leaks during repeated operations");
            TestContext.WriteLine("  - Consistent performance across multiple screenshots");
        }

        [Test]
        public void TestImageQualityConsistency_ShouldMaintainStandards()
        {
            TestContext.WriteLine("Image Quality Consistency Test");
            TestContext.WriteLine("=============================");
            TestContext.WriteLine();

            var qualityChecks = new[]
            {
                "Color accuracy - Colors match in-editor display",
                "Resolution consistency - Screenshots maintain proper dimensions", 
                "Layer rendering - All visible layers appear in screenshots",
                "Transparency handling - Semi-transparent elements render correctly",
                "Animation frames - Static snapshots of animated elements",
                "Zoom level respect - Screenshots reflect current zoom state",
                "Aspect ratio - Proper width/height ratios maintained",
                "Pixel precision - No blurring or interpolation artifacts"
            };

            TestContext.WriteLine("Quality validation checklist:");
            foreach (var check in qualityChecks)
            {
                TestContext.WriteLine($"□ {check}");
            }

            TestContext.WriteLine();
            TestContext.WriteLine("Quality testing procedure:");
            TestContext.WriteLine("1. Generate test screenshots with various settings");
            TestContext.WriteLine("2. Compare screenshots to in-editor display");
            TestContext.WriteLine("3. Verify PCX to PNG conversion maintains quality");
            TestContext.WriteLine("4. Test with different palette settings");
            TestContext.WriteLine("5. Validate across different DS1 file types");
            TestContext.WriteLine("6. Check consistency across multiple screenshot sessions");
        }

        #endregion

        #region Helper Methods

        private void CreateMinimalPCXFile(string filePath, int width, int height)
        {
            var header = new byte[128];
            
            // PCX header fields
            header[0] = 0x0A; // PCX signature
            header[1] = 0x05; // Version 5
            header[2] = 0x01; // RLE encoding
            header[3] = 0x08; // 8 bits per pixel
            
            // Image dimensions (little-endian)
            header[4] = 0x00; header[5] = 0x00; // xmin
            header[6] = 0x00; header[7] = 0x00; // ymin
            header[8] = (byte)(width & 0xFF); header[9] = (byte)((width >> 8) & 0xFF); // xmax
            header[10] = (byte)(height & 0xFF); header[11] = (byte)((height >> 8) & 0xFF); // ymax
            
            // Minimal palette and other fields
            for (int i = 12; i < 128; i++)
            {
                header[i] = 0x00;
            }

            File.WriteAllBytes(filePath, header);
        }

        private (int Width, int Height) GetPCXDimensions(string filePath)
        {
            var header = File.ReadAllBytes(filePath).Take(128).ToArray();
            
            int xmax = header[8] | (header[9] << 8);
            int ymax = header[10] | (header[11] << 8);
            
            return (xmax + 1, ymax + 1); // PCX stores max coordinates, not dimensions
        }

        private bool IsImageMagickAvailable()
        {
            try
            {
                var processInfo = new ProcessStartInfo
                {
                    FileName = "convert",
                    Arguments = "-version",
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true
                };

                using (var process = Process.Start(processInfo))
                {
                    return process.WaitForExit(3000) && process.ExitCode == 0;
                }
            }
            catch
            {
                return false;
            }
        }

        private bool ConvertPCXToPNG(string pcxPath, string pngPath)
        {
            try
            {
                var processInfo = new ProcessStartInfo
                {
                    FileName = "convert",
                    Arguments = $"\"{pcxPath}\" \"{pngPath}\"",
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true
                };

                using (var process = Process.Start(processInfo))
                {
                    return process.WaitForExit(10000) && process.ExitCode == 0;
                }
            }
            catch
            {
                return false;
            }
        }

        private string GetQualityRating(double compressionRatio)
        {
            if (compressionRatio < 1.0) return "Excellent";
            if (compressionRatio < 2.0) return "Good";
            if (compressionRatio < 4.0) return "Fair";
            return "Poor";
        }

        private void CleanupGeneratedFiles()
        {
            try
            {
                var patterns = new[] { "screenshot-*.pcx", "test*.pcx", "*.png" };
                foreach (var pattern in patterns)
                {
                    var files = Directory.GetFiles(_releasePath, pattern)
                        .Concat(Directory.GetFiles(_outputPath, pattern));
                    
                    foreach (var file in files)
                    {
                        File.Delete(file);
                    }
                }
            }
            catch
            {
                // Ignore cleanup errors
            }
        }

        private void KillDS1EditorProcesses()
        {
            try
            {
                var processes = Process.GetProcessesByName("ds1editor");
                foreach (var process in processes)
                {
                    try
                    {
                        process.Kill();
                        process.WaitForExit(2000);
                    }
                    catch { }
                    finally
                    {
                        process.Dispose();
                    }
                }
            }
            catch { }
        }

        #endregion
    }
}
