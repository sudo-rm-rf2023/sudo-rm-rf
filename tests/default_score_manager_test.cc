#include "gtest/gtest.h"
#include <iostream>
#include <fstream>
#include <string>
#include "gmock/gmock.h"
#include "default_score_manager.h"
#include "test_file_system_io.h"
#include "score_util.h"

class DefaultScoreManagerTest : public ::testing::Test {
    protected:
        DefaultScoreManagerTest(){}

        std::shared_ptr<FileSystemIOInterface> file_io =
                            std::make_shared<TestFileSystemIO>();
        std::string score_file = "/score_store/score";
        std::string ranking_file = "/score_store/ranking";
        DefaultScoreManager score_manager =  DefaultScoreManager(file_io,
                            score_file, ranking_file, /*ranking_size=*/3);;
        std::vector<Score> ranking;
};

using ::testing::SizeIs;
using ::testing::ElementsAre;
using ::testing::Field;
using ::testing::AllOf;

TEST_F(DefaultScoreManagerTest, EmptyGetRanking) {
    ranking = score_manager.get_ranking().value();
    EXPECT_THAT(ranking, SizeIs(0));
}

TEST_F(DefaultScoreManagerTest, AddingScore) {
    Score score1 = {.username = "user1", .score = 150};
    EXPECT_TRUE(score_manager.new_score(score1));
    ranking = score_manager.get_ranking().value();
    EXPECT_THAT(ranking, SizeIs(1));
    EXPECT_THAT(ranking, ElementsAre(
        AllOf(
            Field(&Score::username, "user1"),
            Field(&Score::score, 150)
        )
    ));

    Score score2 = {.username = "cs130", .score = 1000};
    EXPECT_TRUE(score_manager.new_score(score2));
    ranking = score_manager.get_ranking().value();
    EXPECT_THAT(ranking, SizeIs(2));
    EXPECT_THAT(ranking, ElementsAre(
        AllOf(
            Field(&Score::username, "cs130"),
            Field(&Score::score, 1000)
        ),
        AllOf(
            Field(&Score::username, "user1"),
            Field(&Score::score, 150)
        )
    ));
}

TEST_F(DefaultScoreManagerTest, ReachRankingSize) {
    Score score1 = {.username = "user1", .score = 250};
    Score score2 = {.username = "user2", .score = 390};
    Score score3 = {.username = "user3", .score = 360};
    EXPECT_TRUE(score_manager.new_score(score1));
    EXPECT_TRUE(score_manager.new_score(score2));
    EXPECT_TRUE(score_manager.new_score(score3));

    ranking = score_manager.get_ranking().value();

    EXPECT_THAT(ranking, SizeIs(3));
    EXPECT_THAT(ranking, ElementsAre(
        AllOf(
            Field(&Score::username, "user2"),
            Field(&Score::score, 390)
        ),
        AllOf(
            Field(&Score::username, "user3"),
            Field(&Score::score, 360)
        ),
        AllOf(
            Field(&Score::username, "user1"),
            Field(&Score::score, 250)
        )
    ));

    Score low_score = {.username = "Me", .score = 200};
    EXPECT_TRUE(score_manager.new_score(low_score));
    ranking = score_manager.get_ranking().value();

    EXPECT_THAT(ranking, SizeIs(3));
    EXPECT_THAT(ranking, ElementsAre(
        AllOf(
            Field(&Score::username, "user2"),
            Field(&Score::score, 390)
        ),
        AllOf(
            Field(&Score::username, "user3"),
            Field(&Score::score, 360)
        ),
        AllOf(
            Field(&Score::username, "user1"),
            Field(&Score::score, 250)
        )
    ));

    Score high_score = {.username = "Nachenberg", .score = 1000};
    EXPECT_TRUE(score_manager.new_score(high_score));
    ranking = score_manager.get_ranking().value();
    EXPECT_THAT(ranking, SizeIs(3));
    EXPECT_THAT(ranking, ElementsAre(
        AllOf(
            Field(&Score::username, "Nachenberg"),
            Field(&Score::score, 1000)
        ),
        AllOf(
            Field(&Score::username, "user2"),
            Field(&Score::score, 390)
        ),
        AllOf(
            Field(&Score::username, "user3"),
            Field(&Score::score, 360)
        )
    ));

    Score middle_score {.username = "Smallberg", .score = 375};
    EXPECT_TRUE(score_manager.new_score(middle_score));
    ranking = score_manager.get_ranking().value();
    EXPECT_THAT(ranking, SizeIs(3));
    EXPECT_THAT(ranking, ElementsAre(
        AllOf(
            Field(&Score::username, "Nachenberg"),
            Field(&Score::score, 1000)
        ),
        AllOf(
            Field(&Score::username, "user2"),
            Field(&Score::score, 390)
        ),
        AllOf(
            Field(&Score::username, "Smallberg"),
            Field(&Score::score, 375)
        )
    ));
}


TEST_F(DefaultScoreManagerTest, EqualScores) {
    Score score1 = {.username = "user1", .score = 250};
    Score score2 = {.username = "user2", .score = 390};
    Score score3 = {.username = "user3", .score = 360};
    EXPECT_TRUE(score_manager.new_score(score1));
    EXPECT_TRUE(score_manager.new_score(score2));
    EXPECT_TRUE(score_manager.new_score(score3));

    ranking = score_manager.get_ranking().value();

    EXPECT_THAT(ranking, SizeIs(3));
    EXPECT_THAT(ranking, ElementsAre(
        AllOf(
            Field(&Score::username, "user2"),
            Field(&Score::score, 390)
        ),
        AllOf(
            Field(&Score::username, "user3"),
            Field(&Score::score, 360)
        ),
        AllOf(
            Field(&Score::username, "user1"),
            Field(&Score::score, 250)
        )
    ));

    Score equal_to_score1 = {.username = "user1enemy", .score = 250};
    EXPECT_TRUE(score_manager.new_score(equal_to_score1));

    ranking = score_manager.get_ranking().value();

    EXPECT_THAT(ranking, SizeIs(3));
    EXPECT_THAT(ranking, ElementsAre(
        AllOf(
            Field(&Score::username, "user2"),
            Field(&Score::score, 390)
        ),
        AllOf(
            Field(&Score::username, "user3"),
            Field(&Score::score, 360)
        ),
        AllOf(
            Field(&Score::username, "user1enemy"),
            Field(&Score::score, 250)
        )
    ));

    Score equal_to_score2 = {.username = "user2enemy", .score = 390};
    EXPECT_TRUE(score_manager.new_score(equal_to_score2));

    ranking = score_manager.get_ranking().value();

    EXPECT_THAT(ranking, SizeIs(3));
    EXPECT_THAT(ranking, ElementsAre(
        AllOf(
            Field(&Score::username, "user2enemy"),
            Field(&Score::score, 390)
        ),
        AllOf(
            Field(&Score::username, "user2"),
            Field(&Score::score, 390)
        ),
        AllOf(
            Field(&Score::username, "user3"),
            Field(&Score::score, 360)
        )
    ));

}
