#include <boost/beast/http.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>

#include "crud_request_handler.h"
#include "crud_request_handler_factory.h"
#include "fs_wrapper.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

class FakeOFStream : public OFSWrapper {
 public:
  FakeOFStream(std::string file_path) {}
  bool is_open() { return isOpen; }
  void close() { isOpen = false; }
  void write(std::string str) { written_val += str; }

  std::string written_val = "";
  bool isOpen = true;
  bool closeReturn = true;
};

class FakeIFStream : public IFSWrapper {
 public:
  FakeIFStream(std::string file_path) {}
  bool is_open() { return isOpen; }
  void close() { isOpen = false; }
  bool get(char &ch) {
    if (file_str_ptr >= file_str.length()) {
      return false;
    }
    ch = file_str[file_str_ptr];
    file_str_ptr++;

    return true;
  }

  std::string file_str = "";
  int file_str_ptr = 0;
  bool isOpen = true;
  bool closeReturn = true;
};

class CrudRequestTestFixture : public ::testing::Test {
 public:
  std::string empty_str = "";
  FakeOFStream *ofstream = new FakeOFStream(empty_str);
  FakeIFStream *ifstream = new FakeIFStream(empty_str);
  std::string file_str = "This is what's written in the file!";
  http::response<http::string_body> res;
};

TEST_F(CrudRequestTestFixture, TestGet) {
  ifstream->file_str = file_str;
  OFSWrapper *owrapper = ofstream;
  IFSWrapper *iwrapper = ifstream;
  // // Paths to CRUD handler do not matter because we are using a mock file
  CrudRequestHandler *crud_handler =
      new CrudRequestHandler(empty_str, empty_str, iwrapper, owrapper);

  auto const target = "./some/path";
  http::request<http::string_body> req(http::verb::get, target, 11);
  crud_handler->serve(req, res);
  EXPECT_EQ(file_str, res.body());
  EXPECT_EQ(200, res.result_int());
}

TEST_F(CrudRequestTestFixture, TestDelete) {
  ifstream->file_str = file_str;
  OFSWrapper *owrapper = ofstream;
  IFSWrapper *iwrapper = ifstream;
  // // Paths to CRUD handler do not matter because we are using a mock file
  CrudRequestHandler *crud_handler =
      new CrudRequestHandler(empty_str, empty_str, iwrapper, owrapper);

  auto const target = "./some/path";
  http::request<http::string_body> req(http::verb::delete_, target, 11);
  crud_handler->serve(req, res);
  EXPECT_EQ("Deleted file", res.body());
}

TEST_F(CrudRequestTestFixture, TestPost) {
  ifstream->file_str = file_str;
  OFSWrapper *owrapper = ofstream;
  IFSWrapper *iwrapper = ifstream;
  // // Paths to CRUD handler do not matter because we are using a mock file
  CrudRequestHandler *crud_handler =
      new CrudRequestHandler(empty_str, empty_str, iwrapper, owrapper);

  auto const target = "./";
  http::request<http::string_body> req(http::verb::post, target, 11);
  crud_handler->serve(req, res);
  EXPECT_EQ("{\"id\": 1}", res.body());
  EXPECT_EQ(200, res.result_int());
}

TEST(CrudRequestFactory, CrudRequestFactoryTest) {
  std::map<std::string, std::string> args{{"fs_path", "./"},
                                          {"url_path", "./"}};
  CrudRequestHandlerFactory *crud_req_factory =
      new CrudRequestHandlerFactory(args);
  RequestHandler *crud_req_handler = crud_req_factory->create();
  EXPECT_NE(crud_req_handler, nullptr);
}