#include "score_util.h"
#include "logger.h"
#include <cstdlib>

namespace {
    std::string getEnvVar(const std::string &key, const std::string &defaultValue) {
        const char *value = std::getenv(key.c_str());
        return (value == nullptr) ? defaultValue : std::string(value);
    }
}

namespace score_util {
    // Comparator for Score
    bool CompareScore(const Score &a, const Score &b){
        return a.score >= b.score;
    }

    // Converts a Score to a Json document
    Json::Value ScoreToJsonObject(const Score &score){
        Json::Value root;
        root["username"] = score.username;
        root["score"] = score.score;
        return root;
    }

    // Converts a vector of Scores to a Json document with a list
    Json::Value ScoresToJson(const std::vector<Score> &scores, const std::string &key){
        Json::Value scores_json(Json::arrayValue);
        for (Score score : scores){
            scores_json.append(ScoreToJsonObject(score));
        }

        Json::Value root;
        root[key] = scores_json;
        return root;
    }

    // Input {"username":"...", "score":123}
    std::optional<Score> ValidateAndGetJsonScoreObject(Json::Value score_json, bool verify_hmac){
        Score score;
        if(score_json.isNull() ||!score_json["score"].isInt() || !score_json["username"].isString()){
            BOOST_LOG_TRIVIAL(error) << "score_json does not follow our format.";
            return std::nullopt;
        }
        score.score = score_json["score"].asInt();
        score.username = score_json["username"].asString();

        if(!verify_hmac){
            return score;
        }

        // verify HMAC
        try {
            std::string SECRET_KEY = getEnvVar("SECRET_KEY", "sudo-rm-rf-secret-key");
            if (calculateHMAC(SECRET_KEY, score.username + std::to_string(score.score)) != score_json["hash"].asString()){
                BOOST_LOG_TRIVIAL(error) << calculateHMAC(SECRET_KEY, score.username + std::to_string(score.score));
                BOOST_LOG_TRIVIAL(error) << "Invalid score: HMAC verification failed.";
                return std::nullopt;
            }
        } catch (std::exception& e) {
            BOOST_LOG_TRIVIAL(error) << "Invalid score: HMAC verification failed.";
            return std::nullopt;
        }

        return score;
    }


    // json_str: {"<key>": [...]}
    std::optional<std::vector<Score>> GetScoresFromJson(std::string json_str,
                                                        std::string key){
        Json::Value root;
        Json::Reader reader;

        if (!reader.parse(json_str, root, /*collectComments=*/false)){
            BOOST_LOG_TRIVIAL(error) << "Failed to parse the json string.";
            return std::nullopt;
        }

        Json::Value score_json_list = root[key];
        if(!score_json_list.isArray()){
            BOOST_LOG_TRIVIAL(error) << "The value is not a list.";
            return std::nullopt;
        }

        std::vector<Score> scores;
        for(Json::Value score_json : score_json_list){
            std::optional<Score> new_score = ValidateAndGetJsonScoreObject(score_json);
            if(!new_score.has_value()){
                BOOST_LOG_TRIVIAL(error) << "ValidateAndGetJsonScoreObject Failed.";
                return std::nullopt;
            }
            scores.push_back(*new_score);
        }
        return scores;
    }
    
    // calculate the expected HMAC using the same secret key
    // verify it against the HMAC received from the client.
    std::string calculateHMAC(std::string key, std::string data){
        unsigned char* digest;
        digest = HMAC(EVP_sha256(), key.c_str(), key.length(), (unsigned char*)data.c_str(), data.length(), NULL, NULL);
        std::stringstream ss;
        for(int i = 0; i < 32; i++){
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
        }
        return ss.str();
    }
}

