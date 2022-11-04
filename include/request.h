/*
 * Lucas Streanga
 * 1001612558
 */
#ifndef __H_REQUEST
#define __H_REQUEST

#include"socket.h"
#include"file_cache.h"

//Here is where our functions for processing the request will go...
void process_request(Socket client_socket);

//Ends with functions for strings
bool ends_with(const std::string & str, std::initializer_list<std::string> endings);
//gets the content type from the filename
std::string content_type(const std::string & filename);

#endif
