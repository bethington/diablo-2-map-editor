#include <gtest/gtest.h>
#include <windows.h>
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>
#include <map>

// Test fixture for DS1 Editor file operations and data integrity
class DS1EditorFileTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test paths
        release_dir = "release";
        data_dir = release_dir + "\\Data";
        ds1_dir = release_dir + "\\ds1";
        config_file = release_dir + "\\ds1edit.ini";
        executable = release_dir + "\\ds1editor.exe";
        
        // Expected DS1 files based on batch files
        expected_ds1_files = {
            "duriel.ds1",
            "tristram.ds1"
        };
        
        // Expected configuration sections
        expected_config_sections = {
            "paths",
            "display",
            "editing"
        };
    }
    
    std::string release_dir;
    std::string data_dir;
    std::string ds1_dir;
    std::string config_file;
    std::string executable;
    std::vector<std::string> expected_ds1_files;
    std::vector<std::string> expected_config_sections;
    
    // Helper to check if directory exists and is accessible
    bool DirectoryExists(const std::string& path) {
        return std::filesystem::exists(path) && std::filesystem::is_directory(path);
    }
    
    // Helper to check file size and basic validation
    bool ValidateFileSize(const std::string& path, size_t min_size = 0) {
        if (!std::filesystem::exists(path)) return false;
        
        try {
            auto size = std::filesystem::file_size(path);
            return size >= min_size;
        } catch (...) {
            return false;
        }
    }
    
    // Helper to read file contents for basic validation
    std::string ReadFileContents(const std::string& path, size_t max_bytes = 1024) {
        std::ifstream file(path, std::ios::binary);
        if (!file.is_open()) return "";
        
        std::string content;
        content.resize(max_bytes);
        file.read(&content[0], max_bytes);
        content.resize(file.gcount());
        
        return content;
    }
    
    // Helper to validate DS1 file format (basic header check)
    bool ValidateDS1Format(const std::string& path) {
        std::string content = ReadFileContents(path, 64);
        if (content.length() < 4) return false;
        
        // DS1 files typically start with version info
        // This is a basic check - real validation would be more complex
        return content.length() > 10; // Minimum size check
    }
};

// Test 1: Release directory structure
TEST_F(DS1EditorFileTest, ReleaseDirectoryStructure) {
    EXPECT_TRUE(DirectoryExists(release_dir)) << "Release directory should exist";
    EXPECT_TRUE(DirectoryExists(data_dir)) << "Data directory should exist";
    EXPECT_TRUE(DirectoryExists(ds1_dir)) << "DS1 files directory should exist";
}

// Test 2: Essential files exist
TEST_F(DS1EditorFileTest, EssentialFilesExist) {
    EXPECT_TRUE(std::filesystem::exists(config_file)) << "Configuration file should exist";
    
    // Check for README
    EXPECT_TRUE(std::filesystem::exists(release_dir + "\\README FIRST.txt")) 
        << "README file should exist";
    
    // Check for batch files
    EXPECT_TRUE(std::filesystem::exists(release_dir + "\\Duriel.bat")) 
        << "Duriel batch file should exist";
    EXPECT_TRUE(std::filesystem::exists(release_dir + "\\Tristram.bat")) 
        << "Tristram batch file should exist";
}

// Test 3: DS1 files validation
TEST_F(DS1EditorFileTest, DS1FilesValidation) {
    for (const auto& ds1_file : expected_ds1_files) {
        std::string full_path = ds1_dir + "\\" + ds1_file;
        
        EXPECT_TRUE(std::filesystem::exists(full_path)) 
            << "DS1 file should exist: " << ds1_file;
        
        if (std::filesystem::exists(full_path)) {
            EXPECT_TRUE(ValidateFileSize(full_path, 100)) 
                << "DS1 file should have reasonable size: " << ds1_file;
            
            EXPECT_TRUE(ValidateDS1Format(full_path)) 
                << "DS1 file should have valid format: " << ds1_file;
        }
    }
}

// Test 4: Configuration file validation
TEST_F(DS1EditorFileTest, ConfigurationFileValidation) {
    if (!std::filesystem::exists(config_file)) {
        GTEST_SKIP() << "Configuration file not found";
    }
    
    EXPECT_TRUE(ValidateFileSize(config_file, 10)) 
        << "Configuration file should not be empty";
    
    // Read and validate basic INI structure
    std::ifstream config(config_file);
    std::string line;
    bool has_section = false;
    
    while (std::getline(config, line)) {
        // Look for INI section headers [section]
        if (line.length() > 2 && line[0] == '[' && line.back() == ']') {
            has_section = true;
            break;
        }
    }
    
    EXPECT_TRUE(has_section) << "Configuration file should have INI format sections";
}

// Test 5: Data directory contents
TEST_F(DS1EditorFileTest, DataDirectoryContents) {
    if (!DirectoryExists(data_dir)) {
        GTEST_SKIP() << "Data directory not found";
    }
    
    // Check for common Diablo 2 data files
    std::vector<std::string> expected_data_files = {
        "LvlTypes.txt",
        "Objects.txt",
        "LvlPrest.txt"
    };
    
    for (const auto& data_file : expected_data_files) {
        std::string full_path = data_dir + "\\" + data_file;
        if (std::filesystem::exists(full_path)) {
            EXPECT_TRUE(ValidateFileSize(full_path, 50)) 
                << "Data file should not be empty: " << data_file;
        }
        // Note: Not all data files may be present, so we don't fail if missing
    }
}

// Test 6: Batch file validation
TEST_F(DS1EditorFileTest, BatchFileValidation) {
    std::vector<std::string> batch_files = {
        "Duriel.bat",
        "Tristram.bat",
        "_Multiple ds1 demo.bat"
    };
    
    for (const auto& batch_file : batch_files) {
        std::string full_path = release_dir + "\\" + batch_file;
        
        if (std::filesystem::exists(full_path)) {
            EXPECT_TRUE(ValidateFileSize(full_path, 10)) 
                << "Batch file should not be empty: " << batch_file;
            
            // Read and check for ds1editor.exe reference
            std::string content = ReadFileContents(full_path, 500);
            EXPECT_TRUE(content.find("ds1editor.exe") != std::string::npos) 
                << "Batch file should reference ds1editor.exe: " << batch_file;
        }
    }
}

// Test 7: File permissions and accessibility
TEST_F(DS1EditorFileTest, FilePermissionsAndAccessibility) {
    // Test if we can read essential files
    if (std::filesystem::exists(config_file)) {
        std::ifstream test_read(config_file);
        EXPECT_TRUE(test_read.is_open()) << "Should be able to read configuration file";
    }
    
    // Test DS1 directory accessibility
    if (DirectoryExists(ds1_dir)) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(ds1_dir)) {
                EXPECT_TRUE(entry.exists()) << "Directory entry should be accessible";
            }
        } catch (const std::exception& e) {
            FAIL() << "Directory should be accessible: " << e.what();
        }
    }
}

// Test 8: File encoding and format consistency
TEST_F(DS1EditorFileTest, FileEncodingAndFormat) {
    // Test configuration file encoding
    if (std::filesystem::exists(config_file)) {
        std::string content = ReadFileContents(config_file);
        
        // Should not contain null bytes (binary data)
        EXPECT_EQ(content.find('\0'), std::string::npos) 
            << "Configuration file should be text-based";
        
        // Should contain typical INI file characters
        bool has_ini_chars = (content.find('=') != std::string::npos || 
                             content.find('[') != std::string::npos);
        EXPECT_TRUE(has_ini_chars) << "Configuration file should have INI format";
    }
}

// Test 9: Backup and temporary file handling
TEST_F(DS1EditorFileTest, BackupAndTemporaryFiles) {
    // Check if there are any leftover temporary files
    if (DirectoryExists(release_dir)) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(release_dir)) {
                std::string filename = entry.path().filename().string();
                
                // Warn about potential temporary files
                if (filename.find(".tmp") != std::string::npos || 
                    filename.find("~") != std::string::npos) {
                    std::cout << "Warning: Found potential temporary file: " << filename << std::endl;
                }
            }
        } catch (...) {
            // Directory iteration failed, but that's not a test failure
        }
    }
}

// Test 10: File integrity and corruption detection
TEST_F(DS1EditorFileTest, FileIntegrityCheck) {
    // Check executable integrity (if it exists)
    if (std::filesystem::exists(executable)) {
        EXPECT_TRUE(ValidateFileSize(executable, 50000)) 
            << "Executable should have reasonable size";
        
        // Check for PE header (Windows executable)
        std::string content = ReadFileContents(executable, 100);
        if (content.length() >= 2) {
            EXPECT_EQ(content.substr(0, 2), "MZ") 
                << "Executable should have valid PE header";
        }
    }
    
    // Check DS1 files for basic integrity
    for (const auto& ds1_file : expected_ds1_files) {
        std::string full_path = ds1_dir + "\\" + ds1_file;
        
        if (std::filesystem::exists(full_path)) {
            std::string content = ReadFileContents(full_path, 20);
            
            // DS1 files should not be empty or contain only null bytes
            bool has_content = false;
            for (char c : content) {
                if (c != '\0') {
                    has_content = true;
                    break;
                }
            }
            EXPECT_TRUE(has_content) 
                << "DS1 file should contain non-null data: " << ds1_file;
        }
    }
}

// Parameterized test for different DS1 files
class DS1FileValidationTest : public DS1EditorFileTest, 
                             public ::testing::WithParamInterface<std::string> {
};

TEST_P(DS1FileValidationTest, IndividualDS1FileValidation) {
    std::string ds1_file = GetParam();
    std::string full_path = ds1_dir + "\\" + ds1_file;
    
    if (std::filesystem::exists(full_path)) {
        EXPECT_TRUE(ValidateFileSize(full_path, 100)) 
            << "DS1 file should have minimum size: " << ds1_file;
        
        EXPECT_TRUE(ValidateDS1Format(full_path)) 
            << "DS1 file should have valid format: " << ds1_file;
        
        // Check file is readable
        std::ifstream file(full_path, std::ios::binary);
        EXPECT_TRUE(file.is_open()) 
            << "DS1 file should be readable: " << ds1_file;
    } else {
        GTEST_SKIP() << "DS1 file not found: " << ds1_file;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllDS1Files,
    DS1FileValidationTest,
    ::testing::ValuesIn(std::vector<std::string>{"duriel.ds1", "tristram.ds1"})
);

// Main test runner
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "DS1 Editor File Operations Test Suite" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "Testing file integrity, directory structure, and data validation" << std::endl;
    std::cout << std::endl;
    
    return RUN_ALL_TESTS();
}
