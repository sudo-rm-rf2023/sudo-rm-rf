#ifndef SCORE_UTIL_H
#define SCORE_UTIL_H

#include "jsoncpp/json/json.h"
#include <optional>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <string>
#include <iomanip>

struct Score {
    std::string username; // Used as key
    int score;
};

namespace score_util { 
bool CompareScore(const Score &a, const Score &b);
Json::Value ScoreToJsonObject(const Score &score);
Json::Value ScoresToJson(const std::vector<Score> &scores, const std::string &key);
std::optional<Score> ValidateAndGetJsonScoreObject(Json::Value score_json, bool verify_hmac=false);
std::optional<std::vector<Score>> GetScoresFromJson(std::string json_str, std::string key);
std::string calculateHMAC(std::string key, std::string data);
}

#endif