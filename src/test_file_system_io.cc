#include "test_file_system_io.h"

std::optional<std::ostringstream> TestFileSystemIO::read_file(std::string file_path) {
    if (file_system_.find(file_path) == file_system_.end()) {
      return std::nullopt;
    }
    std::ostringstream bytes_read;
    bytes_read << file_system_[file_path];
    return bytes_read;
  }

  bool TestFileSystemIO::write_file(std::string file_path, const std::ostringstream& bytes) {
    if (file_system_.find(file_path) == file_system_.end()) {
      file_system_[file_path] = "";
    }

    file_system_[file_path] = bytes.str();
    return true;
  }

  bool TestFileSystemIO::delete_file(std::string file_path) {
    if (file_system_.find(file_path) == file_system_.end()) {
      return false;
    }
    file_system_.erase(file_path);
    return true;
  }

  std::optional<std::vector<std::string>> TestFileSystemIO::ls(std::string dir_name) {
    BOOST_LOG_TRIVIAL(trace) << "LS : " << dir_name;
    std::vector<std::string> result;
    bool found_entry = false;
    for (const auto& entry : file_system_) {
      std::string full_path = entry.first;
      if (dir_name.length() > full_path.length()) {
        continue;  // skip if there is no way to compare
      }
      bool file_in_dir = full_path.compare(0, dir_name.length(), dir_name) == 0;
      if (file_in_dir) {
        found_entry = true;
        std::string file_name = full_path.substr(dir_name.length());
        result.push_back(file_name);
      }
    }
    if (found_entry) {
      return result;
    } else {
      return std::nullopt;  // expected for file DNE
    }
  }

  bool TestFileSystemIO::exists(std::string file_path)
  {
    return file_system_.find(file_path) != file_system_.end();
  }
