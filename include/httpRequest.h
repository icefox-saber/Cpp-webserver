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

#endif