#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include <string>
#include "config_manager.h"
#include "gmock/gmock.h"

class ConfigManagerTest : public ::testing::Test {
    protected:
        ConfigManagerTest(){}
        ~ConfigManagerTest(){
            delete manager;
        }

        ConfigManager *manager;
};

TEST_F(ConfigManagerTest, ReadPortNumber){
    manager = ConfigManager::makeConfigManager("./test_configs/example_config");
    ASSERT_NE(manager, nullptr);
    EXPECT_EQ(manager->port(), 80);
}

TEST_F(ConfigManagerTest, ReadPortNumberFalse){
    manager = ConfigManager::makeConfigManager("./test_configs/empty_config");
    EXPECT_EQ(manager, nullptr);
}

TEST_F(ConfigManagerTest, InvalidListen1){
    manager = ConfigManager::makeConfigManager("./test_configs/invalid_listen_config1");
    EXPECT_EQ(manager, nullptr);
}

TEST_F(ConfigManagerTest, InvalidListen2){
    manager = ConfigManager::makeConfigManager("./test_configs/invalid_listen_config2");
    EXPECT_EQ(manager, nullptr);
}

using ::testing::SizeIs;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::AllOf;


TEST_F(ConfigManagerTest, ValidRouterEntries){
    manager = ConfigManager::makeConfigManager("./test_configs/assign4_full_fledged_config");
    ASSERT_NE(manager, nullptr);
    EXPECT_EQ(manager->port(), 9000);
    std::vector<RouterEntry> router_entries = manager->getRouterEntries();

    EXPECT_THAT(router_entries, SizeIs(3));
    EXPECT_THAT(router_entries,ElementsAre(
    AllOf(  Field(&RouterEntry::request_path, "/"),
            Field(&RouterEntry::base_dir, "/data"),
            Field(&RouterEntry::handler_type,STATIC_HANDLER)),
    AllOf(
            Field(&RouterEntry::request_path, "/echo"),
            Field(&RouterEntry::handler_type,ECHO_HANDLER)),
    AllOf(
            Field(&RouterEntry::request_path, "/webpage"),
            Field(&RouterEntry::base_dir, "/data/www"),
            Field(&RouterEntry::handler_type,STATIC_HANDLER))
    ));
}

TEST_F(ConfigManagerTest, ValidRouterEntriesWithRootOnly){
    manager = ConfigManager::makeConfigManager("./test_configs/assign4_full_fledged_config_concise");
    ASSERT_NE(manager, nullptr);
    EXPECT_EQ(manager->port(), 9000);
    std::vector<RouterEntry> router_entries = manager->getRouterEntries();

    EXPECT_THAT(router_entries, SizeIs(3));
    EXPECT_THAT(router_entries,ElementsAre(
    AllOf(  Field(&RouterEntry::request_path, "/static1"),
            Field(&RouterEntry::base_dir, "/data"),
            Field(&RouterEntry::handler_type,STATIC_HANDLER)),
    AllOf(
            Field(&RouterEntry::request_path, "/echo"),
            Field(&RouterEntry::handler_type,ECHO_HANDLER)),
    AllOf(
            Field(&RouterEntry::request_path, "/webpage"),
            Field(&RouterEntry::base_dir, "/webpages"),
            Field(&RouterEntry::handler_type,STATIC_HANDLER))
    ));
}

TEST_F(ConfigManagerTest, InvalidLocationBlock){
    manager = ConfigManager::makeConfigManager("./test_configs/invalidLocationBlock.conf");
    EXPECT_EQ(manager, nullptr);
}

TEST_F(ConfigManagerTest, InvalidRouterEntriesDuplicateRoot){
    manager = ConfigManager::makeConfigManager("./test_configs/InvalidRouterEntriesDuplicateRoot.conf");
    EXPECT_EQ(manager, nullptr);
}

TEST_F(ConfigManagerTest, InvalidRouterEntriesDuplicateServeType){
    manager = ConfigManager::makeConfigManager("./test_configs/InvalidRouterEntriesDuplicateServeType.conf");
    EXPECT_EQ(manager, nullptr);
}

TEST_F(ConfigManagerTest, UnknownKeywordInLocationBlock){
    manager = ConfigManager::makeConfigManager("./test_configs/UnknownKeywordInLocationBlock.conf");
    EXPECT_EQ(manager, nullptr);
}

TEST_F(ConfigManagerTest, DuplicateListenInServerBlock){
    manager = ConfigManager::makeConfigManager("./test_configs/DuplicateListenInServerBlock.conf");
    EXPECT_EQ(manager, nullptr);
}