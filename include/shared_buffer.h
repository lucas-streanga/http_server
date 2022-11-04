#ifndef __SHARED_BUFFER
#define __SHARED_BUFFER

#include <array>

//shared buffer class
//Like a shared_ptr for buffers only

template<typename T>
class shared_buffer
{
    T * ptr;
    std::size_t m_size;
    std::size_t * count;

    void cleanup()
    {
        if(*count != 0)
            *count--;
        if (*count == 0)
        {
            if (ptr != nullptr)
                delete [] ptr;
            delete count;
        }
    }

public:
    shared_buffer(T * p, std::size_t s): ptr(p), m_size(s), count(new std::size_t(0)) {}
    shared_buffer(): ptr(nullptr), m_size(0), count(new std::size_t(0)) {}
    // copy assignment (copy-and-swap idiom)
    // copy constructor
    shared_buffer(const shared_buffer<T> & obj)
    {
        this->ptr = obj.ptr; // share the underlying pointer
        this->count = obj.count; // share refCount
        this->m_size = obj.size;
        if (obj.ptr != nullptr)
            (*this->count)++;
    }

    shared_buffer<T>& operator=(const shared_buffer<T> & obj)
    {
        cleanup(); // cleanup any existing data

        // Assign incoming object's data to this object
        this->ptr = obj.ptr; // share the underlying pointer
        this->count = obj.count;
        this->m_size = obj.size;
        if (nullptr != obj.ptr)
        {
            (*this->count)++;
        }
    }

    T* operator->() const
    {
        return this->ptr;
    }
    T& operator*() const
    {
        return this->ptr;
    }

    T * get() const { return ptr; }
    std::size_t size() const { return m_size; }


};

#endif
