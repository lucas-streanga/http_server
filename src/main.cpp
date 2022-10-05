#include<iostream>
#include<string>
#include"../include/socket.h"
#include"../include/request.h"
#include<thread>
#include<vector>

/*
 * Main method, where socket will be listening
 * argc and argv are used, such that the user may specify a port number
 *
 *
 */
int main(int argc, char ** argv)
{
    //This specifies the default port
    //Technically, port number is an unsigned short int
    //As only positive values up to 65535 are valid
    unsigned short port = 6789;
    //Our ServerSocket. We'll init it later...
    ServerSocket server_socket;
    //Threadpool to hold all our neat threads
    std::vector<std::thread> threadpool;

    if(argc > 1)
    {
        try{ port = std::stoi(argv[1]); }
        catch(std::exception & e)
        {
            std::cerr << "Invalid port number given: " << argv[1] << '\n';
            exit(EXIT_FAILURE);
        }
    }

    std::cout << "Listening port set to " << port << '\n';

    //Create the server socket
    try { server_socket.init(port); }
    catch (SocketException & e)
    {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }


    while(true)
    {
        Socket client_socket;
        std::cout << "Awaiting connection..." << std::endl;
        try { client_socket = server_socket.accept(); }
        catch (SocketException & e)
        {
            std::cerr << e.what() << std::endl;
            exit(EXIT_FAILURE);
        }

        std::thread t(process_request, client_socket);
        threadpool.push_back(move(t));
    }

    return 0;
}
