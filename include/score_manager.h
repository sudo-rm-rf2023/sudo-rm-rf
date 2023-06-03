#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

#include <optional>
#include <cstdlib>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "jsoncpp/json/json.h"
#include "score_util.h"

class ScoreManager {
public:
    // Register a new score.
    virtual bool new_score(Score new_score) = 0;

    virtual std::optional<std::vector<Score>> get_ranking() = 0;
};

#endif