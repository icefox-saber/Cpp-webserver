#include "../include/httpRequest.h"

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