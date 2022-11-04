/*
 *
 * Utility to cache up to a certain number of files. can be very useful */
#ifndef __FILE_CACHE
#define __FILE_CACHE

#include<memory>
#include<array>

template<std::size_t max_cache>
class file_cache
{
    struct file_data
    {
        std::string filename;
        std::size_t size;
        std::shared_ptr<std::byte []> data;
    };

    std::array<file_data, max_cache> cache;
    std::size_t cur;
public:
    file_cache()
    {
        cur = 0;
        for(auto t: cache)
        {
            t.filename = "";
            t.size = 0;
            t.data = nullptr;
        }
    }

    std::shared_ptr<std::byte []> get(const std::string & filename)
    {
        for(auto t: cache)
            if(t.filename == filename)
                return t.data;
        return nullptr;
    }

    std::size_t size(const std::string & filename)
    {
        for(auto t: cache)
            if(t.filename == filename)
                return t.size;
        return 0;
    }

    void insert(const std::string & filename, std::shared_ptr<std::byte []> data, std::size_t size)
    {
        std::size_t new_cur = (cur == max_cache)? 0 : cur + 1;
        cache[new_cur].filename = filename;
        cache[new_cur].data = data;
        cache[new_cur].size = size;
        cur = new_cur;
    }

};

#endif
