cmake_minimum_required(VERSION 3.16)

project(DS1EditorTests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download and build Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9fd7188081684e5e3a20a3e.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

# Test executables
add_executable(ds1editor_test ds1editor_test.cpp)
add_executable(ds1editor_keyboard_test ds1editor_keyboard_test.cpp)
add_executable(ds1editor_file_test ds1editor_file_test.cpp)

# Link libraries
target_link_libraries(ds1editor_test gtest_main)
target_link_libraries(ds1editor_keyboard_test gtest_main)
target_link_libraries(ds1editor_file_test gtest_main)

# Add tests to CTest
enable_testing()

add_test(NAME DS1EditorBasicTests COMMAND ds1editor_test)
add_test(NAME DS1EditorKeyboardTests COMMAND ds1editor_keyboard_test)
add_test(NAME DS1EditorFileTests COMMAND ds1editor_file_test)

# Set test properties
set_tests_properties(DS1EditorBasicTests PROPERTIES TIMEOUT 30)
set_tests_properties(DS1EditorKeyboardTests PROPERTIES TIMEOUT 10)
set_tests_properties(DS1EditorFileTests PROPERTIES TIMEOUT 20)

# Copy test data to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/../release DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
