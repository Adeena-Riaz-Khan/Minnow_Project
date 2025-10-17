#include "socket.hh"
#include "tcp_minnow_socket.hh"
#include <iostream>
#include <string>


using namespace std;

void get_URL(const string& host, const string& path) {

  CS144TCPSocket sock;
  Address addr(host, "http");
  sock.connect(addr);

  
  string request = "GET " + path + " HTTP/1.1\r\n"
                   "Host: " + host + "\r\n"
                   "Connection: close\r\n\r\n";

  sock.write(request);

  
  while (!sock.eof()) {
    string buffer;
    sock.read(buffer);
    cout << buffer;
  }
}

int main(int argc, char* argv[]) {
  try {
    if (argc < 3) {
      cerr << "Usage: " << argv[0] << " HOST PATH\n";
      return EXIT_FAILURE;
    }

    string host = argv[1];
    string path = argv[2];

    get_URL(host, path);
  } catch (const exception& e) {
    cerr << e.what() << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

