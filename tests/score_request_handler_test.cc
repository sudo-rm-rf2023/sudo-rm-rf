#include "score_request_handler.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "jsoncpp/json/json.h"
#include "score_util.h"

namespace http = boost::beast::http; // from <boost/beast/http.hpp>

class MockScoreManager : public ScoreManager {
    public:
        MockScoreManager(){}
        MOCK_METHOD(bool , new_score, (Score), (override));
        MOCK_METHOD(std::optional<std::vector<Score>>, get_ranking, (), (override));
};

class ScoreRequestHandlerTest : public ::testing::Test {
    protected:
     ScoreRequestHandlerTest(){
        NginxConfig config;
        //config will not be in effect because we are mocking internal modules
        handler = std::make_shared<ScoreRequestHandler>(/*location=*/"/api", config, score_manager, /*verify_hmac=*/false);
     }

    std::shared_ptr<RequestHandler> handler;
    http::request<http::string_body> request;
    http::response<http::string_body> response;
    std::shared_ptr<MockScoreManager> score_manager = std::make_shared<MockScoreManager>();
};

std::string JsonToString(Json::Value json){
    Json::StreamWriterBuilder writer;
    return Json::writeString(writer, json);
}

bool CompareScoreVectors(std::vector<Score> a, std::vector<Score> b){
    if (a.size() != b.size()){
        return false;
    }

    for (unsigned int i = 0; i < a.size(); i++){
        if (a[i].score != b[i].score || a[i].username != b[i].username){
            return false;
        }
    }

    return true;
}

using ::testing::Return;

TEST_F(ScoreRequestHandlerTest, Newscore){
    request = {boost::beast::http::verb::post, /*target=*/"/api/newscore", /*version=*/11};
    request.body() = JsonToString(score_util::ScoreToJsonObject({.username = "testing1", .score = 100}));

    EXPECT_CALL(*score_manager, new_score).WillOnce(Return(true));
    EXPECT_TRUE(handler->handle_request(request, response));

    EXPECT_EQ(response.result(), http::status::ok);
}

TEST_F(ScoreRequestHandlerTest, Ranking){
    request = {boost::beast::http::verb::get, /*target=*/"/api/scoreboard", /*version=*/11};
    std::vector<Score> scores_to_return;
    scores_to_return.push_back({.username = "testing1", .score = 1000});
    scores_to_return.push_back({.username = "testing2", .score = 900});
    scores_to_return.push_back({.username = "testing3", .score = 800});
    scores_to_return.push_back({.username = "testing4", .score = 400});
    scores_to_return.push_back({.username = "testing5", .score = 100});

    EXPECT_CALL(*score_manager, get_ranking)
        .WillOnce(Return(std::optional<std::vector<Score>>(scores_to_return)));

    EXPECT_TRUE(handler->handle_request(request, response));
    EXPECT_EQ(response.result(), http::status::ok);
    std::string output_json = response.body();
    std::vector<Score> output_scores = score_util::GetScoresFromJson(output_json, "ranking").value();

    EXPECT_EQ(output_scores.size(), scores_to_return.size());
    EXPECT_TRUE(CompareScoreVectors(output_scores, scores_to_return));
}

TEST_F(ScoreRequestHandlerTest, EmptyRanking){
        request = {boost::beast::http::verb::get, /*target=*/"/api/scoreboard", /*version=*/11};
    std::vector<Score> scores_to_return;
    EXPECT_CALL(*score_manager, get_ranking)
        .WillOnce(Return(std::optional<std::vector<Score>>(scores_to_return)));

    EXPECT_TRUE(handler->handle_request(request, response));
    EXPECT_EQ(response.result(), http::status::ok);
    std::string output_json = response.body();
    std::vector<Score> output_scores = score_util::GetScoresFromJson(output_json, "ranking").value();
    EXPECT_EQ(output_scores.size(), 0);
}

TEST_F(ScoreRequestHandlerTest, NewScoreBadVerb){
    request = {boost::beast::http::verb::get, /*target=*/"/api/newscore", /*version=*/11};
    request.body() = JsonToString(score_util::ScoreToJsonObject({.username = "testing1", .score = 100}));

    EXPECT_TRUE(handler->handle_request(request, response));

    EXPECT_EQ(response.result(), http::status::bad_request);
}

TEST_F(ScoreRequestHandlerTest, NewScoreBadJSON){
    request = {boost::beast::http::verb::get, /*target=*/"/api/newscore", /*version=*/11};
    request.body() = "HALLOHALLO.";

    EXPECT_TRUE(handler->handle_request(request, response));

    EXPECT_EQ(response.result(), http::status::bad_request);
}

TEST_F(ScoreRequestHandlerTest, RankingBadVerb){
    request = {boost::beast::http::verb::post, /*target=*/"/api/scoreboard", /*version=*/11};

    EXPECT_TRUE(handler->handle_request(request, response));
    EXPECT_EQ(response.result(), http::status::bad_request);
    EXPECT_EQ(score_util::GetScoresFromJson(response.body(), /*key=*/"ranking"), std::nullopt);
}

TEST_F(ScoreRequestHandlerTest, UnknownURL){
    request = {boost::beast::http::verb::get, /*target=*/"/api/unknownurl", /*version=*/11};

    EXPECT_TRUE(handler->handle_request(request, response));
    EXPECT_EQ(response.result(), http::status::bad_request);
    EXPECT_EQ(score_util::GetScoresFromJson(response.body(), /*key=*/"ranking"), std::nullopt);
}
