#include <cassert>
#include <memory>
#include <sys/epoll.h>
#include <vector>

class Epoll {
private:
  int epoll_;
  std::vector<epoll_event> events;
  int timeout_;

public:
  Epoll(size_t maxevents = 16, int timeout = -1);
  bool add_fd(int fd, uint32_t events);
  bool mod_fd(int fd, uint32_t events);
  bool del_fd(int fd, uint32_t events);
  int wait(int timeout);
  int wait();
  int getFd(int index);
  int getEvent(int index);
  ~Epoll();
};
