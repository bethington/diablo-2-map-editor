# Unit Test Templates for DS1 Editor

This folder contains unit test templates for the DS1 Editor components. These tests use Google Test framework and provide comprehensive coverage for the core functionality.

## Test Structure

### 1. Core Module Tests
- **test_ds1_misc.cpp** - Tests for DS1 file operations
- **test_dt1_misc.cpp** - Tests for DT1 tile file operations
- **test_edit_operations.cpp** - Tests for editing operations

### 2. Test Categories

#### Unit Tests
- Individual function testing
- Data structure validation
- Error handling verification

#### Performance Tests
- Loading time benchmarks
- Memory usage validation
- Operation efficiency tests

#### Mock Tests
- Allegro function mocking
- File I/O simulation
- User input simulation

## Running Tests

1. Build tests: `build_tests.bat`
2. Run tests: `run_tests.bat` or directly run executable
3. View results: Check console output or XML reports

## Test Data

Test files are stored in `test_data/` directory:
- Sample DS1 files
- Sample DT1 files
- Expected output files
- Error case files

## Writing New Tests

Follow these patterns when adding new tests:

```cpp
// Basic test
TEST(ComponentName, TestName) {
    // Arrange
    setup_test_data();
    
    // Act
    result = function_under_test();
    
    // Assert
    EXPECT_EQ(expected, result);
}

// Parameterized test
class ComponentTest : public ::testing::TestWithParam<TestCase> {};

TEST_P(ComponentTest, ParameterizedTest) {
    auto test_case = GetParam();
    EXPECT_EQ(test_case.expected, process(test_case.input));
}

// Performance test
TEST(ComponentName, PerformanceTest) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Operation to time
    perform_operation();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    EXPECT_LT(duration.count(), 1000); // Should complete in under 1 second
}
```

## Mock Framework

Use GMock for mocking external dependencies:

```cpp
class MockAllegro {
public:
    MOCK_METHOD2(allegro_load_bitmap, ALLEGRO_BITMAP*(const char*, int));
    MOCK_METHOD1(allegro_destroy_bitmap, void(ALLEGRO_BITMAP*));
};
```

## Test Configuration

Tests can be configured through:
- Command line parameters (`--gtest_filter`, `--gtest_repeat`)
- Environment variables
- Configuration files in test_data/

## Integration with CI

These tests are designed to be run in:
- Local development environment
- Continuous Integration pipelines
- Automated testing workflows

For CI integration, use XML output format for test reporting.
