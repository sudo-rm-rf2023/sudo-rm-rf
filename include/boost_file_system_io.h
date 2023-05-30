#ifndef BOOST_FILE_SYSTEM_IO_H
#define BOOST_FILE_SYSTEM_IO_H

#include <boost/filesystem.hpp>
#include <sstream>
#include <vector>

#include "file_system_io_interface.h"
#include "logger.h"

namespace fs = boost::filesystem;

class BoostFileSystemIO : public FileSystemIOInterface {
 public:
  BoostFileSystemIO() {}

  std::optional<std::ostringstream> read_file(std::string file_path) override;

  bool write_file(std::string file_path,
                  const std::ostringstream &bytes) override;

  bool delete_file(std::string file_path) override;

  std::optional<std::vector<std::string>> ls(std::string dir_name) override;

};

#endif
