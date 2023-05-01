#include "gtest/gtest.h"
#include "config_parser.h"
#include <iostream>
#include <string>
#include "gmock/gmock.h"

class NginxConfigParserTest : public ::testing::Test{
  protected:
    NginxConfigParserTest() {}

    NginxConfigParser parser;
    NginxConfig out_config;

    bool ParseString(std::string config_str){
      std::istringstream config(config_str);
      return parser.Parse(dynamic_cast<std::istream*>(&config), &out_config);
    }
    bool ParseFile(const char* file_name){
      return parser.Parse(file_name, &out_config);;
    }

};

TEST_F(NginxConfigParserTest, SimpleValidConfig) {
  EXPECT_TRUE(ParseFile("./test_configs/example_config"));
}


TEST_F(NginxConfigParserTest, BadQuotation1) {
  EXPECT_FALSE(ParseString("foo\"bar\";\n"));
}

TEST_F(NginxConfigParserTest, BadQuotation2) {
  EXPECT_FALSE(ParseString("\"foo\"bar;\n")); // "foo"bar;
}


TEST_F(NginxConfigParserTest, EscapeCharValid){
  EXPECT_TRUE(ParseString("foo \"bar\\\"\";\n")); // foo "bar\"";
  EXPECT_TRUE(ParseString("foo \"bar\\\'\";\n")); // foo "bar\'";
  EXPECT_TRUE(ParseString("foo \"bar\\{\";\n")); // foo "bar\{";
  EXPECT_TRUE(ParseString("foo \"bar\\}\";\n")); // foo "bar\}";
  EXPECT_TRUE(ParseString("foo \"bar\\;\";\n")); // foo "bar\;";
  EXPECT_TRUE(ParseString("foo \"bar\\ \";\n")); // foo "bar\ ";
  EXPECT_TRUE(ParseString("foo \"bar\\\\\";\n")); // foo "bar\\";
}


TEST_F(NginxConfigParserTest, EscapeCharNotValid){
  EXPECT_FALSE(ParseString("foo \"bar\\d\";\n")); // foo "bar\d";
  EXPECT_FALSE(ParseString("foo \"bar\\;\"\n")); // foo "bar\";
}

TEST_F(NginxConfigParserTest, NonExistentConfig){
  EXPECT_FALSE(ParseFile("./test_configs/non_existent_config"));
}

TEST_F(NginxConfigParserTest, ConfigWithComment){
  EXPECT_TRUE(ParseString("foo \"bar\\\"\"; # this is a comment. \n")); // foo "bar\"";
  EXPECT_TRUE(ParseString("foo \"bar\\\'\";\n # this is a comment. \n")); // foo "bar\'";
}

TEST_F(NginxConfigParserTest, ConfigWithSingleQuote){
  EXPECT_TRUE(ParseString("foo \'bar\\\"\'; \n")); // foo 'bar\';
  EXPECT_TRUE(ParseString("foo \'bar\\\'\'; \n")); // foo 'bar\'';
}

using ::testing::SizeIs;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;

TEST_F(NginxConfigParserTest, ConfigStartingWithComment){
  EXPECT_TRUE(ParseString("# This is a comment.\nfoo \"bar\"; \n")); // foo "bar";
  EXPECT_THAT(out_config.statements_, SizeIs(1));
  EXPECT_THAT(out_config.statements_[0]->tokens_, ElementsAreArray({std::string("foo"), std::string("\"bar\"")}));
}

// NginxConfigStatement::ToString() correctly prints out the statement
TEST(NginxConfigStatementTest, ToString) {
  NginxConfigStatement statement;

  statement.tokens_.push_back("foo");
  statement.tokens_.push_back("bar");

  EXPECT_EQ("foo bar;\n", statement.ToString(0));
} 

TEST(NginxConfigStatementTest, ToStringWithBlock) {
  NginxConfigStatement statement;

  statement.tokens_.push_back("server");

  NginxConfig* child_config = new NginxConfig();
  child_config->statements_.emplace_back(new NginxConfigStatement);
  child_config->statements_.back().get()->tokens_.push_back("listen");
  child_config->statements_.back().get()->tokens_.push_back("8080");

  statement.child_block_ = std::unique_ptr<NginxConfig>(child_config);

  EXPECT_EQ("server {\n  listen 8080;\n}\n", statement.ToString(0));
} 

// NginxConfig::ToString() correctly prints out the config
TEST(NginxConfigTest, ToStringWithBlock) {
  NginxConfig config;

  NginxConfigStatement* statement1 = new NginxConfigStatement();
  statement1->tokens_.push_back("foo");
  statement1->tokens_.push_back("bar");
  
  NginxConfigStatement* statement2 = new NginxConfigStatement();
  statement2->tokens_.push_back("server");
  statement2->child_block_ = std::make_unique<NginxConfig>();
  
  statement2->child_block_.get()->statements_.emplace_back(new NginxConfigStatement);
  statement2->child_block_.get()->statements_.back().get()->tokens_.push_back("listen");
  statement2->child_block_.get()->statements_.back().get()->tokens_.push_back("8080");

  config.statements_.emplace_back(std::shared_ptr<NginxConfigStatement>(statement1));
  config.statements_.emplace_back(std::shared_ptr<NginxConfigStatement>(statement2));

  EXPECT_EQ("foo bar;\nserver {\n  listen 8080;\n}\n", config.ToString(0));
}

// test escapes single quotes in single quoted strings
TEST_F(NginxConfigParserTest, SingleQuotes) {
  EXPECT_TRUE(ParseFile("./test_configs/escape_single_quotes_config"));
}

// test escapes backslashes in double quoted strings
TEST_F(NginxConfigParserTest, BackSlashes) {
  EXPECT_TRUE(ParseFile("./test_configs/escape_back_slashes_config"));
}


// The parser correctly parses a config file with multiple nested blocks
TEST_F(NginxConfigParserTest, MultipleNestedBlocks) {
  EXPECT_TRUE(ParseFile("./test_configs/multiple_nested_blocks_config"));
}

// The parser correctly parses a config file with multiple nested blocks and multiple statements
TEST_F(NginxConfigParserTest, MultipleNestedBlocksAndStatements) {
  EXPECT_TRUE(ParseFile("./test_configs/crossplane_config"));
}

// The parser correctly handles a config file with a missing end bracket
TEST_F(NginxConfigParserTest, InvalidConfig) {
  EXPECT_FALSE(ParseFile("./test_configs/crossplane_missing_end_bracket_config"));
}

// The parser correctly handles a config file with a missing start bracket
TEST_F(NginxConfigParserTest, InvalidConfig2) {
  EXPECT_FALSE(ParseFile("./test_configs/crossplane_missing_start_bracket_config"));
}

// The parser correctly handles a config file with a missing quotation mark
TEST_F(NginxConfigParserTest, InvalidConfig3) {
  EXPECT_FALSE(ParseFile("./test_configs/crossplane_missing_quotation_mark_config"));
}

// Test full config file
TEST_F(NginxConfigParserTest, FullConfig) {
  EXPECT_TRUE(ParseFile("./test_configs/full_example_config"));
}

// The parser correctly parses a config file with a empty block
TEST_F(NginxConfigParserTest, EmptyBlock) {
  EXPECT_TRUE(ParseFile("./test_configs/empty_block_config"));
}

