#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include <string>
#include "gmock/gmock.h"
#include "score_util.h"

class ScoreUtilTest : public ::testing::Test {
    protected:
        ScoreUtilTest(){}
        Json::Value json_value;
        std::string json_string;
        std::vector<Score> score_vector;

};

TEST_F(ScoreUtilTest, ScoresToJsonSimple){
    std::vector<Score> scores;
    scores.push_back({"user1", 100});
    scores.push_back({"user2", 200});
    scores.push_back({"user3", 300});
    json_value = score_util::ScoresToJson(scores, "scores");
    EXPECT_TRUE(json_value["scores"].isArray());
    EXPECT_EQ(json_value["scores"][0]["username"].asString(), "user1");
    EXPECT_EQ(json_value["scores"][1]["username"].asString(), "user2");
    EXPECT_EQ(json_value["scores"][2]["username"].asString(), "user3");
    EXPECT_EQ(json_value["scores"][0]["score"].asInt(), 100);
    EXPECT_EQ(json_value["scores"][1]["score"].asInt(), 200);
    EXPECT_EQ(json_value["scores"][2]["score"].asInt(), 300);
}

TEST_F(ScoreUtilTest, ScoresToJsonEmpty){
    std::vector<Score> scores;
    json_value = score_util::ScoresToJson(scores, "scores");
    EXPECT_TRUE(json_value["scores"].isArray());
    EXPECT_TRUE(json_value["scores"].empty());
}

using ::testing::SizeIs;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::AllOf;

TEST_F(ScoreUtilTest, GetScoresFromJsonSimple){
    json_string = "{\"scores\": [{\"username\": \"user1\", \"score\": 100, \"hash\": \"e163746f49c4625f298f95cd87a2f21b2e64c099e6a1108e2f5bdb1d604d135b\"}, {\"username\": \"user2\", \"score\": 200, \"hash\": \"78818b1b0b3fd1fbdb79710fafa4593fa16b49bfa5298ed6905d635f4e582980\"}] }";
    score_vector = score_util::GetScoresFromJson(json_string, "scores").value();

    EXPECT_THAT(score_vector,SizeIs(2));
    EXPECT_THAT(score_vector, ElementsAre(
        AllOf( Field(&Score::score, 100),
               Field(&Score::username, "user1")),
        AllOf( Field(&Score::score, 200),
               Field(&Score::username, "user2"))
    ));
}

TEST_F(ScoreUtilTest, GetScoresFromJsonEmpty){
    json_string = "{\"scores\": [] }";
    score_vector = score_util::GetScoresFromJson(json_string, "scores").value();

    EXPECT_THAT(score_vector,SizeIs(0));
}

// add a test for calculating the HMAC
TEST_F(ScoreUtilTest, CalculateHMAC){
    std::string hash = score_util::calculateHMAC("sudo-rm-rf-secret-key", "qianli51");
    EXPECT_EQ(hash, "8822fd007ba03e5912183307e41575122b0aeb68d220567e201dd63e2598830f");
}

