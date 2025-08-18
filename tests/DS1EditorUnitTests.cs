using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using NUnit.Framework;

namespace DS1EditorTests
{
    /// <summary>
    /// Unit tests for DS1 Editor command line parameter validation and basic functionality
    /// </summary>
    [TestFixture]
    public class DS1EditorUnitTests
    {
        private string _ds1EditorPath;
        private string _testDataPath;
        private string _outputPath;
        private string _releasePath;

        [OneTimeSetUp]
        public void OneTimeSetUp()
        {
            // Setup paths relative to test project
            var baseDir = TestContext.CurrentContext.TestDirectory;
            _releasePath = Path.GetFullPath(Path.Combine(baseDir, "..", "..", "..", "..", "release"));
            _ds1EditorPath = Path.Combine(_releasePath, "ds1editor.exe");
            _testDataPath = Path.Combine(_releasePath, "ds1");
            _outputPath = Path.Combine(baseDir, "TestOutput");

            // Create output directory
            Directory.CreateDirectory(_outputPath);
            
            // Verify DS1 editor exists
            Assert.That(File.Exists(_ds1EditorPath), 
                $"DS1 Editor executable not found at: {_ds1EditorPath}");
            
            // Verify test data exists
            Assert.That(Directory.Exists(_testDataPath), 
                $"Test DS1 data directory not found at: {_testDataPath}");
        }

        [SetUp]
        public void SetUp()
        {
            // Clean output directory before each test
            CleanOutputDirectory();
            
            // Change to release directory for tests
            Directory.SetCurrentDirectory(_releasePath);
        }

        [TearDown]
        public void TearDown()
        {
            // Clean up any processes that might be hanging
            KillDS1EditorProcesses();
        }

        #region Parameter Validation Tests

        [Test]
        public void TestValidBasicParameters_ShouldSucceed()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 5000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), 
                $"Expected success but got exit code {result.ExitCode}. Output: {result.Output}");
        }

        [Test]
        public void TestInvalidFileExtension_ShouldFail()
        {
            // Arrange
            var args = "invalid.txt 17 481";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 3000);

            // Assert
            Assert.That(result.ExitCode, Is.Not.EqualTo(0), 
                "Expected failure for invalid file extension");
            Assert.That(result.Output, Does.Contain("first argument must be"));
        }

        [Test]
        public void TestMissingParameters_ShouldFail()
        {
            // Arrange
            var args = "";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 3000);

            // Assert
            Assert.That(result.ExitCode, Is.Not.EqualTo(0), 
                "Expected failure for missing parameters");
        }

        [Test]
        public void TestNonNumericLvlTypeID_ShouldFail()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" abc 481";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 3000);

            // Assert
            Assert.That(result.ExitCode, Is.Not.EqualTo(0), 
                "Expected failure for non-numeric LvlType ID");
            Assert.That(result.Output, Does.Contain("not a numerical value"));
        }

        [Test]
        public void TestNonNumericLvlPrestDef_ShouldFail()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 xyz";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 3000);

            // Assert
            Assert.That(result.ExitCode, Is.Not.EqualTo(0), 
                "Expected failure for non-numeric LvlPrest DEF");
            Assert.That(result.Output, Does.Contain("not a numerical value"));
        }

        #endregion

        #region Debug Flag Tests

        [Test]
        public void TestDebugFlag_ShouldCreateDebugFiles()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -debug";
            var debugDir = Path.Combine(_releasePath, "Debug");

            // Ensure debug directory doesn't exist before test
            if (Directory.Exists(debugDir))
                Directory.Delete(debugDir, true);

            // Act
            var result = RunDS1Editor(args, timeoutMs: 8000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), 
                $"Debug flag test failed with exit code {result.ExitCode}");
            Assert.That(Directory.Exists(debugDir), Is.True, 
                "Debug directory should be created when -debug flag is used");
            
            // Check for debug files
            var debugFiles = Directory.GetFiles(debugDir, "*", SearchOption.AllDirectories);
            Assert.That(debugFiles.Length, Is.GreaterThan(0), 
                "Debug directory should contain debug files");
        }

        [Test]
        public void TestNoVisDebugFlag_ShouldSucceed()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -no_vis_debug";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 5000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), 
                "No visual debug flag test should succeed");
        }

        [Test]
        public void TestCombinedDebugFlags_ShouldSucceed()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -debug -no_vis_debug";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 8000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), 
                "Combined debug flags test should succeed");
        }

        #endregion

        #region Force Palette Tests

        [TestCase(1)]
        [TestCase(2)]
        [TestCase(3)]
        [TestCase(4)]
        [TestCase(5)]
        public void TestValidForcePalette_ShouldSucceed(int actNumber)
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -force_pal {actNumber}";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 5000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), 
                $"Force palette Act {actNumber} should succeed");
        }

        [TestCase(0)]
        [TestCase(6)]
        [TestCase(-1)]
        public void TestInvalidForcePalette_ShouldFail(int actNumber)
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -force_pal {actNumber}";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 3000);

            // Assert
            Assert.That(result.ExitCode, Is.Not.EqualTo(0), 
                $"Force palette Act {actNumber} should fail (invalid act number)");
        }

        [Test]
        public void TestForcePaletteNonNumeric_ShouldFail()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -force_pal abc";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 3000);

            // Assert
            Assert.That(result.ExitCode, Is.Not.EqualTo(0), 
                "Force palette with non-numeric value should fail");
        }

        [Test]
        public void TestForcePaletteMissingValue_ShouldFail()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -force_pal";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 3000);

            // Assert
            Assert.That(result.ExitCode, Is.Not.EqualTo(0), 
                "Force palette without value should fail");
        }

        #endregion

        #region Resize Parameter Tests

        [Test]
        public void TestValidResize_ShouldSucceed()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -resize 50 40";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 5000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), 
                "Valid resize parameters should succeed");
        }

        [Test]
        public void TestResizeNonNumericWidth_ShouldFail()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -resize abc 40";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 3000);

            // Assert
            Assert.That(result.ExitCode, Is.Not.EqualTo(0), 
                "Resize with non-numeric width should fail");
        }

        [Test]
        public void TestResizeNonNumericHeight_ShouldFail()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -resize 50 def";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 3000);

            // Assert
            Assert.That(result.ExitCode, Is.Not.EqualTo(0), 
                "Resize with non-numeric height should fail");
        }

        [Test]
        public void TestResizeMissingHeight_ShouldFail()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -resize 50";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 3000);

            // Assert
            Assert.That(result.ExitCode, Is.Not.EqualTo(0), 
                "Resize missing height parameter should fail");
        }

        #endregion

        #region INI File Tests

        [Test]
        public void TestValidIniFile_ShouldSucceed()
        {
            // Arrange
            var iniFile = Path.Combine(_releasePath, "ds1edit.ini");
            if (!File.Exists(iniFile))
            {
                Assert.Ignore("ds1edit.ini file not found for testing");
            }

            var args = "ds1edit.ini";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 8000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), 
                "Valid INI file should load successfully");
        }

        #endregion

        #region Complex Parameter Combination Tests

        [Test]
        public void TestMultipleValidParameters_ShouldSucceed()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -debug -force_pal 3 -resize 60 50 -no_vis_debug";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 10000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), 
                "Multiple valid parameters should succeed");
        }

        [Test]
        public void TestNoCheckActFlag_ShouldSucceed()
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, "Duriel.ds1");
            var args = $"\"{ds1File}\" 17 481 -no_check_act";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 5000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), 
                "No check act flag should succeed");
        }

        #endregion

        #region Different DS1 File Tests

        [TestCase("Duriel.ds1", 17, 481)]
        [TestCase("TownWest.ds1", 29, 863)]
        [TestCase("Fortress.ds1", 26, 797)]
        [TestCase("Wthrone.ds1", 34, 1086)]
        public void TestDifferentDS1Files_ShouldSucceed(string fileName, int lvlTypeId, int lvlPrestDef)
        {
            // Arrange
            var ds1File = Path.Combine(_testDataPath, fileName);
            if (!File.Exists(ds1File))
            {
                Assert.Ignore($"DS1 file not found: {ds1File}");
            }

            var args = $"\"{ds1File}\" {lvlTypeId} {lvlPrestDef}";

            // Act
            var result = RunDS1Editor(args, timeoutMs: 5000);

            // Assert
            Assert.That(result.ExitCode, Is.EqualTo(0), 
                $"DS1 file {fileName} should load successfully");
        }

        #endregion

        #region Helper Methods

        private ProcessResult RunDS1Editor(string arguments, int timeoutMs = 5000)
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
                    // Send input to simulate user interaction (like pressing ESC to exit)
                    Task.Run(async () =>
                    {
                        await Task.Delay(1000); // Wait a bit for the process to start
                        try
                        {
                            if (!process.HasExited)
                            {
                                // Send ESC key sequence to exit
                                await process.StandardInput.WriteLineAsync("");
                                await process.StandardInput.FlushAsync();
                            }
                        }
                        catch
                        {
                            // Ignore errors in input handling
                        }
                    });

                    // Read output asynchronously
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

                    // Wait for process completion or timeout
                    if (!process.WaitForExit(timeoutMs))
                    {
                        process.Kill();
                        output.Add("Process timed out and was killed");
                        exitCode = -1;
                    }
                    else
                    {
                        exitCode = process.ExitCode;
                    }

                    // Wait for output tasks to complete
                    Task.WaitAll(new[] { outputTask, errorTask }, 1000);
                }
            }
            catch (Exception ex)
            {
                output.Add($"Exception running process: {ex.Message}");
                exitCode = -2;
            }

            return new ProcessResult
            {
                ExitCode = exitCode,
                Output = string.Join(Environment.NewLine, output)
            };
        }

        private void CleanOutputDirectory()
        {
            if (Directory.Exists(_outputPath))
            {
                try
                {
                    Directory.Delete(_outputPath, true);
                    Directory.CreateDirectory(_outputPath);
                }
                catch
                {
                    // Ignore cleanup errors
                }
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
                        process.WaitForExit(1000);
                    }
                    catch
                    {
                        // Ignore kill errors
                    }
                    finally
                    {
                        process.Dispose();
                    }
                }
            }
            catch
            {
                // Ignore errors in process cleanup
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
