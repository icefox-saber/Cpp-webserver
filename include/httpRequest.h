#ifndef hTTPREQUEST_H
#define hTTPREQUEST_H

#include "buffer.h"
#include <algorithm>
#include <map>
#include <regex>
#include <string>
#include <unordered_set>

class httpRequest {
private:
  enum PARSE_STATE {
    REQUEST_LINE,
    HEADERS,
    BODY,
    FINISH,
  };

  /// @brief 是否解析错误
  bool isBad_;
  /// @brief http请求头
  std::map<std::string, std::string> header_;
  /// @brief http请求参数
  std::map<std::string, std::string> post_;

  /// @brief http请求方法
  std::string method_;
  /// @brief http请求路径
  std::string path_;
  /// @brief http请求版本
  std::string version_;
  /// @brief http请求体
  std::string body_;
  /// @brief 状态机状态
  PARSE_STATE state_;
  static const std::unordered_set<std::string> DEFAULT_HTML;

private:
  /// @brief 解析请求行
  /// @param line 请求行不包含换行符
  bool ParseRequestLine_(std::string_view line);
  /// @brief 解析一个首部字行
  /// @param  一个首部字行不包含换行符
  void ParseHeader_(std::string_view line);
  /// @brief 解析body
  /// @param line body行
  void ParseBody_(std::string_view line);

  /// @brief 解析路径
  void ParsePath_();

public:
  httpRequest() {};
  /// @brief 解析http请求报文
  /// @param read_buffer_ 请求报文缓冲区
  /// @return true表示解析成功，false表示解析未完成，isbad()可以判断是否非法请求
  bool parse(buffer &read_buffer_);
  /// @brief 获取文件资源路径
  /// @return 文件路径
  std::string_view path() const { return path_; };
  /// @brief 获取是否是长连接
  /// @return ture表示长连接
  bool IsKeepAlive() const;
  /// @brief 获取报文是否非法
  /// @return true表示非法请求
  bool isbad() const { return isBad_; };
  /// @brief 初始化解析器
  void Init();
  /// @brief 获取请求方法
  /// @return 请求方法
  std::string_view method() const { return method_; };
  /// @brief 获取http版本号
  /// @return http版本号
  std::string_view version() const { return version_; };
  ~httpRequest() {};
};

const std::unordered_set<std::string> httpRequest::DEFAULT_HTML{
    "/index",
};

void httpRequest::Init() {
  method_ = path_ = version_ = body_ = "";
  state_ = REQUEST_LINE;
  isBad_ = false;
  header_.clear();
  post_.clear();
}

bool httpRequest::IsKeepAlive() const {

  return header_.count("Connection") == 1 &&
         header_.find("Connection")->second == "keep-alive" &&
         version_ == "1.1";
}

bool httpRequest::parse(buffer &read_buffer_) {
  static const std::string CRLF = "\r\n";
  while (state_ != FINISH) {

    switch (state_) {
    case REQUEST_LINE: {
      auto ed =
          std::search(read_buffer_.begin().base(), read_buffer_.end().base(),
                      CRLF.begin().base(), CRLF.end().base());
      if (ed == read_buffer_.end().base()) {
        return false;
      }
      std::string_view line(read_buffer_.begin().base(),
                            ed - read_buffer_.begin().base());
      if (!ParseRequestLine_(line)) {
        return false;
      }
      ParsePath_();
      read_buffer_.hasread(ed - read_buffer_.begin().base() + 2);
      state_ = HEADERS;
      break;
    }

    case HEADERS: {
      auto ed =
          std::search(read_buffer_.begin().base(), read_buffer_.end().base(),
                      CRLF.begin().base(), CRLF.end().base());
      if (ed == read_buffer_.end().base()) {
        return false;
      }
      std::string_view line(read_buffer_.begin().base(),
                            ed - read_buffer_.begin().base());
      if (line.empty()) {
        read_buffer_.hasread(2);
        state_ = BODY;
        break;
      }
      ParseHeader_(line);
      read_buffer_.hasread(ed - read_buffer_.begin().base() + 2);
      break;
    }

    case BODY: {
      if (header_.count("Content-Length") == 0) {
        state_ = FINISH;
        break;
      }
      if (read_buffer_.end() - read_buffer_.begin() <
          std::stoi(header_["Content-Length"])) {
        return false;
      }

      std::string_view line(&(*read_buffer_.begin()),
                            std::stoi(header_["Content-Length"]));
      ParseBody_(line);
      read_buffer_.hasread(line.size());
      break;
    }

    default:
      break;
    }
  }
  return true;
};

void httpRequest::ParsePath_() {
  if (path_ == "/") {
    path_ = "/index.html";
  } else {
    for (auto &item : DEFAULT_HTML) {
      if (item == path_) {
        path_ += ".html";
        break;
      }
    }
  }
}

bool httpRequest::ParseRequestLine_(std::string_view line) {
  static const std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
  std::smatch subMatch;
  std::string line_copy(line);
  if (std::regex_match(line_copy, subMatch, patten)) {
    method_ = subMatch[1];
    path_ = subMatch[2];
    version_ = subMatch[3];
    state_ = HEADERS;
    return true;
  }
  isBad_ = true;
  return false;
}

void httpRequest::ParseHeader_(std::string_view line) {
  static const std::regex patten("^([^:]*): ?(.*)$");
  std::smatch subMatch;
  std::string line_copy(line);
  if (regex_match(line_copy, subMatch, patten)) {
    header_[subMatch[1]] = subMatch[2];
  } else {
    state_ = BODY;
  }
}

void httpRequest::ParseBody_(std::string_view line) {
  body_ = line;
  state_ = FINISH;
}

#endif