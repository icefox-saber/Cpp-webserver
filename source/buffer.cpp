#include "../include/buffer.h"
#include <cassert>
#include <cerrno>
#include <limits>
#include <sys/uio.h> //readv
#include <unistd.h>  //write

buffer::buffer(std::size_t len) : buffer_(len), readpos_{0}, writepos_{0} {
  assert(buffer_.capacity());
}

buffer::~buffer() {}

void buffer::make_sure_writable(std::size_t len) {
  if (buffer_.size() < writepos_ + len) {
    make_space(len);
  }
  assert(buffer_.size() >= writepos_ + len);
}

void buffer::make_space(std::size_t len) {
  if (PrependableBytes() + writableBytes() < len) {
    buffer_.resize(writepos_ + len);
  } else {
    std::copy(buffer_.begin() + readpos_, writeposIterator(), buffer_.begin());
    writepos_ -= readpos_;
    readpos_ = 0;
  }
}

std::size_t buffer::readableBytes() const { return writepos_ - readpos_; }
std::size_t buffer::writableBytes() const { return buffer_.size() - writepos_; }
std::size_t buffer::PrependableBytes() const { return readpos_; }

std::vector<char>::const_iterator buffer::readposIterator() const {
  return buffer_.begin() + readpos_;
}

std::vector<char>::iterator buffer::writeposIterator() {
  return buffer_.begin() + writepos_;
}

std::size_t buffer::append(const char *buffer, std::size_t len) {
  make_sure_writable(len);
  std::copy(buffer, buffer + len, writeposIterator());
  writepos_ += len;

  return len;
}

std::size_t buffer::get(char *buffer, std::size_t len) {
  len = std::min(len, readableBytes());
  auto readBegin = readposIterator();
  std::copy(readBegin, readBegin + len, buffer);
  readpos_ += len;
  return len;
}

ssize_t buffer::readFromFD(int fd, int *errorptr) {
  char exbuff[std::numeric_limits<u_int16_t>::max()];

  iovec iov[2];
  const size_t writenum = writableBytes();
  iov[0].iov_base = writeposIterator().base();
  iov[0].iov_len = writenum;
  iov[1].iov_base = exbuff;
  iov[1].iov_len = sizeof exbuff;

  const ssize_t len = readv(fd, iov, 2);

  if (len < 0) {
    *errorptr = errno;
  } else {
    const size_t stlen = static_cast<size_t>(len);
    writepos_ += std::min(stlen, writenum);
    if (stlen > writenum) {
      append(exbuff, stlen - writenum);
    }
  }

  return len;
}

ssize_t buffer::writeToFD(int fd, int *errorptr) {

  const ssize_t len = write(fd, readposIterator().base(), readableBytes());
  if (len < 0) {
    *errorptr = errno;
  } else {
    readpos_ += len;
  }

  return len;
}

void buffer::clear() {
  readpos_ = 0;
  writepos_ = 0;
}