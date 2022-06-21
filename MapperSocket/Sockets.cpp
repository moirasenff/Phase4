/////////////////////////////////////////////////////////////////////////
// Sockets.cpp - C++ wrapper for Win32 socket api                      //
// ver 5.3                                                             //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016           //
// CST 4-187, Syracuse University, 315 443-3948, jfawcett@twcny.rr.com //
//---------------------------------------------------------------------//
// Jim Fawcett (c) copyright 2015                                      //
// All rights granted provided this copyright notice is retained       //
//---------------------------------------------------------------------//
// Application: OOD Project #4                                         //
// Platform:    Visual Studio 2015, Dell XPS 8900, Windows 10 pro      //
/////////////////////////////////////////////////////////////////////////

#include "Sockets.hpp"
#include <iostream>
#include <sstream>
#include <thread>
#include <memory>
#include <functional>
#include <exception>
//#include "../Utilities/Utilities.h"

using namespace Sockets;
//using Util = Utilities::StringHelper;
//template<typename T>
//using Conv = Utilities::Converter<T>;
//using Show = StaticLogger<1>;



/////////////////////////////////////////////////////////////////////////////
// Server's client handler class
// - must be callable object so we've built as a functor
// - passed to SocketListener::start(CallObject& co)
// - Client handling thread starts by calling operator()

//class ClientHandler
//{
//public:
//    void operator()(Socket& socket_);
//    bool testStringHandling(Socket& socket_);
//    bool testBufferHandling(Socket& socket_);
//};

//----< Client Handler thread starts running this function >-----------------

void clearBuffer(Socket::byte* buffer, size_t BufLen)
{
    for (size_t i = 0; i < BufLen; ++i)
        buffer[i] = '\0';
}

void ClientHandler::operator()(Socket& socket_)
{
    while (true)
    {
        // interpret test command

        std::string command = Socket::removeTerminator(socket_.recvString());
       /* Show::write("\n  server rcvd command: " + command);*/
        if (command == "Done")
        {
           /* Show::write("\n  server sent : " + command);*/
            socket_.sendString(command);
            break;
        }
        if (command.size() == 0)
        {
            /*Show::write("\n  client connection closed");*/
            break;
        }
        //Show::write("\n  server recvd: " + command);

        //if (command == "TEST_STRING_HANDLING")
        //{
        //    if (testStringHandling(socket_))
        //        Show::write("\n  ----String Handling test passed\n");
        //    else
        //        Show::write("\n  ----String Handling test failed\n");
        //    continue; // go back and get another command
        //}
        //if (command == "TEST_BUFFER_HANDLING")
        //{
        //    if (testBufferHandling(socket_))
        //        Show::write("\n  ----Buffer Handling test passed\n");
        //    else
        //        Show::write("\n  ----Buffer Handling test failed\n");
        //    continue;  // get another command
        //}
    }

    // we get here if command isn't requesting a test, e.g., "TEST_STOP"

  /*  Show::write("\n");
    Show::write("\n  ClientHandler socket connection closing");*/
    socket_.shutDown();
    socket_.close();
   /* Show::write("\n  ClientHandler thread terminating");*/
}

//----< test string handling >-----------------------------------------------
/*
*   Creates strings, sends to server, then reads strings server echos back.
*/
bool ClientHandler::testStringHandling(Socket& socket_)
{
   /* Show::title("String handling test on server");*/

    while (true)
    {
        std::string str = Socket::removeTerminator(socket_.recvString());
        if (socket_ == INVALID_SOCKET)
            return false;
        if (str.size() > 0)
        {
            //Show::write("\n  bytes recvd at server: " + toString(str.size() + 1));
           /* Show::write("\n  server rcvd : " + str);*/

            if (socket_.sendString(str))
            {
               /* Show::write("\n  server sent : " + str);*/
            }
            else
            {
                return false;
            }
            if (str == "TEST_END")
                break;
        }
        else
        {
            break;
        }
    }
    socket_.sendString("TEST_STRING_HANDLING_END");
   /* Show::write("\n  End of string handling test in ClientHandler");*/
    return true;
}

//----< test buffer handling >-----------------------------------------------
/*
*   Creates buffers, sends to server, then reads buffers server echos back.
*/
bool ClientHandler::testBufferHandling(Socket& socket_)
{
    /*Show::title("Buffer handling test on server");*/
    const size_t BufLen = 20;
    Socket::byte buffer[BufLen];
    bool ok;

    while (true)
    {
        ok = socket_.recv(BufLen, buffer);
        if (socket_ == INVALID_SOCKET)
            return false;
        if (ok)
        {
            std::string temp;
            for (size_t i = 0; i < BufLen; ++i)
                temp += buffer[i];
            //Show::write("\n  bytes recvd at server: " + toString(BufLen));
            /*Show::write("\n  server rcvd : " + temp);*/

            buffer[BufLen - 1] = '\0';
            if (socket_.send(BufLen, buffer))
            {
                /*Show::write("\n  server sent : " + std::string(buffer));*/
            }
            else
            {
               /* Show::write("\n  server send failed");*/
                return false;
            }
            if (temp.find("TEST_END") != std::string::npos)
            {
                //std::string out = "TEST_END";
                //socket_.send(out.size(), (Socket::byte*)out.c_str());
                //Show::write("\n  server sent : " + out);
                break;
            }
        }
        else
        {
            break;
        }
    }
   /* Show::write("\n  End of buffer handling test in ClientHandler");*/
    ::Sleep(4000);
    return true;
}

//----< test string handling - server echos back client sent string >--------

void clientTestStringHandling(Socket& si)
{
    std::string command = "TEST_STRING_HANDLING";
    si.sendString(command);
   /* Show::write("\n  client sent : " + command);*/

    for (size_t i = 0; i < 5; ++i)
    {
        std::string text = "Hello World " + std::string("#");// +Conv<size_t>::toString(i + 1);
        si.sendString(text);
       /* Show::write("\n  client sent : " + text);*/
    }
    command = "TEST_END";
    si.sendString(command);
    /*Show::write("\n  client sent : " + command);*/

    while (true)
    {
        std::string str = Socket::removeTerminator(si.recvString());
       /* if (str.size() == 0)
        {
            Show::write("\n  client detected closed connection");
            break;
        }
        Show::write("\n  client recvd: " + str);
        if (str == "TEST_END")
        {
            Show::write("\n  End of string handling test in client");
            break;
        }*/
    }
}
//----< test buffer handling - server echos back client sent buffer >--------

void clientTestBufferHandling(Socket& si)
{
    std::string command = "TEST_BUFFER_HANDLING";
    si.sendString(command);
   /* Show::write("\n  client sent : " + command);*/

    const int BufLen = 20;
    Socket::byte buffer[BufLen];

    for (size_t i = 0; i < 5; ++i)
    {
        std::string text = "Hello World " + std::string("#");// +Conv<size_t>::toString(i + 1);
        for (size_t i = 0; i < BufLen; ++i)
        {
            if (i < text.size())
                buffer[i] = text[i];
            else
                buffer[i] = '.';
        }
        buffer[BufLen - 1] = '\0';
        si.send(BufLen, buffer);
       /* Show::write("\n  client sent : " + std::string(buffer));*/
    }
    std::string text = "TEST_END";
    for (size_t i = 0; i < BufLen; ++i)
    {
        if (i < text.size())
            buffer[i] = text[i];
        else
            buffer[i] = '.';
    }
    buffer[BufLen - 1] = '\0';
    si.send(BufLen, buffer);
    /*Show::write("\n  client sent : " + std::string(buffer));*/

    bool ok;
    std::string collector;
    while (true)
    {
        if (si.bytesWaiting() == 0)
            break;
        ok = si.recv(BufLen, buffer);
        if (!ok)
        {
           /* Show::write("\n  client unable to receive");*/
            break;
        }
        std::string str(buffer);
        collector += str;
        if (str.size() == 0)
        {
            /*Show::write("\n  client detected closed connection");*/
            break;
        }
       /* Show::write("\n  client rcvd : " + str);*/
        if (collector.find("TEST_END") != std::string::npos)
        {
           /* Show::write("\n  End of buffer handling test in client");*/
            break;
        }
    }
}