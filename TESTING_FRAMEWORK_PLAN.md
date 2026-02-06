# Testing Framework Setup for DS1 Editor

## Recommended Testing Strategy

### 1. C++ Unit Tests: Google Test Framework

**Installation via vcpkg:**
```bash
vcpkg install gtest:x86-windows
vcpkg install gmock:x86-windows
```

**Or via NuGet:**
```xml
<PackageReference Include="Microsoft.googletest.v140.windesktop.msvcstl.static.rt-dyn" Version="1.8.1.7" />
```

**Benefits:**
- Excellent MSVC integration
- Rich assertion macros (EXPECT_EQ, ASSERT_TRUE, etc.)
- Test fixtures and parameterized tests
- Death tests for error conditions
- Built-in test discovery and filtering

**Example Test Structure:**
```cpp
// tests/unit/test_ds1_misc.cpp
#include <gtest/gtest.h>
#include "../../ds1misc.h"

class DS1MiscTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup test data
    }
    void TearDown() override {
        // Cleanup
    }
};

TEST_F(DS1MiscTest, LoadDS1File) {
    // Test DS1 file loading
    EXPECT_TRUE(ds1_load_file("test.ds1"));
}
```

### 2. System/Integration Tests: Current NUnit Setup

**Keep the existing NUnit framework for:**
- Executable integration tests
- File I/O validation
- Command-line parameter testing
- Image comparison tests
- End-to-end workflow testing

### 3. Functional Tests: Custom Test Harness

**For Allegro/Graphics testing:**
- Custom test framework that can mock Allegro functions
- Image comparison utilities
- Performance benchmarks

## Implementation Plan

### Phase 1: Setup Google Test
1. Install Google Test via vcpkg or NuGet
2. Create CMakeLists.txt or update MSVC project
3. Setup basic test structure

### Phase 2: Create Unit Tests
1. Test core algorithms (ds1misc, dt1misc, etc.)
2. Test data structures and file parsing
3. Test utility functions

### Phase 3: Integration Tests
1. Enhance existing NUnit tests
2. Add more comprehensive executable tests
3. Add graphics/rendering validation

### Phase 4: Continuous Integration
1. Setup automated test runs
2. Add test coverage reporting
3. Performance regression testing
