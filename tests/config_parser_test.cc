#include "gtest/gtest.h"
#include "config_parser.h"
#include <iostream>
#include <string>

class NginxConfigParserTest : public ::testing::Test{
  protected:
    NginxConfigParserTest() {}

    NginxConfigParser parser;
    NginxConfig out_config;

};

TEST_F(NginxConfigParserTest, SimpleValidConfig) {
  EXPECT_TRUE(parser.Parse("example_config", &out_config));
}


TEST_F(NginxConfigParserTest, BadQuotation1) {
  std::string config_str = "foo\"bar\";\n"; // foo"bar";
  std::istringstream config(config_str);

  EXPECT_FALSE(parser.Parse(dynamic_cast<std::istream*>(&config), &out_config));
}

TEST_F(NginxConfigParserTest, BadQuotation2) {
  std::string config_str = "\"foo\"bar;\n"; // "foo"bar;
  std::istringstream config(config_str);

  EXPECT_FALSE(parser.Parse(dynamic_cast<std::istream*>(&config), &out_config));
}

TEST_F(NginxConfigParserTest, EscapeCharValid){
  std::string config_str1 = "foo \"bar\\\"\";\n"; // foo "bar\"";
  std::string config_str2 = "foo \"bar\\\'\";\n"; // foo "bar\'";
  std::string config_str3 = "foo \"bar\\{\";\n"; // foo "bar\{";
  std::string config_str4 = "foo \"bar\\}\";\n"; // foo "bar\}";
  std::string config_str5 = "foo \"bar\\;\";\n"; // foo "bar\;";
  std::string config_str6 = "foo \"bar\\ \";\n"; // foo "bar\ ";
  std::string config_str7 = "foo \"bar\\\\\";\n"; // foo "bar\\";

  std::istringstream config1(config_str1);
  std::istringstream config2(config_str2);
  std::istringstream config3(config_str3);
  std::istringstream config4(config_str4);
  std::istringstream config5(config_str5);
  std::istringstream config6(config_str6);
  std::istringstream config7(config_str7);

  EXPECT_TRUE(parser.Parse(dynamic_cast<std::istream*>(&config1), &out_config));
  EXPECT_TRUE(parser.Parse(dynamic_cast<std::istream*>(&config2), &out_config));
  EXPECT_TRUE(parser.Parse(dynamic_cast<std::istream*>(&config3), &out_config));
  EXPECT_TRUE(parser.Parse(dynamic_cast<std::istream*>(&config4), &out_config));
  EXPECT_TRUE(parser.Parse(dynamic_cast<std::istream*>(&config5), &out_config));
  EXPECT_TRUE(parser.Parse(dynamic_cast<std::istream*>(&config6), &out_config));
  EXPECT_TRUE(parser.Parse(dynamic_cast<std::istream*>(&config7), &out_config));
}


TEST_F(NginxConfigParserTest, EscapeCharNotValid){
  std::string config_str1 = "foo \"bar\\d\";\n"; // foo "bar\d";
  std::string config_str2 = "foo \"bar\\;\"\n"; // foo "bar\";

  std::istringstream config1(config_str1);
  std::istringstream config2(config_str2);

  EXPECT_FALSE(parser.Parse(dynamic_cast<std::istream*>(&config1), &out_config));
  EXPECT_FALSE(parser.Parse(dynamic_cast<std::istream*>(&config2), &out_config));
}
