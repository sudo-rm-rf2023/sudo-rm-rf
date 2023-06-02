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
    json_string = "{\"scores\": [{\"username\": \"user1\", \"score\": 100}, {\"username\": \"user2\", \"score\": 200}] }";
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

