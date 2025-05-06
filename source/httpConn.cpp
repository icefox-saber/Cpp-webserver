#include "../include/httpConn.h"

void httpconn::clear() {
  close(FD_);
  read_buffer_.clear();
  write_buffer_.clear();
  requestParser_.Init();
  response_.clear();
  FD_ = -1;
}

httpconn::httpconn(int fd, sockaddr_in client_addr)
    : FD_(fd), client_addr_(client_addr) {}

void httpconn::reset(int fd, sockaddr_in client_addr) {
  if (FD_ != -1) {
    clear();
  }
  FD_ = fd;
  client_addr_ = client_addr;
}
void httpconn::dealwrite(int *ifrest) {
  //  写入响应
  write_buffer_.writeToFD(FD_, ifrest);
}
void httpconn::process() {
  read_buffer_.readFromFD(FD_, nullptr);
  std::string_view line(read_buffer_.begin().base(), read_buffer_.end().base());
  std::string srcdir = "./resourse";
  requestParser_.Init();
  if (!requestParser_.parse(read_buffer_)) {
    // 解析出错误
    if (requestParser_.isbad()) {
      log("read request path :{}", requestParser_.path());
      response_.init(requestParser_.path(), false, 400, srcdir);
      response_.makeResponse(write_buffer_);
      write_buffer_.writeToFD(FD_, nullptr);
      clear();
      return;
    }
    // 如果包体未完成接受
    log("message not complete");
    return;
  }

  // 如果是post请求告知不支持
  if (requestParser_.method() == "POST") {
    log("post request");
    response_.init(requestParser_.path(), false, 405, srcdir);
  } else {
    log("get request {}", requestParser_.path());
    response_.init(requestParser_.path(), requestParser_.IsKeepAlive(), 200,
                   srcdir);
  }

  response_.makeResponse(write_buffer_);
  std::string_view line1(write_buffer_.begin().base(),
                         write_buffer_.end().base());


  return;
}

httpconn::httpconn() : FD_(-1), client_addr_(0) {}

httpconn::~httpconn() {}
