#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include <string>
#include "utils.h"
#include "gmock/gmock.h"

class ConfigUtilsTest : public ::testing::Test {
    protected:
        ConfigUtilsTest(){}
        std::vector<std::string> parsed_token;
};

using ::testing::SizeIs;
using ::testing::ElementsAre;

TEST_F(ConfigUtilsTest, ParseURL1){
    parsed_token = util::parseURL("/hello/world!");
    EXPECT_THAT(parsed_token,SizeIs(2));
    EXPECT_THAT(parsed_token, ElementsAre("hello", "world!"));
}

TEST_F(ConfigUtilsTest, ParseURL2){
        parsed_token = util::parseURL("/hello/world/");
    EXPECT_THAT(parsed_token,SizeIs(2));
    EXPECT_THAT(parsed_token, ElementsAre("hello", "world"));
}

TEST_F(ConfigUtilsTest, ParseURL3){
    parsed_token = util::parseURL("//hello//world/");
    EXPECT_THAT(parsed_token,SizeIs(2));
    EXPECT_THAT(parsed_token, ElementsAre("hello", "world"));
}

TEST_F(ConfigUtilsTest, ParseEmptyURL){
    parsed_token = util::parseURL("");
    EXPECT_THAT(parsed_token,SizeIs(0));
}

TEST_F(ConfigUtilsTest, ParseEmptyURL2){
    parsed_token = util::parseURL("/");
    EXPECT_THAT(parsed_token,SizeIs(0));
}

TEST_F(ConfigUtilsTest, ParseURL4){
    parsed_token = util::parseURL("/hello/");
    EXPECT_THAT(parsed_token,SizeIs(1));
    EXPECT_THAT(parsed_token, ElementsAre("hello"));
}

