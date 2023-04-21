#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include <string>
#include "config_manager.h"

class ConfigManagerTest : public ::testing::Test {
    protected:
        ConfigManagerTest(){}
        ~ConfigManagerTest(){
            delete manager;
        }

        ConfigManager *manager;
};

TEST_F(ConfigManagerTest, ReadPortNumber){
    manager = ConfigManager::makeConfigManager("example_config");
    ASSERT_NE(manager, nullptr);
    EXPECT_EQ(manager->port(), 80);
}

TEST_F(ConfigManagerTest, ReadPortNumberFalse){
    manager = ConfigManager::makeConfigManager("empty_config");
    EXPECT_EQ(manager, nullptr);
}

TEST_F(ConfigManagerTest, InvalidListen1){
    manager = ConfigManager::makeConfigManager("invalid_listen_config1");
    EXPECT_EQ(manager, nullptr);
}

TEST_F(ConfigManagerTest, InvalidListen2){
    manager = ConfigManager::makeConfigManager("invalid_listen_config2");
    EXPECT_EQ(manager, nullptr);
}
// TODO(yiyangzhou123): more test cases
