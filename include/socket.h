/*
 * Lucas Streanga
 * 1001612558
 */
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
#include<memory>
#include<errno.h>
#include"shared_buffer.h"


//Class wrappers for sockets. Makes it easier to deal with them in main and makes their lifetimes more well-defined

struct SocketException : public std::exception
{
public:
    std::string msg;
    int error_num;

    SocketException(const char * message, int err): msg(message)
    {
        char error_msg[16];
        if(!strerror_r(err, error_msg, 16))
        {
            msg.append(": ");
            msg.append(error_msg);
        }
    }

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
    //Read 512 bytes at a time
    static const std::size_t buffer_size = 1024;
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

    //This one writes raw bytes. Note the use of shared pointer and const,
    //Indicating this function does not own or modify this memory
    void write(const std::shared_ptr<std::byte []> & buffer, std::size_t size) noexcept(false);
    void write(const shared_buffer<std::byte> & buffer) noexcept(false);
    //This one writes a string. String is not modified (const)
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
    //file descripor set needed for select
    fd_set read_fds;
    fd_set write_fds;
    //timeout
    struct timeval timeout;

public:
    //This can throw if the socket can't be binded
    ServerSocket(unsigned short port) noexcept(false);
    //Returns a client socket on accepting connection
    //This can throw
    Socket accept() noexcept(false);
    //This function will wait for a connection for 5 seconds
    //Why do we have this? So accept does not block indefinitely
    //Why is accept blocking indefinitely bad? Thread safety with aborting main with ctrl-C
    //Returns true if a connection happens, false if not...
    bool await(unsigned int seconds = 5);

    //we can close the socket here (very convenient)
    ~ServerSocket();

};

#endif
