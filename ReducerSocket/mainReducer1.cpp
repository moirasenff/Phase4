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
	SocketListener sl(9080, Socket::IP4);
	ClientHandler cp;
	sl.start(cp);

	Trie* trieHead1;
	std::string* strPtr1;
	std::string outDir;

	recv(9080, (char*)trieHead1, sizeof(Trie*), 0);
	

	recv(9080, (char*)strPtr1, sizeof(strPtr1), 0);

	recv(9080, (char*)&outDir, sizeof(outDir), 0);


	Reducer reducer(outDir);
	reducer.reduce(trieHead1, strPtr1);

	sl.sendString("SUCCESS");

	sl.stop();
}