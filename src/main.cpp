/*
 * Lucas Streanga
 * 1001612558
 */

#include<iostream>
#include<string>
#include"socket.h"
#include"request.h"
#include"file_cache.h"
#include"shared_buffer.h"
#include<thread>
#include<vector>
#include<memory>
#include<signal_handler.h>

extern std::atomic<bool> terminated;
file_cache<2> cache = file_cache<2>();

/*
 * Main method, where socket will be listening
 * argc and argv are used, such that the user may specify a port number
 */
int main(int argc, char ** argv)
{
    //set up signal handling thread
    std::thread(signal_thread).detach();
    //block sig int on main...
    block_signal(SIGINT);


    //This specifies the default port
    //Technically, port number is an unsigned short int
    //As only positive values up to 65535 are valid
    unsigned short port = 6789;
    //Our ServerSocket. We'll construct it later...
    std::unique_ptr<ServerSocket> server_socket;

    if(argc > 1)
    {
        try{ port = std::stoi(argv[1]); }
        catch(std::exception & e)
        {
            std::cerr << "Invalid port number given: " << argv[1] << '\n';
            return EXIT_FAILURE;
        }
    }

    std::cout << "Listening port set to " << port << '\n';
    std::cout << "Awaiting connection..." << std::endl;

    //Create the server socket
    try { server_socket = std::make_unique<ServerSocket>(port); }
    catch (SocketException & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }


    while(true)
    {
        if(terminated)
            break;

        Socket client_socket;
        if(server_socket->await())
        {
            try { client_socket = server_socket->accept(); }
            catch (SocketException & e)
            {
                std::cerr << e.what() << std::endl;
            }

            //why no catching exceptions? Since the thread is detached, exceptions must be caught there.
            //Notice a new thread is created and will run the process_request function
            std::thread(process_request, client_socket).detach();
        }
    }

    return 0;
}
