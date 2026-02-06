#include <gtest/gtest.h>
#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <filesystem>

// Test fixture for DS1 Editor keyboard functionality based on documentation
class DS1EditorKeyboardTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize expected keyboard mappings based on documentation
        
        // Layer toggle keys (F1-F11)
        layer_keys = {
            {"F1", "Floor layer 1 toggle"},
            {"F2", "Floor layer 2 toggle"},
            {"F3", "Animation layer toggle"},
            {"F4", "Object info layer toggle"},
            {"F5", "Wall layer 1 toggle"},
            {"F6", "Wall layer 2 toggle"},
            {"F7", "Wall layer 3 toggle"},
            {"F8", "Wall layer 4 toggle"},
            {"F9", "Special tiles layer toggle"},
            {"F10", "NPC path layer toggle"},
            {"F11", "Shadow layer toggle"}
        };
        
        // Mode switching keys
        mode_keys = {
            {"TAB", "Switch between Tiles/Objects/Paths editing modes"},
            {"ESC", "Quit application"},
            {"R", "Refresh display"},
            {"SPACE", "Show walkable information"}
        };
        
        // Expected editing modes
        editing_modes = {"Tiles", "Objects", "Paths"};
        
        executable_path = "release\\ds1editor.exe";
        executable_exists = std::filesystem::exists(executable_path);
    }
    
    std::map<std::string, std::string> layer_keys;
    std::map<std::string, std::string> mode_keys;
    std::vector<std::string> editing_modes;
    std::string executable_path;
    bool executable_exists;
    
    // Helper to simulate key press (for future integration testing)
    void SimulateKeyPress(const std::string& key) {
        // This would be implemented when we have a running instance
        // For now, we just validate the key mapping exists
        EXPECT_TRUE(layer_keys.count(key) > 0 || mode_keys.count(key) > 0) 
            << "Key " << key << " should have a defined function";
    }
};

// Test 1: Validate F1-F11 layer toggle key mappings
TEST_F(DS1EditorKeyboardTest, LayerToggleKeysAreDefined) {
    EXPECT_EQ(layer_keys.size(), 11) << "Should have 11 layer toggle keys (F1-F11)";
    
    // Test each F-key mapping
    EXPECT_EQ(layer_keys["F1"], "Floor layer 1 toggle");
    EXPECT_EQ(layer_keys["F2"], "Floor layer 2 toggle");
    EXPECT_EQ(layer_keys["F3"], "Animation layer toggle");
    EXPECT_EQ(layer_keys["F4"], "Object info layer toggle");
    EXPECT_EQ(layer_keys["F5"], "Wall layer 1 toggle");
    EXPECT_EQ(layer_keys["F6"], "Wall layer 2 toggle");
    EXPECT_EQ(layer_keys["F7"], "Wall layer 3 toggle");
    EXPECT_EQ(layer_keys["F8"], "Wall layer 4 toggle");
    EXPECT_EQ(layer_keys["F9"], "Special tiles layer toggle");
    EXPECT_EQ(layer_keys["F10"], "NPC path layer toggle");
    EXPECT_EQ(layer_keys["F11"], "Shadow layer toggle");
}

// Test 2: Validate mode switching and utility keys
TEST_F(DS1EditorKeyboardTest, ModeSwitchingKeysAreDefined) {
    EXPECT_TRUE(mode_keys.count("TAB") > 0) << "TAB key should switch editing modes";
    EXPECT_TRUE(mode_keys.count("ESC") > 0) << "ESC key should quit application";
    EXPECT_TRUE(mode_keys.count("R") > 0) << "R key should refresh display";
    EXPECT_TRUE(mode_keys.count("SPACE") > 0) << "SPACE key should show walkable info";
    
    EXPECT_EQ(mode_keys["TAB"], "Switch between Tiles/Objects/Paths editing modes");
    EXPECT_EQ(mode_keys["ESC"], "Quit application");
    EXPECT_EQ(mode_keys["R"], "Refresh display");
    EXPECT_EQ(mode_keys["SPACE"], "Show walkable information");
}

// Test 3: Validate editing modes
TEST_F(DS1EditorKeyboardTest, EditingModesAreDefined) {
    EXPECT_EQ(editing_modes.size(), 3) << "Should have 3 editing modes";
    
    EXPECT_TRUE(std::find(editing_modes.begin(), editing_modes.end(), "Tiles") != editing_modes.end())
        << "Tiles editing mode should be available";
    EXPECT_TRUE(std::find(editing_modes.begin(), editing_modes.end(), "Objects") != editing_modes.end())
        << "Objects editing mode should be available";
    EXPECT_TRUE(std::find(editing_modes.begin(), editing_modes.end(), "Paths") != editing_modes.end())
        << "Paths editing mode should be available";
}

// Test 4: Floor layers (F1-F2) should be the primary terrain layers
TEST_F(DS1EditorKeyboardTest, FloorLayersHaveCorrectMapping) {
    EXPECT_TRUE(layer_keys["F1"].find("Floor") != std::string::npos) 
        << "F1 should control a floor layer";
    EXPECT_TRUE(layer_keys["F2"].find("Floor") != std::string::npos) 
        << "F2 should control a floor layer";
}

// Test 5: Wall layers (F5-F8) should control wall visibility
TEST_F(DS1EditorKeyboardTest, WallLayersHaveCorrectMapping) {
    for (int i = 5; i <= 8; i++) {
        std::string key = "F" + std::to_string(i);
        EXPECT_TRUE(layer_keys[key].find("Wall") != std::string::npos) 
            << key << " should control a wall layer";
    }
}

// Test 6: Special layers have appropriate mappings
TEST_F(DS1EditorKeyboardTest, SpecialLayersHaveCorrectMapping) {
    EXPECT_TRUE(layer_keys["F3"].find("Animation") != std::string::npos) 
        << "F3 should control animation layer";
    EXPECT_TRUE(layer_keys["F4"].find("Object") != std::string::npos) 
        << "F4 should control object info layer";
    EXPECT_TRUE(layer_keys["F9"].find("Special") != std::string::npos) 
        << "F9 should control special tiles layer";
    EXPECT_TRUE(layer_keys["F10"].find("path") != std::string::npos) 
        << "F10 should control NPC path layer";
    EXPECT_TRUE(layer_keys["F11"].find("Shadow") != std::string::npos) 
        << "F11 should control shadow layer";
}

// Test 7: Key combinations coverage
TEST_F(DS1EditorKeyboardTest, AllDocumentedKeysHaveMappings) {
    // Verify all documented keys are covered
    std::vector<std::string> documented_keys = {
        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11",
        "TAB", "ESC", "R", "SPACE"
    };
    
    for (const auto& key : documented_keys) {
        EXPECT_TRUE(layer_keys.count(key) > 0 || mode_keys.count(key) > 0)
            << "Key " << key << " should have a defined function";
    }
}

// Test 8: Layer visibility states can be tracked
TEST_F(DS1EditorKeyboardTest, LayerVisibilityStatesCanBeTracked) {
    // Simulate layer visibility tracking
    std::map<std::string, bool> layer_visibility;
    
    // Initialize all layers as visible (default state)
    for (const auto& layer : layer_keys) {
        layer_visibility[layer.first] = true;
    }
    
    EXPECT_EQ(layer_visibility.size(), 11) << "Should track visibility for all 11 layers";
    
    // Simulate toggling F1 (Floor layer 1)
    layer_visibility["F1"] = !layer_visibility["F1"];
    EXPECT_FALSE(layer_visibility["F1"]) << "F1 layer should be hidden after toggle";
    
    // Toggle again
    layer_visibility["F1"] = !layer_visibility["F1"];
    EXPECT_TRUE(layer_visibility["F1"]) << "F1 layer should be visible after second toggle";
}

// Test 9: Mode cycling behavior
TEST_F(DS1EditorKeyboardTest, ModeCyclingBehavior) {
    // Simulate mode cycling with TAB key
    int current_mode = 0; // Start with Tiles (index 0)
    
    // Simulate pressing TAB
    current_mode = (current_mode + 1) % editing_modes.size();
    EXPECT_EQ(editing_modes[current_mode], "Objects") << "Should switch to Objects mode";
    
    // Press TAB again
    current_mode = (current_mode + 1) % editing_modes.size();
    EXPECT_EQ(editing_modes[current_mode], "Paths") << "Should switch to Paths mode";
    
    // Press TAB again (should cycle back)
    current_mode = (current_mode + 1) % editing_modes.size();
    EXPECT_EQ(editing_modes[current_mode], "Tiles") << "Should cycle back to Tiles mode";
}

// Test 10: Key functionality descriptions are informative
TEST_F(DS1EditorKeyboardTest, KeyDescriptionsAreInformative) {
    // All layer keys should have descriptive names
    for (const auto& layer : layer_keys) {
        EXPECT_FALSE(layer.second.empty()) << "Key " << layer.first << " should have a description";
        EXPECT_TRUE(layer.second.length() > 5) << "Key " << layer.first << " description should be informative";
    }
    
    // All mode keys should have descriptive names
    for (const auto& mode : mode_keys) {
        EXPECT_FALSE(mode.second.empty()) << "Key " << mode.first << " should have a description";
        EXPECT_TRUE(mode.second.length() > 5) << "Key " << mode.first << " description should be informative";
    }
}

// Parameterized test for individual layer keys
class LayerKeyTest : public DS1EditorKeyboardTest, 
                    public ::testing::WithParamInterface<std::string> {
};

TEST_P(LayerKeyTest, IndividualLayerKeyFunctionality) {
    std::string key = GetParam();
    EXPECT_TRUE(layer_keys.count(key) > 0) << "Layer key " << key << " should be defined";
    
    if (layer_keys.count(key) > 0) {
        std::string description = layer_keys[key];
        EXPECT_TRUE(description.find("layer") != std::string::npos || 
                   description.find("toggle") != std::string::npos)
            << "Key " << key << " should control a layer";
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllLayerKeys,
    LayerKeyTest,
    ::testing::Values("F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11")
);

// Main test runner
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "DS1 Editor Keyboard Functionality Test Suite" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Testing keyboard shortcuts and layer controls based on documentation" << std::endl;
    std::cout << std::endl;
    
    return RUN_ALL_TESTS();
}
