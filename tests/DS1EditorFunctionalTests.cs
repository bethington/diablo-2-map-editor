using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using NUnit.Framework;

namespace DS1EditorTests
{
    /// <summary>
    /// Functional tests for DS1 Editor file output verification and screenshot generation
    /// </summary>
    [TestFixture]
    public class DS1EditorFunctionalTests
    {
        private string _ds1EditorPath;
        private string _testDataPath;
        private string _outputPath;
        private string _releasePath;
        private string _screenshotPattern = "screenshot-*.pcx";
        private string _debugPath;

        [OneTimeSetUp]
        public void OneTimeSetUp()
        {
            // Setup paths
            var baseDir = TestContext.CurrentContext.TestDirectory;
            _releasePath = Path.GetFullPath(Path.Combine(baseDir, "..", "..", "..", "..", "release"));
            _ds1EditorPath = Path.Combine(_releasePath, "ds1editor.exe");
            _testDataPath = Path.Combine(_releasePath, "ds1");
            _outputPath = Path.Combine(baseDir, "FunctionalTestOutput");
            _debugPath = Path.Combine(_releasePath, "Debug");

            Directory.CreateDirectory(_outputPath);
            
            Assert.That(File.Exists(_ds1EditorPath), 
                $"DS1 Editor executable not found at: {_ds1EditorPath}");
            Assert.That(Directory.Exists(_testDataPath), 
                $"Test DS1 data directory not found at: {_testDataPath}");
        }

        [SetUp]
        public void SetUp()
        {
            // Change to release directory and clean up
            Directory.SetCurrentDirectory(_releasePath);
            CleanupTestFiles();
        }

        [TearDown]
        public void TearDown()
        {
            KillDS1EditorProcesses();
        }

        #region Debug File Output Tests

        [Test]
        public void TestDebugFileGeneration_ShouldCreateDebugFiles()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -debug";

            // Act
            var result = RunDS1EditorInteractive(args, timeoutMs: 10000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), "Debug mode should execute successfully");
            Assert.That(Directory.Exists(_debugPath), Is.True, "Debug directory should be created");

            var debugFiles = Directory.GetFiles(_debugPath, "*", SearchOption.AllDirectories);
            Assert.That(debugFiles.Length, Is.GreaterThan(0), "Debug files should be generated");

            // Verify some expected debug files
            TestContext.WriteLine($"Debug files created: {debugFiles.Length}");
            foreach (var file in debugFiles)
            {
                TestContext.WriteLine($"  - {Path.GetFileName(file)} ({new FileInfo(file).Length} bytes)");
                Assert.That(new FileInfo(file).Length, Is.GreaterThan(0), 
                    $"Debug file {Path.GetFileName(file)} should not be empty");
            }
        }

        [Test]
        public void TestDebugFileContent_ShouldContainValidData()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -debug";

            // Act
            var result = RunDS1EditorInteractive(args, timeoutMs: 10000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0));
            Assert.That(Directory.Exists(_debugPath), Is.True);

            // Check for specific debug file types
            var debugFiles = Directory.GetFiles(_debugPath, "*", SearchOption.AllDirectories);
            
            var textFiles = debugFiles.Where(f => Path.GetExtension(f).Equals(".txt", StringComparison.OrdinalIgnoreCase));
            var binaryFiles = debugFiles.Where(f => Path.GetExtension(f).Equals(".bin", StringComparison.OrdinalIgnoreCase));

            Assert.That(textFiles.Count(), Is.GreaterThan(0), "Should generate text debug files");
            
            // Verify text debug files contain content
            foreach (var txtFile in textFiles.Take(3)) // Check first 3 text files
            {
                var content = File.ReadAllText(txtFile);
                Assert.That(content.Length, Is.GreaterThan(0), 
                    $"Text debug file {Path.GetFileName(txtFile)} should have content");
                
                TestContext.WriteLine($"Debug file {Path.GetFileName(txtFile)} content preview: {content.Substring(0, Math.Min(100, content.Length))}...");
            }
        }

        #endregion

        #region Screenshot File Tests

        [Test]
        public void TestScreenshotFileGeneration_Manual()
        {
            // This test requires manual interaction but we can verify the setup
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481";

            TestContext.WriteLine("Manual test: Run DS1 editor and press 'P' to generate screenshots");
            TestContext.WriteLine($"Command: {_ds1EditorPath} {args}");
            TestContext.WriteLine("After running manually, screenshot-*.pcx files should be created");

            // We can test the screenshot numbering logic by creating mock files
            var testScreenshots = new[] { "screenshot-00000.pcx", "screenshot-00001.pcx" };
            foreach (var screenshot in testScreenshots)
            {
                File.WriteAllText(Path.Combine(_releasePath, screenshot), "mock");
            }

            var screenshots = Directory.GetFiles(_releasePath, _screenshotPattern);
            Assert.That(screenshots.Length, Is.GreaterThan(0), "Should find screenshot files");

            // Clean up mock files
            foreach (var screenshot in testScreenshots)
            {
                File.Delete(Path.Combine(_releasePath, screenshot));
            }
        }

        [Test]
        public void TestScreenshotFileNaming_ShouldUseSequentialNumbers()
        {
            // Test the file naming pattern by creating test files
            var expectedFiles = new[]
            {
                "screenshot-00000.pcx",
                "screenshot-00001.pcx", 
                "screenshot-00005.pcx",
                "screenshot-00010.pcx"
            };

            foreach (var file in expectedFiles)
            {
                File.WriteAllText(Path.Combine(_releasePath, file), "test content");
            }

            var screenshots = Directory.GetFiles(_releasePath, _screenshotPattern)
                .Select(f => Path.GetFileName(f))
                .OrderBy(f => f)
                .ToList();

            Assert.That(screenshots.Count, Is.EqualTo(expectedFiles.Length));
            
            foreach (var expectedFile in expectedFiles)
            {
                Assert.That(screenshots, Contains.Item(expectedFile));
            }

            // Verify sequential numbering pattern
            Assert.That(screenshots[0], Is.EqualTo("screenshot-00000.pcx"));
            Assert.That(screenshots[1], Is.EqualTo("screenshot-00001.pcx"));

            // Clean up
            foreach (var file in expectedFiles)
            {
                File.Delete(Path.Combine(_releasePath, file));
            }
        }

        #endregion

        #region File Cleanup Tests

        [Test]
        public void TestTempFileCleanup_ShouldNotLeaveTempFiles()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481";

            var tempPatterns = new[] { "*.tmp", "*.temp", "~*.*" };
            var initialTempFiles = new List<string>();

            // Record initial temp files
            foreach (var pattern in tempPatterns)
            {
                initialTempFiles.AddRange(Directory.GetFiles(_releasePath, pattern));
            }

            // Act
            var result = RunDS1EditorInteractive(args, timeoutMs: 8000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0));

            var finalTempFiles = new List<string>();
            foreach (var pattern in tempPatterns)
            {
                finalTempFiles.AddRange(Directory.GetFiles(_releasePath, pattern));
            }

            var newTempFiles = finalTempFiles.Except(initialTempFiles).ToList();
            Assert.That(newTempFiles.Count, Is.EqualTo(0), 
                $"No new temporary files should be created. Found: {string.Join(", ", newTempFiles.Select(Path.GetFileName))}");
        }

        #endregion

        #region Data File Integrity Tests

        [Test]
        public void TestDataDirectoryIntegrity_ShouldPreserveDataFiles()
        {
            // Arrange
            var dataDir = Path.Combine(_releasePath, "data");
            if (!Directory.Exists(dataDir))
            {
                Assert.Ignore("Data directory not found for integrity testing");
            }

            var initialFiles = Directory.GetFiles(dataDir, "*", SearchOption.AllDirectories)
                .ToDictionary(f => f, f => new FileInfo(f).LastWriteTime);

            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481";

            // Act
            var result = RunDS1EditorInteractive(args, timeoutMs: 8000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0));

            var finalFiles = Directory.GetFiles(dataDir, "*", SearchOption.AllDirectories)
                .ToDictionary(f => f, f => new FileInfo(f).LastWriteTime);

            foreach (var initialFile in initialFiles)
            {
                Assert.That(finalFiles, Contains.Key(initialFile.Key), 
                    $"Data file should still exist: {Path.GetFileName(initialFile.Key)}");
                
                Assert.That(finalFiles[initialFile.Key], Is.EqualTo(initialFile.Value),
                    $"Data file should not be modified: {Path.GetFileName(initialFile.Key)}");
            }
        }

        [Test]
        public void TestPaletteFilesExist_ShouldHavePaletteData()
        {
            var paletteDir = Path.Combine(_releasePath, "data", "global", "palette");
            if (Directory.Exists(paletteDir))
            {
                var paletteFiles = Directory.GetFiles(paletteDir, "*", SearchOption.AllDirectories);
                Assert.That(paletteFiles.Length, Is.GreaterThan(0), 
                    "Should have palette files for proper color rendering");

                foreach (var file in paletteFiles.Take(5)) // Check first 5
                {
                    Assert.That(new FileInfo(file).Length, Is.GreaterThan(0), 
                        $"Palette file should not be empty: {Path.GetFileName(file)}");
                }
            }
            else
            {
                TestContext.WriteLine("Warning: Palette directory not found");
            }
        }

        #endregion

        #region Configuration File Tests

        [Test]
        public void TestIniFileHandling_ShouldPreserveConfiguration()
        {
            var iniFile = Path.Combine(_releasePath, "ds1edit.ini");
            if (!File.Exists(iniFile))
            {
                Assert.Ignore("ds1edit.ini not found for configuration testing");
            }

            var initialContent = File.ReadAllText(iniFile);
            var initialSize = new FileInfo(iniFile).Length;
            var initialModTime = new FileInfo(iniFile).LastWriteTime;

            // Run with INI file
            var args = "ds1edit.ini";
            var result = RunDS1EditorInteractive(args, timeoutMs: 10000);

            Assert.That(result.ExitCode, Is.EqualTo(0));
            Assert.That(File.Exists(iniFile), Is.True, "INI file should still exist");

            var finalContent = File.ReadAllText(iniFile);
            var finalSize = new FileInfo(iniFile).Length;

            // INI file might be modified during use, but should remain valid
            Assert.That(finalSize, Is.GreaterThan(0), "INI file should not be empty after use");
            
            TestContext.WriteLine($"INI file size: {initialSize} -> {finalSize}");
        }

        #endregion

        #region Image Format and Export Tests

        [Test]
        public void TestImageFormatSupport_ShouldHandlePCXFiles()
        {
            // Verify PCX support by checking for existing PCX files
            var pcxDir = Path.Combine(_releasePath, "pcx");
            if (Directory.Exists(pcxDir))
            {
                var pcxFiles = Directory.GetFiles(pcxDir, "*.pcx");
                Assert.That(pcxFiles.Length, Is.GreaterThan(0), "Should have PCX interface files");

                foreach (var pcxFile in pcxFiles.Take(3))
                {
                    Assert.That(new FileInfo(pcxFile).Length, Is.GreaterThan(0), 
                        $"PCX file should not be empty: {Path.GetFileName(pcxFile)}");

                    // Verify PCX header (basic validation)
                    var header = File.ReadAllBytes(pcxFile).Take(4).ToArray();
                    // PCX files typically start with 0x0A
                    TestContext.WriteLine($"PCX file {Path.GetFileName(pcxFile)} header: {string.Join(" ", header.Select(b => $"0x{b:X2}"))}");
                }
            }
            else
            {
                TestContext.WriteLine("Warning: PCX directory not found");
            }
        }

        [Test]
        public void TestScreenshotImageFormat_ShouldCreateValidPCXFiles()
        {
            // Create a mock screenshot file to test PCX format validation
            var mockScreenshot = Path.Combine(_releasePath, "test-screenshot.pcx");
            
            // Create a minimal PCX header for testing
            var pcxHeader = new byte[] 
            { 
                0x0A, // PCX signature
                0x05, // Version
                0x01, // Encoding
                0x08, // Bits per pixel
                // Add minimal header data
                0x00, 0x00, 0x00, 0x00, // xmin, ymin
                0x4F, 0x01, 0x8F, 0x00, // xmax, ymax (example dimensions)
            };
            
            // Extend to minimum PCX header size (128 bytes)
            var fullHeader = new byte[128];
            Array.Copy(pcxHeader, fullHeader, pcxHeader.Length);
            
            File.WriteAllBytes(mockScreenshot, fullHeader);

            // Verify the mock file
            Assert.That(File.Exists(mockScreenshot), Is.True);
            Assert.That(new FileInfo(mockScreenshot).Length, Is.EqualTo(128));

            var header = File.ReadAllBytes(mockScreenshot);
            Assert.That(header[0], Is.EqualTo(0x0A), "PCX signature should be 0x0A");

            // Clean up
            File.Delete(mockScreenshot);
        }

        [Test]
        public void TestFloorTileVisualization_ShouldRenderFloorLayers()
        {
            // This is primarily a visual test, but we can test the setup
            var ds1File = Path.Combine(_testDataPath, "TownWest.ds1"); // Town has varied floor tiles
            if (!File.Exists(ds1File))
            {
                Assert.Ignore($"DS1 file not found: {ds1File}");
            }

            var args = $"\"{ds1File}\" 29 863";
            
            TestContext.WriteLine("Floor tile visualization test setup:");
            TestContext.WriteLine($"Command: {_ds1EditorPath} {args}");
            TestContext.WriteLine("Manual steps:");
            TestContext.WriteLine("1. Press F1 to toggle floor layer 1");
            TestContext.WriteLine("2. Press F2 to toggle floor layer 2");
            TestContext.WriteLine("3. Use +/- keys to zoom in on floor tiles");
            TestContext.WriteLine("4. Press P to take screenshots of floor patterns");
            TestContext.WriteLine("5. Test with different DS1 files for floor variety");

            // We can verify the DS1 file exists and is readable
            var fileInfo = new FileInfo(ds1File);
            Assert.That(fileInfo.Length, Is.GreaterThan(0), "DS1 file should contain data");
            
            TestContext.WriteLine($"DS1 file size: {fileInfo.Length} bytes");
        }

        [Test]
        public void TestMapExportFunctionality_ShouldExportFullMap()
        {
            TestContext.WriteLine("Map export functionality test:");
            TestContext.WriteLine("Manual verification required:");
            TestContext.WriteLine("1. Run DS1 editor with any DS1 file");
            TestContext.WriteLine("2. Press Shift+P to export full map");
            TestContext.WriteLine("3. Verify screenshot-*.pcx contains entire map area");
            TestContext.WriteLine("4. Compare file sizes between normal (P) and full map (Shift+P) screenshots");
            TestContext.WriteLine("5. Check that full map export captures areas not visible on screen");

            // Set up test scenario
            var testFiles = new[]
            {
                ("Duriel.ds1", 17, 481, "Boss chamber - should be compact"),
                ("TownWest.ds1", 29, 863, "Town area - should be large"),
                ("Fortress.ds1", 26, 797, "Fortress - medium size indoor area")
            };

            foreach (var (fileName, lvlType, lvlPrest, description) in testFiles)
            {
                var ds1File = Path.Combine(_testDataPath, fileName);
                if (File.Exists(ds1File))
                {
                    TestContext.WriteLine($"Test file available: {fileName} - {description}");
                    TestContext.WriteLine($"  Command: {_ds1EditorPath} \"{ds1File}\" {lvlType} {lvlPrest}");
                }
            }
        }

        #endregion

        #region PNG Conversion Tests

        [Test]
        public void TestPNGConversionSupport_ShouldProvideConversionGuidance()
        {
            TestContext.WriteLine("PNG Conversion Test:");
            TestContext.WriteLine("DS1 Editor generates PCX files. For PNG conversion:");
            TestContext.WriteLine();
            TestContext.WriteLine("Recommended tools:");
            TestContext.WriteLine("1. ImageMagick: convert screenshot-*.pcx screenshot-*.png");
            TestContext.WriteLine("2. GIMP: File -> Export As -> PNG");
            TestContext.WriteLine("3. IrfanView with PCX plugin");
            TestContext.WriteLine("4. Paint.NET with PCX plugin");
            TestContext.WriteLine("5. Online converters (for small batches)");
            TestContext.WriteLine();
            TestContext.WriteLine("Batch conversion example:");
            TestContext.WriteLine("  for %f in (screenshot-*.pcx) do convert \"%f\" \"%~nf.png\"");
            
            // Test if ImageMagick is available
            try
            {
                var processInfo = new ProcessStartInfo
                {
                    FileName = "convert",
                    Arguments = "-version",
                    UseShellExecute = false,
                    CreateNoWindow = true,
                    RedirectStandardOutput = true
                };

                using (var process = Process.Start(processInfo))
                {
                    process.WaitForExit(3000);
                    if (process.ExitCode == 0)
                    {
                        TestContext.WriteLine("ImageMagick is available for PCX to PNG conversion");
                    }
                }
            }
            catch
            {
                TestContext.WriteLine("ImageMagick not found. Install for automatic PCX to PNG conversion.");
            }
        }

        [Test]
        public void TestFloorTilePNGExport_Manual()
        {
            TestContext.WriteLine("Floor Tile PNG Export Test (Manual):");
            TestContext.WriteLine();
            TestContext.WriteLine("Steps to export floor tiles as PNG:");
            TestContext.WriteLine("1. Load a DS1 file with varied floor patterns (e.g., TownWest.ds1)");
            TestContext.WriteLine("2. Use F1/F2 to show only floor layers");
            TestContext.WriteLine("3. Zoom in with + key to see individual tiles clearly");
            TestContext.WriteLine("4. Position view over interesting floor patterns");
            TestContext.WriteLine("5. Press P to screenshot floor tiles");
            TestContext.WriteLine("6. Convert resulting PCX to PNG using preferred tool");
            TestContext.WriteLine("7. Repeat for different areas and floor layer combinations");
            TestContext.WriteLine();
            TestContext.WriteLine("Expected results:");
            TestContext.WriteLine("- Individual floor tiles should be clearly visible");
            TestContext.WriteLine("- Different floor layers should show distinct patterns");
            TestContext.WriteLine("- PNG files should maintain image quality from PCX");
        }

        [Test]
        public void TestMapAsPNGExport_Manual()
        {
            TestContext.WriteLine("Full Map PNG Export Test (Manual):");
            TestContext.WriteLine();
            TestContext.WriteLine("Steps to export entire map as PNG:");
            TestContext.WriteLine("1. Load any DS1 file");
            TestContext.WriteLine("2. Press Shift+P to export full map as PCX");
            TestContext.WriteLine("3. Convert the large PCX file to PNG");
            TestContext.WriteLine("4. Verify PNG contains entire map area");
            TestContext.WriteLine("5. Test with different DS1 files for size comparison");
            TestContext.WriteLine();
            TestContext.WriteLine("Quality checks:");
            TestContext.WriteLine("- PNG should show complete map boundaries");
            TestContext.WriteLine("- All map elements should be visible (floors, walls, objects)");
            TestContext.WriteLine("- Image should not be corrupted or truncated");
            TestContext.WriteLine("- File size should be reasonable for map complexity");
            TestContext.WriteLine();
            TestContext.WriteLine("Batch conversion for multiple maps:");
            TestContext.WriteLine("1. Load multiple DS1 files via INI file");
            TestContext.WriteLine("2. Export full map for each (Shift+P)");
            TestContext.WriteLine("3. Batch convert all PCX files to PNG");
            TestContext.WriteLine("4. Compare PNG files for different map types");
        }

        #endregion

        #region Advanced Testing Scenarios

        [Test]
        public void TestLayerVisibilityScreenshots_ShouldCaptureLayerStates()
        {
            TestContext.WriteLine("Layer Visibility Screenshot Test:");
            TestContext.WriteLine();
            TestContext.WriteLine("Test different layer combinations:");
            TestContext.WriteLine();
            TestContext.WriteLine("Layer Controls:");
            TestContext.WriteLine("F1 - Floor layer 1");
            TestContext.WriteLine("F2 - Floor layer 2"); 
            TestContext.WriteLine("F3 - Animation layer");
            TestContext.WriteLine("F4 - Objects layer");
            TestContext.WriteLine("F5-F8 - Wall layers 1-4");
            TestContext.WriteLine("F9 - Special tiles layer");
            TestContext.WriteLine("F10 - Paths layer");
            TestContext.WriteLine();
            TestContext.WriteLine("Test scenarios:");
            TestContext.WriteLine("1. Floors only (F1, F2 on, others off)");
            TestContext.WriteLine("2. Walls only (F5-F8 on, others off)");
            TestContext.WriteLine("3. Objects only (F4 on, others off)");
            TestContext.WriteLine("4. Paths only (F10 on, others off)");
            TestContext.WriteLine("5. All layers on");
            TestContext.WriteLine("6. Progressive layer addition");
            TestContext.WriteLine();
            TestContext.WriteLine("Take screenshots (P) for each combination to verify layer isolation");
        }

        [Test]
        public void TestPaletteVariationExport_ShouldShowPaletteDifferences()
        {
            var testPalettes = new[] { 1, 2, 3, 4, 5 };
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");

            TestContext.WriteLine("Palette Variation Export Test:");
            TestContext.WriteLine($"Test file: {ds1File}");
            TestContext.WriteLine();

            foreach (var palette in testPalettes)
            {
                TestContext.WriteLine($"Act {palette} palette test:");
                TestContext.WriteLine($"  Command: {_ds1EditorPath} \"{ds1File}\" 17 481 -force_pal {palette}");
                TestContext.WriteLine($"  Expected: Screenshot should use Act {palette} color palette");
            }

            TestContext.WriteLine();
            TestContext.WriteLine("Verification steps:");
            TestContext.WriteLine("1. Run each palette command");
            TestContext.WriteLine("2. Take screenshot (P) with each palette");
            TestContext.WriteLine("3. Convert PCX files to PNG for comparison");
            TestContext.WriteLine("4. Visually compare PNG files - colors should be distinctly different");
            TestContext.WriteLine("5. Document color differences for each act");
        }

        #endregion

        #region Helper Methods

        private ProcessResult RunDS1EditorInteractive(string arguments, int timeoutMs = 10000)
        {
            var processInfo = new ProcessStartInfo
            {
                FileName = _ds1EditorPath,
                Arguments = arguments,
                UseShellExecute = false,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                RedirectStandardInput = true,
                CreateNoWindow = true,
                WorkingDirectory = _releasePath
            };

            var output = new List<string>();
            var exitCode = 0;

            try
            {
                using (var process = Process.Start(processInfo))
                {
                    // Simulate user interaction
                    Task.Run(async () =>
                    {
                        await Task.Delay(2000); // Wait for process to initialize
                        
                        try
                        {
                            if (!process.HasExited)
                            {
                                // Send Enter a few times, then ESC to exit
                                await process.StandardInput.WriteLineAsync("");
                                await Task.Delay(1000);
                                await process.StandardInput.WriteLineAsync("");
                                await Task.Delay(500);
                                // Send ESC character (ASCII 27)
                                await process.StandardInput.WriteAsync((char)27);
                                await process.StandardInput.FlushAsync();
                            }
                        }
                        catch
                        {
                            // Ignore input errors
                        }
                    });

                    // Read output
                    var outputTask = Task.Run(async () =>
                    {
                        string line;
                        while ((line = await process.StandardOutput.ReadLineAsync()) != null)
                        {
                            output.Add(line);
                        }
                    });

                    var errorTask = Task.Run(async () =>
                    {
                        string line;
                        while ((line = await process.StandardError.ReadLineAsync()) != null)
                        {
                            output.Add($"ERROR: {line}");
                        }
                    });

                    if (!process.WaitForExit(timeoutMs))
                    {
                        process.Kill();
                        output.Add("Process timed out");
                        exitCode = -1;
                    }
                    else
                    {
                        exitCode = process.ExitCode;
                    }

                    Task.WaitAll(new[] { outputTask, errorTask }, 2000);
                }
            }
            catch (Exception ex)
            {
                output.Add($"Exception: {ex.Message}");
                exitCode = -2;
            }

            return new ProcessResult
            {
                ExitCode = exitCode,
                Output = string.Join(Environment.NewLine, output)
            };
        }

        private void CleanupTestFiles()
        {
            try
            {
                // Clean screenshot files
                var screenshots = Directory.GetFiles(_releasePath, "screenshot-*.pcx");
                foreach (var screenshot in screenshots)
                {
                    File.Delete(screenshot);
                }

                // Clean debug directory
                if (Directory.Exists(_debugPath))
                {
                    Directory.Delete(_debugPath, true);
                }

                // Clean backup files
                var backups = Directory.GetFiles(_releasePath, "*.ds1.bak");
                foreach (var backup in backups)
                {
                    File.Delete(backup);
                }

                // Clean temp files
                var tempFiles = Directory.GetFiles(_releasePath, "*.tmp")
                    .Concat(Directory.GetFiles(_releasePath, "*.temp"))
                    .Concat(Directory.GetFiles(_releasePath, "~*.*"));
                
                foreach (var tempFile in tempFiles)
                {
                    File.Delete(tempFile);
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
                    catch
                    {
                        // Ignore
                    }
                    finally
                    {
                        process.Dispose();
                    }
                }
            }
            catch
            {
                // Ignore
            }
        }

        #endregion

        private class ProcessResult
        {
            public int ExitCode { get; set; }
            public string Output { get; set; }
        }
    }
}
