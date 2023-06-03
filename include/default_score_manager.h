#ifndef DEFAULT_SCORE_MANAGER_H
#define DEFAULT_SCORE_MANAGER_H

#include "score_manager.h"
#include <memory>
#include "file_system_io_interface.h"
#include <boost/thread/mutex.hpp>

// Use a csv file to record all the files.
// Use a json file to keep the current ranking as a source of truth.
// Should be a Singleton to ensure synchronization.
class DefaultScoreManager : public ScoreManager {
public:
    DefaultScoreManager(std::shared_ptr<FileSystemIOInterface> file_io,
                        const std::string &score_file,
                        const std::string &ranking_file,
                        const unsigned short &ranking_size)
        : file_io_(file_io), score_file_(score_file),
          ranking_file_(ranking_file), ranking_size_(ranking_size){}

    // Atomically push a new score, and update the ranking if needed.
    // Newer scores will have a higher precedence than older scores, if score values are equal.
    virtual bool new_score(Score new_score) override;

    // Get the current ranking
    virtual std::optional<std::vector<Score>> get_ranking() override;

private:
    bool push_ranking_non_atomic(const std::vector<Score> &score); // Helper function for new_score
    std::optional<std::vector<Score>> get_ranking_non_atomic(); // Helper function for get_ranking
    std::shared_ptr<FileSystemIOInterface> file_io_;
    const std::string score_file_; // csv file that records all the scores
    const std::string ranking_file_; // the json file that records the current ranking
    const unsigned short ranking_size_;

    boost::mutex ranking_lock_;

};

#endif