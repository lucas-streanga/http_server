#include"../include/request.h"
#include<thread>
#include<sstream>
#include<string>
#include<fstream>

const char * CRLF = "\r\n";

void process_request(Socket sock)
{
    //std::cout is actually thread safe, provided you have ONE cout statement
    //multiple cout statements can cause them to become jumbled across threads...

    bool file_exists = true;
    std::ifstream file;

    //to form our response!
    std::string status_line;
    std::string content_type_line;
    std::string entity_body_line;
    std::string content_length_line;

    std::string test;

    //lets create a buffer and zero it out...
    std::string content = sock.read();
    //scratch space string
    std::string scratch;
    //Extract the request line from the content and put it in a stringstream so its easy to parse
    std::stringstream request(content.substr(0, content.find("\r\n")));

    std::string filename = ".";
    request >> scratch;
    request >> scratch;
    filename.append(scratch);

    //try to open the file
    file.open(filename);
    if(!file.is_open())
        file_exists = false;

    if(file_exists)
    {

    }
    else
    {
        status_line = "HTTP/1.1 404 Not Found";
        entity_body_line = "<HTML><HEAD><TITLE>Not Found</TITLE></HEAD><BODY>Not Found</BODY></HTML>";
        //entity_body_line = "Hello World!";
        content_type_line = "Content-Type: text/html; charset=utf-8";
        content_length_line = "Content-Length: " + std::to_string(entity_body_line.length()) + "\r\nConnection: close";
    }

    //sock.write(test);
    sock.write(status_line + CRLF + content_length_line + CRLF + content_type_line + CRLF + CRLF + entity_body_line);
    std::cout << status_line + CRLF + content_length_line + CRLF + content_type_line + CRLF + CRLF + entity_body_line << std::endl;

    std::cout << "New connection (address " << sock.get_address() << ") established on thread ID " << std::this_thread::get_id()
    << "\n****CONTENT****\nRequest: " << request.str() << "\nFile " << filename << " Exists? " << file_exists << std::endl << content << std::endl;

    sock.close();
}
