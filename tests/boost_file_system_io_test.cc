#include "boost_file_system_io.h"

#include <algorithm>
#include <filesystem>
#include <map>
#include <sstream>

#include "gtest/gtest.h"

class BoostFileSystemIOTest : public ::testing::Test {
 protected:
  std::string get_current_bytes(std::string file_name) {
    std::ifstream file(file_name);
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    return buffer.str();
  }

  BoostFileSystemIO file_system_io;
};

TEST_F(BoostFileSystemIOTest, CanReadFiles) {
  std::string file_name = "./test_file_system_io/file_to_read";
  std::string expected_bytes = get_current_bytes(file_name);

  std::optional<std::ostringstream> byte_stream =
      file_system_io.read_file(file_name);
  EXPECT_TRUE(byte_stream.has_value());
  std::string returned_bytes = byte_stream.value().str();

  EXPECT_EQ(returned_bytes, expected_bytes);
}

TEST_F(BoostFileSystemIOTest, CanWriteFile) {
  std::string file_name = "./test_file_system_io/file_to_update";
  std::string bytes_before_write = get_current_bytes(file_name);

  std::ostringstream bytes_to_write;
  bytes_to_write << "update!";

  bool success = file_system_io.write_file(file_name, bytes_to_write);
  EXPECT_TRUE(success);

  std::string bytes_after_write = get_current_bytes(file_name);
  EXPECT_EQ(bytes_after_write, bytes_to_write.str());
}

TEST_F(BoostFileSystemIOTest, CanDeleteFile) {
  std::string file_name = "./test_file_system_io/file_to_delete";
  std::ofstream file(file_name, std::ios::out);
  if (file.is_open()) {
    file.close();
  }
  ASSERT_TRUE(std::filesystem::exists(file_name));
  bool success = file_system_io.delete_file(file_name);
  EXPECT_TRUE(success);
  EXPECT_FALSE(std::filesystem::exists(file_name));
}

TEST_F(BoostFileSystemIOTest, lsCanHandleNoDirectoryExists) {
  std::optional<std::vector<std::string>> output =
      file_system_io.ls("./does_not_exist");
  EXPECT_FALSE(output.has_value());
}

TEST_F(BoostFileSystemIOTest, lsCanHandleNotADirectory) {
  std::optional<std::vector<std::string>> output =
      file_system_io.ls("./test_file_system_io/file_to_update");
  EXPECT_FALSE(output.has_value());
}

TEST_F(BoostFileSystemIOTest, lsOutputsCorrectFileList) {
  std::optional<std::vector<std::string>> output =
      file_system_io.ls("./test_file_system_io");
  ASSERT_TRUE(output.has_value());
  std::vector<std::string> files = output.value();
  EXPECT_EQ(files.size(), 2);
  EXPECT_TRUE(std::find(files.begin(), files.end(), "file_to_update") !=
              files.end());
  EXPECT_TRUE(std::find(files.begin(), files.end(), "file_to_read") !=
              files.end());
}

TEST_F(BoostFileSystemIOTest, AllCRUDOperationsWork) {
  std::string file_name = "./test_file_system_io/all_crud_one_file";

  EXPECT_TRUE(file_system_io.write_file(file_name, std::ostringstream()));
  EXPECT_TRUE(std::filesystem::exists(file_name));

  std::ostringstream bytes_to_write;
  bytes_to_write << "{a_fake: \"json_object\"}";
  EXPECT_TRUE(file_system_io.write_file(file_name, bytes_to_write));

  std::optional<std::ostringstream> bytes_read;
  bytes_read = file_system_io.read_file(file_name);
  EXPECT_EQ(bytes_read.value().str(), bytes_to_write.str());

  EXPECT_TRUE(file_system_io.delete_file(file_name));
  EXPECT_FALSE(std::filesystem::exists(file_name));
}
