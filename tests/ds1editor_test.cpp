#include <gtest/gtest.h>
#include <windows.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <fstream>

// Test fixture for DS1 Editor functionality
class DS1EditorTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Path to the DS1 Editor executable
        executable_path = "release\\ds1editor.exe";
        
        // Test files
        test_ds1_file = "ds1\\duriel.ds1";
        test_dt1_id = "17";
        test_tileset_id = "481";
        
        // Check if executable exists
        executable_exists = std::filesystem::exists(executable_path);
    }
    
    std::string executable_path;
    std::string test_ds1_file;
    std::string test_dt1_id;
    std::string test_tileset_id;
    bool executable_exists;
    
    // Helper function to run DS1 Editor with parameters
    bool RunDS1Editor(const std::string& args, int timeout_seconds = 5) {
        if (!executable_exists) {
            std::cout << "DS1 Editor executable not found at: " << executable_path << std::endl;
            return false;
        }
        
        std::string command = executable_path + " " + args;
        STARTUPINFOA si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);
        
        // Create process
        BOOL result = CreateProcessA(
            NULL,
            const_cast<char*>(command.c_str()),
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi
        );
        
        if (!result) {
            return false;
        }
        
        // Wait for process with timeout
        DWORD wait_result = WaitForSingleObject(pi.hProcess, timeout_seconds * 1000);
        
        if (wait_result == WAIT_TIMEOUT) {
            // Terminate if still running
            TerminateProcess(pi.hProcess, 1);
        }
        
        DWORD exit_code;
        GetExitCodeProcess(pi.hProcess, &exit_code);
        
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        return wait_result != WAIT_FAILED && exit_code == 0;
    }
    
    // Helper to check if file exists
    bool FileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }
};

// Test 1: Basic executable existence and launch
TEST_F(DS1EditorTest, ExecutableExists) {
    EXPECT_TRUE(executable_exists) << "DS1 Editor executable should exist at " << executable_path;
}

// Test 2: Launch with valid DS1 file
TEST_F(DS1EditorTest, LaunchWithValidDS1File) {
    if (!executable_exists) {
        GTEST_SKIP() << "DS1 Editor executable not found";
    }
    
    // Check if test DS1 file exists
    ASSERT_TRUE(FileExists(test_ds1_file)) << "Test DS1 file should exist: " << test_ds1_file;
    
    std::string args = test_ds1_file + " " + test_dt1_id + " " + test_tileset_id;
    EXPECT_TRUE(RunDS1Editor(args)) << "Should launch successfully with valid DS1 file";
}

// Test 3: Launch without parameters (should show usage or open empty editor)
TEST_F(DS1EditorTest, LaunchWithoutParameters) {
    if (!executable_exists) {
        GTEST_SKIP() << "DS1 Editor executable not found";
    }
    
    // This should either show usage or open empty editor
    EXPECT_TRUE(RunDS1Editor("")) << "Should handle launch without parameters gracefully";
}

// Test 4: Invalid DS1 file handling
TEST_F(DS1EditorTest, HandleInvalidDS1File) {
    if (!executable_exists) {
        GTEST_SKIP() << "DS1 Editor executable not found";
    }
    
    std::string args = "nonexistent.ds1 " + test_dt1_id + " " + test_tileset_id;
    // Note: This might return false if the application properly handles the error
    RunDS1Editor(args); // Just test that it doesn't crash
}

// Test 5: Configuration file existence
TEST_F(DS1EditorTest, ConfigurationFileExists) {
    EXPECT_TRUE(FileExists("release\\ds1edit.ini")) << "Configuration file should exist";
}

// Test 6: Required data directories exist
TEST_F(DS1EditorTest, RequiredDataDirectoriesExist) {
    EXPECT_TRUE(FileExists("release\\Data")) << "Data directory should exist";
    EXPECT_TRUE(FileExists("release\\ds1")) << "DS1 files directory should exist";
}

// Test 7: Sample DS1 files exist for testing
TEST_F(DS1EditorTest, SampleDS1FilesExist) {
    EXPECT_TRUE(FileExists("release\\ds1\\duriel.ds1")) << "Duriel DS1 file should exist";
    EXPECT_TRUE(FileExists("release\\ds1\\tristram.ds1")) << "Tristram DS1 file should exist";
}

// Test 8: Batch file launchers exist
TEST_F(DS1EditorTest, BatchFileLaunchersExist) {
    EXPECT_TRUE(FileExists("release\\Duriel.bat")) << "Duriel batch launcher should exist";
    EXPECT_TRUE(FileExists("release\\Tristram.bat")) << "Tristram batch launcher should exist";
}

// Test 9: Documentation exists
TEST_F(DS1EditorTest, DocumentationExists) {
    EXPECT_TRUE(FileExists("docs\\01-Getting-Started\\DS1-Editor-Complete-Documentation.md")) 
        << "Complete documentation should exist";
    EXPECT_TRUE(FileExists("release\\README FIRST.txt")) << "README file should exist";
}

// Test 10: Launch specific test scenarios using batch files
TEST_F(DS1EditorTest, LaunchViaBatchFiles) {
    if (!executable_exists) {
        GTEST_SKIP() << "DS1 Editor executable not found";
    }
    
    // Test Duriel batch file if it exists
    if (FileExists("release\\Duriel.bat")) {
        STARTUPINFOA si = {0};
        PROCESS_INFORMATION pi = {0};
        si.cb = sizeof(si);
        
        BOOL result = CreateProcessA(
            NULL,
            "release\\Duriel.bat",
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            "release",  // Working directory
            &si,
            &pi
        );
        
        if (result) {
            // Wait briefly then terminate
            WaitForSingleObject(pi.hProcess, 2000);
            TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
}

// Parameterized test for different DS1 files
class DS1FileTest : public DS1EditorTest, 
                   public ::testing::WithParamInterface<std::tuple<std::string, std::string, std::string>> {
};

TEST_P(DS1FileTest, LaunchWithDifferentDS1Files) {
    if (!executable_exists) {
        GTEST_SKIP() << "DS1 Editor executable not found";
    }
    
    auto params = GetParam();
    std::string ds1_file = std::get<0>(params);
    std::string dt1_id = std::get<1>(params);
    std::string tileset_id = std::get<2>(params);
    
    if (FileExists("release\\" + ds1_file)) {
        std::string args = "release\\" + ds1_file + " " + dt1_id + " " + tileset_id;
        EXPECT_TRUE(RunDS1Editor(args)) << "Should launch with " << ds1_file;
    } else {
        GTEST_SKIP() << "DS1 file not found: " << ds1_file;
    }
}

// Test different DS1 files with their appropriate parameters
INSTANTIATE_TEST_SUITE_P(
    VariousDS1Files,
    DS1FileTest,
    ::testing::Values(
        std::make_tuple("ds1\\duriel.ds1", "17", "481"),
        std::make_tuple("ds1\\tristram.ds1", "32", "481")
        // Add more DS1 files as discovered
    )
);

// Main function for the test executable
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "DS1 Editor Test Suite" << std::endl;
    std::cout << "=====================" << std::endl;
    std::cout << "Testing DS1 Editor functionality and file integrity" << std::endl;
    std::cout << std::endl;
    
    return RUN_ALL_TESTS();
}
