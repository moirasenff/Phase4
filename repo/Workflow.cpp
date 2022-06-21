#include <chrono>
#include <thread>
#include <mutex>
#include "Workflow.hpp"
#include "FileManager.hpp"
#include "Reducer.hpp"
#include "Mapper.hpp"
#include <sstream>
#include <thread>
#include <memory>
#include <functional>
#include <exception>
#include <Winsock2.h>
#include <MSWSock.h>
#include "Sockets.hpp"


using Sockets::SocketSystem;
using Sockets::SocketConnecter;


std::atomic<int> keyCounter;
std::mutex mutex;

bool Workflow::run(std::string inDir, std::string tempDir, std::string outDir) {

	BOOST_LOG_TRIVIAL(info) << "Starting workflow.";
	std::vector<std::string> inVect = FileManager::read(inDir);
	std::vector<std::thread> threadVect;
	keyCounter = 0;
	static Trie* trieHead1 = new Trie();
	static Trie* trieHead2 = new Trie();
	std::string outStr1;
	std::string* strPtr1 = &outStr1;
	std::string outStr2;
	std::string* strPtr2 = &outStr2;
	//Reducer reducer1(outDir);
	//Reducer reducer2(outDir);
	

	auto start = std::chrono::high_resolution_clock::now();
	try {
		for (int i = 0; i < inVect.size(); i++) {
			threadVect.emplace_back([=] {
				int threadKey = keyCounter++;

				SocketSystem ss;
				SocketConnecter si;
				si.connect("localhost", 9070);
				
				send(9070, (char*)threadKey, sizeof(threadKey), 0);

				send(9070, (char*)&inVect.at(i), sizeof(inVect.at(i)), 0);

				send(9070, (char*)&outDir, sizeof(outDir), 0);

				/*std::string dataToSend = inVect.at(i);

				uint32_t dataLength = htonl(dataToSend.size());

				send(9070, (char*)dataLength, sizeof(uint32_t), 0);
				send(9070, dataToSend.c_str(), dataToSend.size(), 0);

				std::string dataToSendToo = tempDir;
				uint32_t dataLengthToo = htonl(dataToSendToo.size());

				send(9070, (char*)dataLengthToo, sizeof(uint32_t), 0);
				send(9070, dataToSendToo.c_str(), dataToSendToo.size(), 0);*/

				uint32_t temp;
				int tester = recv(9070, (char*)temp, sizeof(temp), 0);
				


				si.shutDownSend();

				if (tester != -1) {


					//// setup objects
					//Mapper mapper;
					//// for each file in the inDirectory
					//int threadKey = keyCounter++;
					//mapper.map(threadKey, inVect.at(i), tempDir);

					BOOST_LOG_TRIVIAL(info) << "Wrote temp File" << threadKey;
					// temp dir now has intermediate files in it. now read it back in
					std::vector<std::string> tempVect = FileManager::read(tempDir, threadKey);

					sort(trieHead1, trieHead2, tempVect);
				
				}
				else {
					BOOST_LOG_TRIVIAL(error) << "Something bad happened in the mapper socket. Exiting now.";
					return false;
				}
			});
			
		}
		for (auto& thread : threadVect) {
			thread.join();
		}
		// trie is populated with string values. pump them out to the vector
		std::thread redT1([=] {

			SocketSystem ss;
			SocketConnecter si;
			si.connect("localhost", 9080);

			send(9080, (char*)trieHead1, sizeof(Trie*), 0);

			send(9080, (char*)strPtr1, sizeof(std::string*), 0);

			send(9080, (char*)&outDir, sizeof(outDir), 0);
			
			uint32_t temp;
			int tester = recv(9070, (char*)temp, sizeof(temp), 0);

			si.shutDownSend();

			/*Reducer reducer(outDir);*/

			if (tester != -1) { //FIX THIS TO WORK WITH SOCKETS
				BOOST_LOG_TRIVIAL(info) << "All files in thread 1 completed successfully!";

			}
			else {
				BOOST_LOG_TRIVIAL(info) << "Something bad happened in the trie. Exiting now.";
				return false;
			}});
		std::thread redT2([=] {

			SocketSystem ss;
			SocketConnecter si;
			si.connect("localhost", 9090);

			send(9080, (char*)trieHead2, sizeof(Trie*), 0);

			send(9080, (char*)strPtr2, sizeof(std::string*), 0);

			send(9080, (char*)&outDir, sizeof(outDir), 0);

			uint32_t temp2;
			int tester2 = recv(9070, (char*)temp2, sizeof(temp2), 0);

			si.shutDownSend();

			/*Reducer reducer(outDir);*/
			if (tester2 != -1) { //FIX THIS TO WORK WITH SOCKETS
				BOOST_LOG_TRIVIAL(info) << "All files in thread 2 completed successfully!";
			}
			else {
				BOOST_LOG_TRIVIAL(info) << "Something bad happened in the trie. Exiting now.";
				return false;
			}});
		redT1.join();
		redT2.join();

		std::string outTot = outStr1 + outStr2;
		//std::cout << "Print string:" << std::endl;
		//std::cout << outStr1 << std::endl;
		//FileManager::write(outStr1, outDir, -1);
		//FileManager::write(outStr2, outDir, -1);
		FileManager::write(outTot, outDir, -1);
	}
	catch (std::exception& e) {
		BOOST_LOG_TRIVIAL(info) << "ERROR: " << e.what();
	}
	auto finish = std::chrono::high_resolution_clock::now();
	auto durationSec = std::chrono::duration_cast<std::chrono::seconds>(finish - start);
	BOOST_LOG_TRIVIAL(info) << "Workflow took: " << durationSec.count() << "s to complete.";
	return true;
}

void Workflow::sort(Trie* head1, Trie* head2, std::vector<std::string> inVect) {
	for (int i = 0; i < inVect.size(); i++) {
		std::string line = inVect.at(i);
		std::string::size_type start_pos = line.find("\"");
		std::string::size_type end_pos;
		if (start_pos != std::string::npos) {
			end_pos = line.find("\"", start_pos + 1);
			// get the word without quotes
			std::string word = line.substr(start_pos + 1, end_pos - 1);
			// populate the common trie. mutex lock to be thread safe
			mutex.lock();
			if (word[0] >= 'a' && word[0] <= 'm')
			{
				if (head1->search(word) == 0) {
					// this is a new word
					head1->insert(word);
				}
				else {
					// word already existed. increment the count
					head1->increment(word);
				}
			}
			else if (word[0] >= 'n' && word[0] <= 'z')
			{
				if (head2->search(word) == 0) {
					// this is a new word
					head2->insert(word);
				}
				else {
					// word already existed. increment the count
					head2->increment(word);
				}
			}
			mutex.unlock();
		}
		else {
			BOOST_LOG_TRIVIAL(error) << "Malformed temp directory found in sorted()";
		}
	}
}

bool Workflow::test() {
	// tests sort, reduce, trie
	bool retVal = false;
	std::string outStr;
	std::string* outPtr = &outStr;
	_mkdir("test");
	Workflow wf;
	Reducer reducer("test");
	std::vector<std::string> tempVect;
	tempVect.push_back("\"test\", 1");
	tempVect.push_back("\"other\", 1");
	tempVect.push_back("\"a\", 1");
	tempVect.push_back("\"test\", 1");
	Trie* trieHead1 = new Trie();
	Trie* trieHead2 = new Trie();
	wf.sort(trieHead1, trieHead2, tempVect);
	if (trieHead1->search("test") == 0 ||
		trieHead1->search("other") == 0 ||
		trieHead1->search("a") == 0) {
		retVal = false;
		goto clean_up;
	}

	if (reducer.reduce(trieHead1, outPtr)) {
		retVal = true;
	}
	else {
		retVal = false;
	}
clean_up:
	// clean up memory
	while (!trieHead1->isLeaf) {
		trieHead1->pop();
	}
	std::filesystem::remove_all("test");
	return retVal;
}