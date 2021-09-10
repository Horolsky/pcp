#include "pcp-buffer.hpp"
#include "logger.hpp"

namespace pcp
{

    Buffer::Buffer()
        : m_deque()
    {
        log("ctor");
    }
    Buffer::~Buffer()
    {
        log("dtor");
    }

    void Buffer::put(std::string item)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        log("put " + item);
        m_deque.push_front(item);
    }

    std::string Buffer::pop()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        ;
        auto item = m_deque.pop_back();
        log("pop " + item);
        return item;
    }

    int Buffer::size() const
    {
        return m_deque.size();
    }
    int Buffer::capacity() const
    {
        return CAPACITY;
    }
    void Buffer::clear()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_deque.clear();
    }

    void Buffer::log(std::string &&msg)
    {
        logger::instance()
            << "Buffer;"
            << this << ";"
            << msg << ";"
            << m_deque.size() << "\n";
    }

} // namespace pcp