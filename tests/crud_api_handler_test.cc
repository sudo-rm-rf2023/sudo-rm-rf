#include "crud_api_handler.h"

#include "gtest/gtest.h"
#include "request_handler.h"
#include "test_file_system_io.h"

namespace http = boost::beast::http;  // from <boost/beast/http.hpp>

class CRUDApiHandlerTest : public ::testing::Test {
 protected:
  CRUDApiHandlerTest() {
    BOOST_LOG_TRIVIAL(debug) << "Starting test";
    try {
      file_system_io_->write_file("/mnt/crud/Shoes/1", std::ostringstream());
      file_system_io_->write_file("/mnt/crud/Shoes/2", std::ostringstream());
      file_system_io_->write_file("/mnt/crud/Shoes/3", std::ostringstream());
      file_system_io_->write_file("/mnt/crud/Books/1", std::ostringstream());
    } catch (...) {
      BOOST_LOG_TRIVIAL(error) << "Couldn't write to IO";
    }

    NginxConfig config;
    std::shared_ptr<NginxConfigStatement> statement =
        std::make_shared<NginxConfigStatement>();
    statement->tokens_.push_back("data_path");
    statement->tokens_.push_back("/mnt/crud");

    config.statements_.push_back(statement);
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
  files_opt = file_system_io_->ls("/mnt/crud/DOES_NOT_EXIST");
  ASSERT_FALSE(files_opt.has_value());
}

TEST_F(CRUDApiHandlerTest, SmokeTest) {
  bool success = handler_->handle_request(request_, response_);
  ASSERT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::not_implemented);
}

/*** CREATE tests ***/

TEST_F(CRUDApiHandlerTest, CreateNewEntityTest) {
  // prepare request
  request_ = {boost::beast::http::verb::post,
              /*target=*/"/api/Lyrics", /*version=*/11};
  std::string payload = "{Jeremiah was a bullfrog}";
  request_.body() = payload;
  request_.prepare_payload();

  EXPECT_EQ(payload, request_.body());

  EXPECT_TRUE(handler_->handle_request(request_, response_));
  EXPECT_EQ(response_.result_int(), 200);
  EXPECT_EQ(response_[http::field::content_type].to_string(),
            "application/json");
  EXPECT_EQ(response_.body(), "{\"id\":1}");
  // test file is created
  const std::optional<std::vector<std::string>>& files_opt =
      file_system_io_->ls("/mnt/crud/Lyrics");
  ASSERT_TRUE(files_opt.has_value());
  if (!files_opt.has_value()) {
    BOOST_LOG_TRIVIAL(debug) << "No such directory: /mnt/crud/Lyrics";
    return;
  } else {
    BOOST_LOG_TRIVIAL(debug) << "Directory found!";
    std::vector<std::string> files = files_opt.value();
    ASSERT_EQ(files[0], "/1");
    // test contents of file are expected
    std::optional<std::ostringstream> bytes_read =
        file_system_io_->read_file("/mnt/crud/Lyrics/1");
    ASSERT_TRUE(bytes_read.has_value());
    if (bytes_read.has_value()) {
      ASSERT_EQ(bytes_read.value().str(), payload);
    } else {
      BOOST_LOG_TRIVIAL(debug) << "No contents found";
      return;
    }
  }
}

TEST_F(CRUDApiHandlerTest, HandleCreateRequest_EmptyPath_ReturnsBadRequest) {
  // Create request
  request_ = {boost::beast::http::verb::post,
              /*target=*/"/api/", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::bad_request);
}

/*** UPDATE tests ***/

TEST_F(CRUDApiHandlerTest, PutChangeEntityTest) {
  // prepare request
  request_ = {boost::beast::http::verb::put,
              /*target=*/"/api/Shoes/1", /*version=*/11};
  std::string payload = "{Jeremiah was a bullfrog}";
  request_.body() = payload;
  request_.prepare_payload();
  EXPECT_EQ(payload, request_.body());
  EXPECT_TRUE(handler_->handle_request(request_, response_));
  EXPECT_EQ(response_.result_int(), 200);

  // test contents of file are expected
  std::optional<std::ostringstream> bytes_read =
      file_system_io_->read_file("/mnt/crud/Shoes/1");
  ASSERT_TRUE(bytes_read.has_value());
  if (bytes_read.has_value()) {
    ASSERT_EQ(bytes_read.value().str(), payload);
  } else {
    BOOST_LOG_TRIVIAL(debug) << "No contents found";
    return;
  }
  payload = "{was a good friend of mine}";
  request_.body() = payload;
  request_.prepare_payload();
  EXPECT_EQ(payload, request_.body());
  EXPECT_TRUE(handler_->handle_request(request_, response_));
  bytes_read = file_system_io_->read_file("/mnt/crud/Shoes/1");
  ASSERT_TRUE(bytes_read.has_value());
  if (bytes_read.has_value()) {
    ASSERT_EQ(bytes_read.value().str(), payload);
  } else {
    BOOST_LOG_TRIVIAL(debug) << "No contents found the second time";
    return;
  }
}

TEST_F(CRUDApiHandlerTest, HandleUpdateRequest_InValidFile_ReturnsNotFound) {
  // Create update request
  request_ = {boost::beast::http::verb::put,
              /*target=*/"/api/None/0", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::not_found);
}

TEST_F(CRUDApiHandlerTest, HandleUpdateRequest_EmptyPath_ReturnsBadRequest) {
  // Create update request
  request_ = {boost::beast::http::verb::put,
              /*target=*/"/api/", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::bad_request);
}

/*** DELETE tests ***/

TEST_F(CRUDApiHandlerTest, HandleDeleteRequest_ValidFile_ReturnsNoContent) {
  // Create a new item
  std::ostringstream bytes_to_write;
  bytes_to_write << "{donut}";
  file_system_io_->write_file("/mnt/crud/Donut/1", bytes_to_write);

  // Create delete request
  request_ = {boost::beast::http::verb::delete_,
              /*target=*/"/api/Donut/1", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::no_content);
}

TEST_F(CRUDApiHandlerTest, HandleDeleteRequest_NonExistentFile_ReturnsNotFound) {
  // Create delete request
  request_ = {boost::beast::http::verb::delete_,
              /*target=*/"/api/None/0", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::not_found);
}

TEST_F(CRUDApiHandlerTest, HandleDeleteRequest_EmptyPath_ReturnsBadRequest) {
  // Create delete request
  request_ = {boost::beast::http::verb::delete_,
              /*target=*/"/api", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::bad_request);
}

/*** RETRIEVE tests ***/

TEST_F(CRUDApiHandlerTest, HandleRetrieveRequest_ValidFile_ReturnsOkResponse) {
  // Create a new item
  std::ostringstream bytes_to_write;
  bytes_to_write << "{potatoes}";
  file_system_io_->write_file("/mnt/crud/Potato/1", bytes_to_write);

  // Create retrieve request
  request_ = {boost::beast::http::verb::get,
              /*target=*/"/api/Potato/1", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::ok);
  EXPECT_EQ(response_.body(), "{potatoes}");
}

TEST_F(CRUDApiHandlerTest, HandleRetrieveRequest_NotValidFile_ReturnsNotFound) {
  // Create retrieve request
  request_ = {boost::beast::http::verb::get,
              /*target=*/"/api/None/0", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::not_found);
}

TEST_F(CRUDApiHandlerTest, HandleRetrieveRequest_EmptyPath_ReturnsBadRequest) {
  // Create retrieve request
  request_ = {boost::beast::http::verb::get,
              /*target=*/"/api/", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::bad_request);
}

/*** LIST tests ***/

TEST_F(CRUDApiHandlerTest, HandleListRequest_ValidDir_ReturnsOkResponse) {
  // Create new items
  std::ostringstream bytes_to_write;
  bytes_to_write << "{cat}";
  file_system_io_->write_file("/mnt/crud/Cat/1", bytes_to_write);
  file_system_io_->write_file("/mnt/crud/Cat/2", bytes_to_write);
  file_system_io_->write_file("/mnt/crud/Cat/3", bytes_to_write);

  // Create list request
  request_ = {boost::beast::http::verb::get,
              /*target=*/"/api/Cat", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::ok);
  EXPECT_EQ(response_.body(), "[1,2,3]");
}

TEST_F(CRUDApiHandlerTest, HandleListRequest_NotValidDir_ReturnsNotFound) {
  // Create list request
  request_ = {boost::beast::http::verb::get,
              /*target=*/"/api/None", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::not_found);
}

TEST_F(CRUDApiHandlerTest, HandleListRequest_EmptyPath_ReturnsBadRequest) {
  // Create list request
  request_ = {boost::beast::http::verb::get,
              /*target=*/"/api/", /*version=*/11};
  bool success = handler_->handle_request(request_, response_);

  EXPECT_TRUE(success);
  EXPECT_EQ(response_.result(), http::status::bad_request);
}
