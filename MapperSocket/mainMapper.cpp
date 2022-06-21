#include "Sockets.hpp"
#include "Mapper.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <memory>
#include <functional>
#include <exception>
#include <vector>


using Sockets::SocketSystem;
using Sockets::SocketListener;
using Sockets::ClientHandler;
using Sockets::Socket;

int main() {
	SocketSystem ss;
	SocketListener sl(9070, Socket::IP4);
	ClientHandler cp;
	sl.start(cp);
	uint32_t dataLength;
	uint32_t dataLengthToo;
	std::string container;
	std::string dir;
	int threadKey = 0;

	//add testing
	recv(9070, (char*)threadKey, sizeof(threadKey), NULL);

	recv(9070, (char*)&container, sizeof(container), 0);

	recv(9070, (char*)&dir, sizeof(dir), 0);

	/*recv(9070, (char*)dataLength, sizeof(uint32_t), 0);
	dataLength = ntohl(dataLength);

	std::vector<uint8_t> rcvBuf;
	rcvBuf.resize(dataLength, 0x00);

	recv(9070, (char*)(rcvBuf[0]), dataLength, 0);

	std::string container;
	container.assign(&(rcvBuf[0]), rcvBuf.size());

	recv(9070, (char*)dataLengthToo, sizeof(uint32_t), 0);
	dataLengthToo = ntohl(dataLengthToo);

	std::vector<uint8_t> rcvBuf2;
	rcvBuf2.resize(dataLengthToo, 0x00);

	recv(9070, (char*)(rcvBuf2[0]), dataLengthToo, 0);

	std::string dir;
	dir.assign(&(rcvBuf2[0]), rcvBuf2.size());*/



	Mapper mapper;
	mapper.map(threadKey, container, dir);


	sl.sendString("SUCCESS");

	sl.stop();
}