#include "boost_file_system_io.h"

std::optional<std::ostringstream> BoostFileSystemIO::read_file(
    std::string file_path) {
  fs::path boost_file_path(file_path);
  if (!fs::exists(boost_file_path) || !fs::is_regular_file(boost_file_path)) {
    BOOST_LOG_TRIVIAL(debug) << "Cannot open file: " << file_path
                             << ". Does not exist or non-regular";
    return std::nullopt;
  }

  fs::ifstream file_stream(boost_file_path, std::ios::binary);
  if (!file_stream.is_open()) {
    BOOST_LOG_TRIVIAL(debug)
        << "Cannot open file: " << file_path << " to read bytes from";
    return std::nullopt;
  }

  std::ostringstream bytes_read;
  bytes_read << file_stream.rdbuf();
  file_stream.close();
  return bytes_read;
}

bool BoostFileSystemIO::write_file(std::string file_path,
                                   const std::ostringstream &bytes) {
  fs::path boost_file_path(file_path);

  fs::create_directories(boost_file_path.parent_path());

  std::ios_base::openmode flags = std::ios::binary | std::ios::trunc;
  fs::ofstream file_stream(boost_file_path, flags);
  if (!file_stream.is_open()) {
    BOOST_LOG_TRIVIAL(debug)
        << "Cannot open file: " << file_path << " to write bytes ";
    return false;
  }
  file_stream << bytes.str();
  file_stream.close();
  return true;
}

bool BoostFileSystemIO::delete_file(std::string file_path) {
  fs::path boost_file_path(file_path);
  if (!fs::exists(boost_file_path)) {
    BOOST_LOG_TRIVIAL(debug)
        << "Cannot delete " << file_path << " that does not exist";
    return false;
  }

  try {
    fs::remove(boost_file_path);
  } catch (const fs::filesystem_error &ex) {
    BOOST_LOG_TRIVIAL(debug)
        << "Error occurred while deleting the file: " << ex.what();
    return false;
  }
  return true;
}

std::optional<std::vector<std::string>> BoostFileSystemIO::ls(
    std::string dir_name) {
  BOOST_LOG_TRIVIAL(trace) << "LS : " << dir_name;
  fs::path dir_path(dir_name);
  if (!fs::exists(dir_path) || !fs::is_directory(dir_path)) {
    BOOST_LOG_TRIVIAL(debug) << "Cannot list files in: " << dir_path
                             << ". Does not exist or not a directory";
    return std::nullopt;
  }

  fs::directory_iterator iter(dir_path);
  std::vector<std::string> file_names;
  for (; iter != fs::directory_iterator(); ++iter) {
    std::string file_name = iter->path().filename().string();
    file_names.push_back(file_name);
  }

  return file_names;
}
