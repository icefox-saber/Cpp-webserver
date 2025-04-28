#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include "buffer.h"
#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>

class httpResponse {
private:
  std::string_view path_;
  std::string_view version_;
  std::string_view srcdir_;
  bool isKeepAlive_;
  int code_;
  std::string filecontent_; // 注意:这里需要真正存放数据
public:
  httpResponse() = default;
  ~httpResponse() = default;
  void init(std::string_view path, bool isKeepAlive, int code,
    std::string_view srcdir);
  
  void makeResponse(buffer &write_buffer_);
  
  void clear();
  
private:
std::string_view getContentType(std::string_view path);
  
};

#endif
