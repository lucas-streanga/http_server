/*
 * Lucas Streanga
 * 1001612558
 */

#include"socket.h"
#include<arpa/inet.h>
#include<iostream>
#include<memory>
#include <fcntl.h>


//Just returns the address as a human readable string
std::string Socket::get_address()
{
    char address_string[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(((struct sockaddr_in *) & client_address)->sin_addr), address_string, sizeof(address_string));
    return std::string(address_string);
}

//closes the socket
void Socket::close()
{
    ::close(socketd);
}

//read syscall from socket descriptor (like file descriptor)
//This function can throw!
//I chose 8kb for the buffer size as this is the max Apache web server will read for headers
//Why not buffered reader? The read syscall will use zero to close the socket,
//and we have no way of knowing how long the header lines can be
std::string Socket::read()
{
    char * buffer = new char[buffer_size];
    int len = 1;
    std::string ret;
    do
    {
        len = ::read(socketd, (void *) buffer, buffer_size - 1);
        //Add in a null terminator. Read will not guarantee a null terminator
        if(len > 0)
        {
            buffer[len] = '\0';
            ret.append(buffer);
        }
    } while (len < 1 || len == (buffer_size - 1));
    delete [] buffer;

    return ret;
}


//write syscall to socket descriptor (like file descriptor)
//This function can throw!
void Socket::write(const std::string & str)
{
    if(::write(socketd, (void * ) str.c_str(), strlen(str.c_str())) < 0)
        throw SocketException("socket write failed", errno);
}

//write syscall to socket descriptor (like file descriptor)
//This function can throw!
void Socket::write(const std::shared_ptr<std::byte []> & buffer, std::size_t size)
{
    if(::write(socketd, (void * ) buffer.get(), size) < 0)
        throw SocketException("socket write failed", errno);
}

//write syscall to socket descriptor (like file descriptor)
//This function can throw!
void Socket::write(const shared_buffer<std::byte> & buffer)
{
    if(::write(socketd, (void * ) buffer.get(), buffer.size()) < 0)
        throw SocketException("socket write failed", errno);
}


//Initialize the server socket.
//Mostly lower-level c library functions to sort it out
//We also bind and listen here. Neither functions can block
ServerSocket::ServerSocket(unsigned short port)
{
    //opt will be set to 1
    opt = 1;
    //create a socket and assign the value to the socket descriptor
    socketd = socket(AF_INET, SOCK_STREAM, 0);
    //throw an exception if socket can't be created...
    if(socketd < 0) throw SocketException("unable to create socket", socketd);
    //set socket options... reuse address, port
    if (setsockopt(socketd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        throw SocketException("setsockopt failed", errno);
    //Set up address
    //Explicitly zero out struct
    sock_address = {0};
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = INADDR_ANY;
    sock_address.sin_port = htons(port);

    //Now we bind
    if (bind(socketd, (struct sockaddr*) &sock_address, sizeof(sock_address)) < 0)
        throw SocketException("unable to bind to socket", errno);

    //set to listen with a buffer of 10
    listen(socketd, 10);
    //set the socket to be non-blocking
    fcntl(socketd, F_SETFL, O_NONBLOCK);
}

//just close the socket
ServerSocket::~ServerSocket()
{
    close(socketd);
}

//Accept function, returns a client socket when accepted.
//runs lower-level accept function. Remember, Sockets can be copied safely in my implementation
//This is a non-blocking implementation.
Socket ServerSocket::accept()
{
    Socket client_sock;
    client_sock.client_address_len = sizeof(client_sock.client_address);

    client_sock.socketd = ::accept(socketd, (struct sockaddr *) &client_sock.client_address, (socklen_t *) &client_sock.client_address_len);
    fcntl(client_sock.socketd, F_SETFL, O_NONBLOCK);
    if(client_sock.socketd < 0)
        throw SocketException("enable to create client socket on accept", errno);
    return client_sock;
}

bool ServerSocket::await(unsigned int seconds)
{
    //File descriptor set for select() just one fd, socketd
    FD_ZERO(&read_fds);
    FD_ZERO(&write_fds);
    FD_SET(socketd, &read_fds);
    FD_SET(socketd, &read_fds);

    bool ret = false;

    //run select for the given seconds. Will just block and wait for a connection ready to be accepted.
    timeout = {seconds, 0};
    if(select(socketd + 1, &read_fds, &write_fds, NULL, &timeout) == 1)
        ret = true;
    return ret;
}
