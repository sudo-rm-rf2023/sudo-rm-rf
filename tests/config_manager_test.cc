#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include <string>
#include "config_manager.h"

class ConfigManagerTest : public ::testing::Test {
    protected:
        ConfigManagerTest(){}
        ~ConfigManagerTest(){
            if(config_file.is_open()){
                config_file.close();
            }
        }

        std::ifstream config_file;
};

TEST_F(ConfigManagerTest, ReadPortNumber){
    config_file.open("example_config");

    ConfigManager manager(dynamic_cast<std::istream*>(&config_file));
    EXPECT_EQ(manager.port(), 80);
    EXPECT_TRUE(manager.isValid());
}

TEST_F(ConfigManagerTest, ReadPortNumberFalse){
    config_file.open("empty_config");

    ConfigManager manager(dynamic_cast<std::istream*>(&config_file));
    EXPECT_FALSE(manager.isValid());
}

TEST_F(ConfigManagerTest, InvalidListen1){
    config_file.open("invalid_listen_config1");

    ConfigManager manager(dynamic_cast<std::istream*>(&config_file));
    EXPECT_FALSE(manager.isValid());
}

TEST_F(ConfigManagerTest, InvalidListen2){
    config_file.open("invalid_listen_config2");

    ConfigManager manager(dynamic_cast<std::istream*>(&config_file));
    EXPECT_FALSE(manager.isValid());
}
// TODO(yiyangzhou123): more test cases
