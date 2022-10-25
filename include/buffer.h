#ifndef __H_BUFFER
#define __H_BUFFER

#include<fstream>


/*Pretty simple header-only utility for us to use
 * Stores data of any type in a buffer and manages the memory
 *
 * Why not just use std::string? Because string types are for characters
 * Our data may not be characters, but file data like an image
 */



template<typename T>
class buffer
{
private:
    T * data;
    std::size_t size;
public:

    buffer(std::fstream & file) noexcept(false)
    {

    }


};

#endif
