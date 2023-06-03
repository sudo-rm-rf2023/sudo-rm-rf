#include "default_score_manager.h"
#include "jsoncpp/json/json.h"
#include "logger.h"

bool DefaultScoreManager::new_score(Score new_score){
    boost::lock_guard<boost::mutex> lock(ranking_lock_);  // ATOMIC

    std::optional<std::vector<Score>> ranking = get_ranking_non_atomic();
    if(!ranking.has_value()){
        return false;
    }

    // Insert the new score
    const std::vector<Score>::iterator pos = std::upper_bound(ranking->begin(), ranking->end(), new_score, score_util::CompareScore);
    if(ranking->size() == ranking_size_){ // If the ranking is full
        if(pos != ranking->end()){
            BOOST_LOG_TRIVIAL(trace) << "Insert " << new_score.score <<
                        " at position " << std::distance(ranking->begin(), pos);
            ranking->insert(pos, new_score);
            ranking->pop_back();
            return push_ranking_non_atomic(ranking.value());
        }
    }else if (ranking->size() < ranking_size_) { // If ranking is not full
        BOOST_LOG_TRIVIAL(trace) << "Insert " << new_score.score <<" at position " << std::distance(ranking->begin(), pos);
        ranking->insert(pos, new_score);
        return push_ranking_non_atomic(ranking.value());
    } else {
        BOOST_LOG_TRIVIAL(error) << "The actual ranking size is greater than intended. This should not happen.";
        return false;
    }

    return true;
}

std::optional<std::vector<Score>> DefaultScoreManager::get_ranking_non_atomic(){
    std::optional<std::ostringstream> ranking_stream = file_io_->read_file(ranking_file_);
    if(!ranking_stream.has_value()){
        // Check if it's because the file DNE; If so, push an empty ranking
        if (!file_io_->exists(ranking_file_)){
            push_ranking_non_atomic(std::vector<Score>());
            return std::vector<Score>();
        }
        BOOST_LOG_TRIVIAL(error) << "Unable to read the ranking from storage: " << ranking_file_;
        return std::nullopt;
    }
    std::string ranking_str = ranking_stream->str();
    return score_util::GetScoresFromJson(ranking_str, /*key=*/"ranking");
}

std::optional<std::vector<Score>> DefaultScoreManager::get_ranking(){
    boost::lock_guard<boost::mutex> lock(ranking_lock_);  // ATOMIC
    return get_ranking_non_atomic();
}

bool DefaultScoreManager::push_ranking_non_atomic(const std::vector<Score> &score){
    Json::Value ranking_json = score_util::ScoresToJson(score, /*key=*/"ranking");
    Json::StreamWriterBuilder writer;
    std::string ranking_str = Json::writeString(writer, ranking_json);
    std::ostringstream out(ranking_str);
    return file_io_->write_file(ranking_file_, out);
}
