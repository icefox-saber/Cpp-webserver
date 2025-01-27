#include "../include/tcpClient.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

using namespace std;
//
int main(int argc, char *argv[]) {
  // std::string destinationFile = argv[3];
  struct file_info {
    char filename[256];
    size_t size;
  } file;
  strcpy(file.filename, "../resource/test.mp4");
  std::string destinationFile = "../resource/dest.mp4";
  std::ofstream outfile(destinationFile, std::ios::binary);
  tcpClient client;
  if (client.initialize() == -1) {
    cerr << "initialize";
    return 1;
  }

  if (client.connect(argv[1], stoi(argv[2])) == -1) {
    cerr << "connect";
    outfile.close();
    return 2;
  }

  client.send((void *)&file, sizeof(file_info));
  int j = client.recv(2000);
  while (j > 0) {
    outfile.write(client.getBuffer(), j);
    j = client.recv(2000);
  }
  outfile.close();
  return 0;
}
