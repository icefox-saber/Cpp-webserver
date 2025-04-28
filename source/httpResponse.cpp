#include "../include/httpResponse.h"
#include "../include/log.h"
void httpResponse::init(std::string_view path, bool isKeepAlive, int code,
                        std::string_view srcdir) {
  path_ = path;
  isKeepAlive_ = isKeepAlive;
  code_ = code;
  srcdir_ = srcdir;
  filecontent_.clear();
}

void httpResponse::makeResponse(buffer &write_buffer_) {
  // 简单状态码文本
  static std::unordered_map<int, std::string_view> statusText = {
      {200, "OK"},
      {400, "Bad Request"},
      {404, "Not Found"},
      {500, "Internal Server Error"}};

  if (code_ == 0)
    code_ = 200;
  // 1. 打开文件
  log("open file: {}", std::string(srcdir_) + std::string(path_));
  std::ifstream file(std::string(srcdir_) + std::string(path_),
                     std::ios::binary);
  if (!file.is_open()) {
    // 找不到文件直接返回简单错误页面
    filecontent_ = "<html><body><h1>404 Not Found</h1></body></html>";
    code_ = 404;
  } else {
    filecontent_ = std::string((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
    file.close();
  }

  // 2. 状态行
  write_buffer_.append("HTTP/1.1 ");
  write_buffer_.append(std::to_string(code_));
  write_buffer_.append(" ");
  write_buffer_.append(statusText.count(code_) ? statusText[code_] : "Unknown");
  write_buffer_.append("\r\n");

  // 3. 响应头部
  write_buffer_.append("Content-Type: ");
  write_buffer_.append(getContentType(path_));
  write_buffer_.append("\r\n");

  write_buffer_.append("Content-Length: ");
  write_buffer_.append(std::to_string(filecontent_.size()));
  write_buffer_.append("\r\n");
  log("Content-Length: {}", filecontent_.size());
  write_buffer_.append("Connection: ");
  write_buffer_.append(isKeepAlive_ ? "keep-alive" : "close");
  write_buffer_.append("\r\n");
  // 4. 空行
  write_buffer_.append("\r\n");

  // 5. 响应体
  write_buffer_.append(filecontent_);
}

void httpResponse::clear() {
  path_ = "";
  version_ = "";
  srcdir_ = "";
  isKeepAlive_ = false;
  code_ = 0;
  filecontent_.clear();
}

std::string_view httpResponse::getContentType(std::string_view path) {
  if (path.ends_with(".html"))
    return "text/html";
  if (path.ends_with(".css"))
    return "text/css";
  if (path.ends_with(".js"))
    return "application/javascript";
  if (path.ends_with(".json"))
    return "application/json";
  if (path.ends_with(".png"))
    return "image/png";
  if (path.ends_with(".jpg") || path.ends_with(".jpeg"))
    return "image/jpeg";
  if (path.ends_with(".mpd"))
    return "application/dash+xml";
  if (path.ends_with(".m4s") || path.ends_with(".mp4"))
    return "video/mp4";
  return "text/plain"; // 默认
}