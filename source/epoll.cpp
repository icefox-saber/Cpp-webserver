#include "../include/epoll.h"

Epoll::Epoll(size_t maxevents, int timeout)
    : events(maxevents), timeout_(timeout) {
  epoll_ = epoll_create1(EPOLL_CLOEXEC);
}

bool Epoll::add_fd(int fd, uint32_t events) {
  if (fd < 0) {
    return false;
  }

  epoll_event ev{};
  ev.events = events;
  ev.data.fd = fd;
  return 0 == epoll_ctl(epoll_, EPOLL_CTL_ADD, fd, &ev);
}
bool Epoll::mod_fd(int fd, uint32_t events) {
  if (fd < 0) {
    return false;
  }

  epoll_event ev{};
  ev.events = events;
  ev.data.fd = fd;
  return 0 == epoll_ctl(epoll_, EPOLL_CTL_MOD, fd, &ev);
}

bool Epoll::del_fd(int fd, uint32_t events) {
  if (fd < 0) {
    return false;
  }

  epoll_event ev{};

  return 0 == epoll_ctl(epoll_, EPOLL_CTL_DEL, fd, &ev);
}

int Epoll::wait(int timeout) {

  return epoll_wait(epoll_, events.data(), static_cast<int>(events.size()),
                    timeout);
}

int Epoll::wait() {

  return epoll_wait(epoll_, events.data(), static_cast<int>(events.size()),
                    timeout_);
}

int Epoll::getFd(int index) {
  assert(index >= 0 && index < static_cast<int>(events.size()));
  return events[index].data.fd;
}
int Epoll::getEvent(int index) {
  assert(index >= 0 && index < static_cast<int>(events.size()));

  return events[index].events;
}
Epoll::~Epoll() {}
