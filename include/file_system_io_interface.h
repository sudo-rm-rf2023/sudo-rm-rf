#ifndef FILE_SYSTEM_IO_INTERFACE_H
#define FILE_SYSTEM_IO_INTERFACE_H

#include <sstream>
#include <vector>

#include "logger.h"

class FileSystemIOInterface {
 public:
  virtual std::optional<std::ostringstream> read_file(
      std::string file_path) = 0;
  virtual bool write_file(std::string file_path,
                          const std::ostringstream &bytes) = 0;
  virtual bool delete_file(std::string file_path) = 0;
  virtual std::optional<std::vector<std::string>> ls(std::string dir_name) = 0;
  virtual bool exists(std::string file_path) = 0;
};

#endif
