#include"../include/socket.h"
#include <arpa/inet.h>
#include<iostream>


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
std::string Socket::read()
{
    char buffer[1024] = {};
    ::read(socketd, (void *) buffer, sizeof(buffer) - 1);

    return std::string(buffer);
}


//write syscall to socket descriptor (like file descriptor)
//This function can throw!
void Socket::write(const std::string & str)
{
   send(socketd, (void * ) str.c_str(), strlen(str.c_str()) + 1, 0);
}
//Initialize the server socket.
//Mostly lower-level c library functions to sort it out
//We also bind and listen here. Neither functions can block! Accept is the function that blocks
void ServerSocket::init(unsigned short port)
{
    //opt will be set to 1
    opt = 1;
    //create a socket and assign the value to the socket descriptor
    socketd = socket(AF_INET, SOCK_STREAM,0);
    //throw an exception if socket can't be created...
    if(socketd < 0) throw SocketException("unable to create socket");
    //set socket options
    if (setsockopt(socketd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        throw SocketException("setsockopt failed");
    //Set up address
    //Explicitly zero out struct
    sock_address = {0};
    sock_address.sin_family = AF_INET;
    sock_address.sin_addr.s_addr = INADDR_ANY;
    sock_address.sin_port = htons(port);

    //Now we bind
    if (bind(socketd, (struct sockaddr*) &sock_address, sizeof(sock_address)) < 0)
        throw SocketException("unable to bind to socket");

    //Lastly, set to listen with a buffer of 10
    listen(socketd, 10);
}

//just close the socket
ServerSocket::~ServerSocket()
{
    close(socketd);
}

//Accept function, returns a client socket when accepted.
//runs lower-level accept function. Remember, Sockets can be copied safely in my implementation
Socket ServerSocket::accept()
{
    Socket client_sock;
    client_sock.client_address_len = sizeof(client_sock.client_address);

    client_sock.socketd = ::accept(socketd, (struct sockaddr *) &client_sock.client_address, (socklen_t *) &client_sock.client_address_len);
    if(client_sock.socketd < 0)
        throw SocketException("enable to create client socket on accept");
    return client_sock;

}
