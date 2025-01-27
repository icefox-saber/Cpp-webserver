#include "../include/sender.h"

int main() {
  sender send;
  send.initialize(8080);
  send.run();
}