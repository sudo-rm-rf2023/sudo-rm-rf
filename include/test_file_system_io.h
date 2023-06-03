#ifndef TEST_FILE_SYSTEM_IO_H
#define TEST_FILE_SYSTEM_IO_H

#include "file_system_io_interface.h"
#include <optional>
#include <string>
#include <filesystem>
#include <map>
#include <sstream>

class TestFileSystemIO : public FileSystemIOInterface {
 public:
  virtual std::optional<std::ostringstream> read_file(std::string file_path) override;

  virtual bool write_file(std::string file_path, const std::ostringstream& bytes) override;

  virtual bool delete_file(std::string file_path) override;

  virtual std::optional<std::vector<std::string>> ls(std::string dir_name) override;

  virtual bool exists(std::string file_path) override;

 private:
  std::map<std::string, std::string> file_system_;
};

#endif
