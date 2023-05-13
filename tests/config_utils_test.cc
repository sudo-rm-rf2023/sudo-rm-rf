#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include <string>
#include "config_utils.h"
#include "gmock/gmock.h"
#include "config_parser.h"

class ConfigUtilsTest : public ::testing::Test {
    protected:
        ConfigUtilsTest(){}

        NginxConfigParser parser;
        NginxConfig config;

        bool ParseFile(const char* file_name){
            return parser.Parse(file_name, &config);
        }
};

TEST_F(ConfigUtilsTest, GetPort){
    ASSERT_TRUE(ParseFile("./test_configs/new_configs/simple_port.conf"));
    EXPECT_EQ(config_util::getPortFromConfig(config).value(), 9090);
}

TEST_F(ConfigUtilsTest, GetPortInvalid){
    ASSERT_TRUE(ParseFile("./test_configs/new_configs/simple_port_invalid.conf"));
    EXPECT_EQ(config_util::getPortFromConfig(config), std::nullopt);
}

TEST_F(ConfigUtilsTest, GetPortWithNoListenStatement){
    ASSERT_TRUE(ParseFile("./test_configs/new_configs/no_listen_statement.conf"));
    EXPECT_EQ(config_util::getPortFromConfig(config), std::nullopt);
}

TEST_F(ConfigUtilsTest, GetBaseDir){
    ASSERT_TRUE(ParseFile("./test_configs/new_configs/valid_location_block.conf"));
    EXPECT_EQ(config_util::getBaseDirFromLocationConfig(config).value(), "./cs130/secret/exam");
}

TEST_F(ConfigUtilsTest, GetBaseDirInvalid){
    ASSERT_TRUE(ParseFile("./test_configs/new_configs/invalid_location_block.conf"));
    EXPECT_EQ(config_util::getBaseDirFromLocationConfig(config), std::nullopt);
}


using ::testing::SizeIs;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::AllOf;

TEST_F(ConfigUtilsTest, GetDispatcherEntries){
    ASSERT_TRUE(ParseFile("./test_configs/new_configs/valid_config.conf"));
    ASSERT_EQ(config_util::getPortFromConfig(config).value(), 8080);
    ASSERT_NE(config_util::getDispatcherEntriesFromConfig(config), std::nullopt);
    std::vector<DispatcherEntry> entries = config_util::getDispatcherEntriesFromConfig(config).value();

    EXPECT_THAT(entries, SizeIs(4));
    EXPECT_THAT(entries, ElementsAre(
    AllOf(  Field(&DispatcherEntry::location, "/static1"),
            Field(&DispatcherEntry::handler_type, STATIC_HANDLER),
            Field(&DispatcherEntry::location_config, Field(&NginxConfig::statements_, SizeIs(1)))), // TODO: search for "root".
    AllOf(
            Field(&DispatcherEntry::location, "/echoecho"),
            Field(&DispatcherEntry::handler_type,ECHO_HANDLER),
            Field(&DispatcherEntry::location_config, Field(&NginxConfig::statements_, SizeIs(0)))),
    AllOf(
            Field(&DispatcherEntry::location, "/static2"),
            Field(&DispatcherEntry::handler_type, STATIC_HANDLER),
            Field(&DispatcherEntry::location_config, Field(&NginxConfig::statements_, SizeIs(1)))),
    AllOf(
            Field(&DispatcherEntry::location, "/"),
            Field(&DispatcherEntry::handler_type, NOTFOUND_HANDLER),
            Field(&DispatcherEntry::location_config, Field(&NginxConfig::statements_, SizeIs(0)))
    )
    ));
}

TEST_F(ConfigUtilsTest, EmptyDispatcherEntries){
    ASSERT_TRUE(ParseFile("./test_configs/new_configs/simple_port.conf"));
    ASSERT_EQ(config_util::getPortFromConfig(config).value(), 9090);
    ASSERT_NE(config_util::getDispatcherEntriesFromConfig(config), std::nullopt);
    std::vector<DispatcherEntry> entries = config_util::getDispatcherEntriesFromConfig(config).value();
    EXPECT_THAT(entries, SizeIs(0));
}

TEST_F(ConfigUtilsTest, GetDispatcherEntriesInvalid){
    ASSERT_TRUE(ParseFile("./test_configs/new_configs/invalid_config_unknown_handler.conf"));
    EXPECT_EQ(config_util::getDispatcherEntriesFromConfig(config), std::nullopt);
}




