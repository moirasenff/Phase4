#include "Sockets.hpp"
#include "Reducer.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <memory>
#include <functional>
#include <exception>

using Sockets::SocketSystem;
using Sockets::SocketListener;
using Sockets::Socket;
using Sockets::ClientHandler;


int main() {
	SocketSystem ss;
	SocketListener sl(9090, Socket::IP4);
	ClientHandler cp;
	sl.start(cp);

	Trie* trieHead2;
	std::string* strPtr2;
	std::string outDir;

	recv(9090, (char*)trieHead2, sizeof(Trie*), 0);


	recv(9090, (char*)strPtr2, sizeof(strPtr2), 0);

	recv(9090, (char*)&outDir, sizeof(outDir), 0);


	Reducer reducer(outDir);
	reducer.reduce(trieHead2, strPtr2);

	sl.sendString("SUCCESS");

	sl.stop();
}