#pragma once
#include "deque.hpp"
#include "pcp-fwd.hpp"
#include <mutex>

class Buffer
{
public:
    static const int CAPACITY{32};
    std::mutex m_mtx;
    
    Buffer();
    ~Buffer();

    void put(std::string item);

    std::string pop();

    int size() const;
    int capacity() const;
    void clear();

private:
    deque<std::string, CAPACITY> m_deque{};

    void log(std::string &&msg);
};
