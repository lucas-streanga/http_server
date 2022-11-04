/*
 * Lucas Streanga
 * 1001612558
 */
#include"request.h"
#include<thread>
#include<sstream>
#include<string>
#include<fstream>
#include"file_cache.h"
#include"shared_buffer.h"

const char * CRLF = "\r\n";

/* Because threads work differently in C++ vs Java, I just used functions here instead of a class*/

//Interestingly, C++ doesn't have an ends with function until C++20
//will return true if the string str ends with any of the ending strings
bool ends_with(const std::string & str, std::initializer_list<std::string> endings)
{
    for(auto ending : endings)
        if(ending.length() > str.length())
            return false;

    for(auto ending : endings)
        if(!(str.substr(str.length() - ending.length())).compare(ending))
            return true;
    return false;
}

//Gives the content type from the filename
std::string content_type(const std::string & filename)
{
    if(ends_with(filename, {"htm", "html"}))
        return "text/html";
    else if(ends_with(filename, {"jpeg", "jpg"}))
        return "image/jpeg";
    else if(ends_with(filename, {"jpeg", "jpg"}))
        return "image/jpeg";
     else if(ends_with(filename, {"gif"}))
        return "image/gif";
     else if(ends_with(filename, {"css"}))
        return "text/css";
     else if(ends_with(filename, {"js"}))
        return "text/javascript";
    else
        return "application/octet-stream";
}

void process_request(Socket sock)
{
    //std::cout is actually thread safe
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
    std::string content;
    try{ content = sock.read(); }
    catch( SocketException & e) { std::cerr << "Processing request failed: " << e.what(); sock.close(); return; }

    //scratch space string
    std::string scratch;
    //Extract the request line from the content and put it in a stringstream so its easy to parse
    std::stringstream request(content.substr(0, content.find(CRLF)));

    std::string filename = ".";
    request >> scratch;
    request >> scratch;
    filename.append(scratch);

    //try to open the file
    //open in binary mode to get raw bytes, necessary for things like images
    if(filename != ".")
        file.open(filename, std::ios::binary);
    if(!file.is_open())
        file_exists = false;

    std::cout << "\nNew connection (address " << sock.get_address() << ") established on thread ID " << std::this_thread::get_id()
    << "\n**Request: " << request.str() << std::endl;

    if(file_exists)
    {
        status_line = "HTTP/1.0 200 OK";
        content_type_line = "Content-Type: " + content_type(filename) + "; charset=utf-8";
        //shared_buffer<std::byte> buffer;
        std::size_t size;

        //This just gets the size of the file
        file.seekg(0, std::ios::end);
        size = file.tellg();
        file.seekg(0, std::ios::beg);

        //read file into buffer
        //buffer = shared_buffer<std::byte>(new std::byte[size], size);
        std::shared_ptr<std::byte []> buffer(new std::byte[size]);

        file.read((char *) buffer.get(), size);

        content_length_line = "Content-Length: " + std::to_string(size) + CRLF + "Connection: close";
        //write the headers
        try{ sock.write(status_line + CRLF + content_length_line + CRLF + content_type_line + CRLF + CRLF); }
        catch( SocketException & e) { std::cerr << "Processing request failed: " << e.what(); sock.close(); return; }

        //Now we write the file contents...
        try{ sock.write(buffer, size); }
        catch( SocketException & e) { std::cerr << "Processing request failed: " << e.what(); sock.close(); return; }
    }
    else
    {
        status_line = "HTTP/1.0 404 Not Found";
        entity_body_line = "<HTML><HEAD><TITLE>Not Found</TITLE></HEAD><BODY>Not Found</BODY></HTML>";
        content_type_line = "Content-Type: text/html; charset=utf-8";
        content_length_line = "Content-Length: " + std::to_string(entity_body_line.length()) + CRLF + "Connection: close";

        try{ sock.write(status_line + CRLF + content_length_line + CRLF + content_type_line + CRLF + CRLF + entity_body_line); }
        catch( SocketException & e) { std::cerr << "Processing request failed: " << e.what(); sock.close(); return; }
    }

    std::cout << "**Response headers:\n" << status_line << '\n' << content_type_line << '\n' << content_length_line << std::endl;

    sock.close();
}
