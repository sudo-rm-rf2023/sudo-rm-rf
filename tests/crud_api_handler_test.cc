#include "crud_api_handler.h"

#include <filesystem>
#include <map>
#include <sstream>

#include "gtest/gtest.h"
#include "request_handler.h"

namespace http = boost::beast::http;  // from <boost/beast/http.hpp>

class TestFileSystemIO : public FileSystemIOInterface {
 public:
  std::optional<std::ostringstream> read_file(std::string file_path) {
    if (file_system_.find(file_path) == file_system_.end()) {
      return std::nullopt;
    }

    std::ostringstream bytes_read;
    bytes_read << file_system_[file_path];
    return bytes_read;
  }

  bool write_file(std::string file_path, const std::ostringstream& bytes) {
    if (file_system_.find(file_path) == file_system_.end()) {
      file_system_[file_path] = "";
    }

    file_system_[file_path] = bytes.str();
    return true;
  }

  bool delete_file(std::string file_path) {
    if (file_system_.find(file_path) == file_system_.end()) {
      return false;
    }
    file_system_.erase(file_path);
    return true;
  }

  std::optional<std::vector<std::string>> ls(std::string dir_name) {
    std::vector<std::string> result;
    for (const auto& entry : file_system_) {
      std::string full_path = entry.first;
      bool file_in_dir = full_path.compare(0, dir_name.length(), dir_name) == 0;
      if (file_in_dir) {
        std::string file_name = full_path.substr(dir_name.length());
        result.push_back(file_name);
      }
    }
    return result;
  }

 private:
  std::map<std::string, std::string> file_system_;
};

class CRUDApiHandlerTest : public ::testing::Test {
 protected:
  CRUDApiHandlerTest() {
    file_system_io_->write_file("/mnt/crud/Shoes/1", std::ostringstream());
    file_system_io_->write_file("/mnt/crud/Shoes/2", std::ostringstream());
    file_system_io_->write_file("/mnt/crud/Shoes/3", std::ostringstream());
    file_system_io_->write_file("/mnt/crud/Books/1", std::ostringstream());

    NginxConfig config;
    handler_ =
        std::make_shared<CRUDApiHandler>(location_, config, file_system_io_);
  }

  std::string location_ = "/api";
  std::shared_ptr<FileSystemIOInterface> file_system_io_ =
      std::make_shared<TestFileSystemIO>();
  std::shared_ptr<RequestHandler> handler_;
  http::request<http::string_body> request_;
  http::response<http::string_body> response_;
};

TEST_F(CRUDApiHandlerTest, TestFileSystemIOWorksAsExpected) {
  std::ostringstream bytes_to_write;
  bytes_to_write << "{json_looking: bytes}";
  ASSERT_TRUE(file_system_io_->write_file("/mnt/crud/Shoes/1", bytes_to_write));

  std::optional<std::ostringstream> bytes_read =
      file_system_io_->read_file("/mnt/crud/Shoes/1");
  ASSERT_TRUE(bytes_read.has_value());
  ASSERT_EQ(bytes_read.value().str(), bytes_to_write.str());

  ASSERT_TRUE(
      file_system_io_->write_file("/mnt/crud/Phones/1", std::ostringstream()));

  std::optional<std::vector<std::string>> files_opt =
      file_system_io_->ls("/mnt/crud/");
  ASSERT_TRUE(files_opt.has_value());
  std::vector<std::string> files = files_opt.value();
  ASSERT_EQ(files.size(), 5);
  EXPECT_TRUE(std::find(files.begin(), files.end(), "Shoes/1") != files.end());
  EXPECT_TRUE(std::find(files.begin(), files.end(), "Shoes/2") != files.end());
  EXPECT_TRUE(std::find(files.begin(), files.end(), "Shoes/3") != files.end());
  EXPECT_TRUE(std::find(files.begin(), files.end(), "Books/1") != files.end());
  EXPECT_TRUE(std::find(files.begin(), files.end(), "Phones/1") != files.end());
}

TEST_F(CRUDApiHandlerTest, SmokeTest) {
  bool success = handler_->handle_request(request_, response_);
  ASSERT_FALSE(success);
}
