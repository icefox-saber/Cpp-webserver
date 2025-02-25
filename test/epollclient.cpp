#include "../include/tcpClient.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;
//
int main(int argc, char *argv[]) {
  tcpClient client;
  if (client.initialize() == -1) {
    cerr << "initialize";
    return 1;
  }

  if (client.connect(argv[1], stoi(argv[2])) == -1) {
    cerr << "connect";
    return 2;
  }
  while (true) {
    std::string str;
    cin >> str;
    client.send(str, str.size());
    int i = client.recv(2000);

    std::cout.write(client.getBuffer(), i);
    std::cout.flush();
  }

  return 0;
}
