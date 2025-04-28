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
  
  void httpconn::process() {
    read_buffer_.readFromFD(FD_, nullptr);
    std::string_view line(read_buffer_.begin().base(), read_buffer_.end().base());
    log("read request\n:{}",line);
    // 如果包体未完成接受
    std::string srcdir = "./resourse";
    requestParser_.Init();
    if (!requestParser_.parse(read_buffer_)) {
      if (requestParser_.isbad()) {
        response_.init(requestParser_.path(), false, 400,srcdir);
        response_.makeResponse(write_buffer_);
        write_buffer_.writeToFD(FD_, nullptr);
        clear();
        return;
      }
      return;
    }
  
    // 如果是post请求告知不支持
    if (requestParser_.method() == "POST") {
      response_.init(requestParser_.path(), false, 405,srcdir);
    } else {
      response_.init(requestParser_.path(), requestParser_.IsKeepAlive(), 200,srcdir);
    }
  
    response_.makeResponse(write_buffer_);
    std::string_view line1(write_buffer_.begin().base(), write_buffer_.end().base());
    //log("respose\n:{}",line1);
    // 写入响应
    write_buffer_.writeToFD(FD_, nullptr);
    if (!requestParser_.IsKeepAlive()) {
      clear();
    }
  
    return;
  }
  
  httpconn::httpconn() : FD_(-1),client_addr_(0) {
  }

  httpconn::~httpconn() {}
  