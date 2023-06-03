#include "logger.h"
#include "score_request_handler.h"
#include "score_util.h"
#include "config_utils.h"
#include "utils.h"
#include "jsoncpp/json/json.h"

namespace http = boost::beast::http;

ScoreRequestHandlerFactory::ScoreRequestHandlerFactory
    (const std::string &location, const NginxConfig &config_block)
    : RequestHandlerFactory(location, config_block) {
        const std::string score_storage_file =
            config_util::getScoreStorageFromLocationConfig(config_block_).value_or("./score_storage");
        const std::string leaderboard_file =
            config_util::getScoreStorageFromLocationConfig(config_block_).value_or("./leaderboard");
        const unsigned short leaderboard_size =
            config_util::getRankingSizeFromLocationConfig(config_block_).value_or(10); // Default to 10

        // Create a long-lived ScoreManager to ensure multithreading synchronization
        score_manager_ = std::make_shared<DefaultScoreManager>(std::make_shared<BoostFileSystemIO>(),
                                                        score_storage_file,
                                                        leaderboard_file,
                                                        leaderboard_size);
}

std::shared_ptr<RequestHandler> ScoreRequestHandlerFactory::create(){
    return std::make_shared<ScoreRequestHandler>(location_, config_block_, score_manager_);
}

status ScoreRequestHandler::handle_request
    (const http::request<http::string_body> &request,
                        http::response<http::string_body> &response) {
    std::string url = request.target().to_string();
    // TODO: refactor into util.h, along with the code in crud_api_handler.cc
    std::string url_minus_request_path;
    try {
        url_minus_request_path = url.substr(url.find(request_path_) + request_path_.size());
    } catch (...) {
        return handle_bad_request(response);
    }

    std::vector<std::string> url_tokens = util::parseURL(url_minus_request_path);
    if(url_tokens.size() != 1){
        return handle_bad_request(response);
    }
    http::verb method = request.method();

    response.clear();
    response.version(11);

    // Either /newscore or /scoreboard
    if(url_tokens[0] == "newscore" && method == http::verb::post){ // POST /<request_path>/newscore

        BOOST_LOG_TRIVIAL(info) << "Received JSON: " << request.body();
        Json::Value score_json;
        Json::Reader reader;

        if(!reader.parse(request.body(), score_json,  /*collectComments=*/false)){
            BOOST_LOG_TRIVIAL(warning) << "Parser: Received invalid score in JSON format.";
            response.result(http::status::bad_request);
            response.body() = "Invalid Score in JSON format.";
            response.set(http::field::content_type, "text/plain");
            response.prepare_payload();
            return true;
        }

        std::optional<Score> new_score = score_util::ValidateAndGetJsonScoreObject(score_json, /*verify_hmac=*/true);
        if(!new_score.has_value()){
            // TODO: refactor repetitive code.
            BOOST_LOG_TRIVIAL(warning) << "Unable to convert Json to Score: Received invalid score in JSON format.";
            response.result(http::status::bad_request);
            response.body() = "Invalid Score in JSON format.";
            response.set(http::field::content_type, "text/plain");
            response.prepare_payload();
            return true;
        }

        if(!score_manager_->new_score(new_score.value())){
            response.result(http::status::internal_server_error);
            response.body() = "Failed to register the new score.";
            response.set(http::field::content_type, "text/plain");
            response.prepare_payload();
            return true;
        }

        BOOST_LOG_TRIVIAL(info) << "Registered new score: " << new_score->score;

        response.result(http::status::ok);
        response.prepare_payload();
        return true;

    } else if (url_tokens[0] == "scoreboard" && method == http::verb::get){ // GET /<request_path>/scoreboard
        std::optional<std::vector<Score>> scores = score_manager_->get_ranking();
        if(!scores.has_value()){
            response.result(http::status::internal_server_error);
            response.body() = "Unable to retrive the leaderboard.";
            response.set(http::field::content_type, "text/plain");
            response.prepare_payload();
            return true;
        }

        Json::Value scores_json = score_util::ScoresToJson(scores.value(), "ranking");
        Json::StreamWriterBuilder writer;
        response.body() = Json::writeString(writer, scores_json);
        response.result(http::status::ok);
        response.set(http::field::content_type, "application/json");
        response.prepare_payload();
        return true;
    } else {
        BOOST_LOG_TRIVIAL(warning) <<"Bad request in ScoreRequestHandler: Only POST <path>/newscore and GET <path>/scoreboard are supported.";
        return handle_bad_request(response);
    }

}