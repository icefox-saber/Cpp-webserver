#include "../include/server.h"

int main() {
  server sev(3, 8080, 2, 1000, 8, 3);
  sev.run();
}