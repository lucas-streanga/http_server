#ifndef __H_SOCKET_INIT
#define __H_SOCKET_INIT

//Simple function to set up our socket for us
//Function is defined in socket.cpp

#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<exception>
#include<vector>


//Class wrappers for sockets. Makes it easier to deal with them in main and makes their lifetimes more well-defined

struct SocketException : public std::exception
{
public:
    std::string msg;
    SocketException(const char * message): msg(message) {}
	const char * what () const throw ()
    {
    	return msg.c_str();
    }
};

//Note: in this implementation, sockets can safely be passed by value. ServerSockets cannot (they will close the socketd)
//Default copy constructors should be fine, as the members can just be copied safely.
class Socket
{
    friend class ServerSocket;
    static const std::size_t buffer_size = 8192;
    //Socket Descriptor. Important!
    int socketd;
    //We need to hang onto these too
    sockaddr_in client_address;
    socklen_t client_address_len;


public:

    Socket(): socketd{0}, client_address{0}, client_address_len{0} {}
    Socket(const Socket &) = default;
    void close();
    std::string read() noexcept(false);
    void write(std::byte *, std::size_t);
    void write(const std::string &) noexcept(false);

    //returns the address as human-readable
    std::string get_address();

    //Destructor does not close socket.
    ~Socket() = default;

};


class ServerSocket
{
    //socket descriptor. We need to hang on to this
    int socketd;
    //Socket address. castable to sockaddr for bind()
    sockaddr_in sock_address;
    //needed for setsockopp
    int opt;

public:
    //This can throw if the socket can't be binded
    ServerSocket(unsigned short port) noexcept(false);
    Socket accept() noexcept(false);

    //we can close the socket here (very convenient)
    ~ServerSocket();

};

#endif
