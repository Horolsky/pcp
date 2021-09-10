#include "pcp-client.hpp"
#include "logger.hpp"

namespace pcp
{

    void Client::log(std::string &&msg)
    {
        logger::instance()
            << name() << ";"
            << this << ";"
            << msg << ";"
            << m_items << "\n";
    }

    const char *Client::name() const { return "Client"; }

    Client::Client()
        : m_jobs(0), m_items(0)
    {
        log("ctor");
    }

    Client::Client(int jobs)
        : m_jobs(jobs), m_items(0)
    {
        log("ctor");
    }

    Client::~Client() { log("dtor"); }

    Client::Client(Client &&other)
        : m_jobs(other.m_jobs), m_items(other.m_items)
    {
    }

    Client::Client(const Client &other)
        : m_jobs(other.m_jobs), m_items(other.m_items)
    {
    }

    Client &Client::operator=(const Client &other)
    {
        m_jobs = other.m_jobs;
        m_items = other.m_items;
        return *this;
    }
    Client &Client::operator=(Client &&other)
    {
        m_jobs = other.m_jobs;
        m_items = other.m_items;
        return *this;
    }

    bool Client::in_progress()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_items < m_jobs;
    }

    void Client::reset(int jobs)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_items = 0;
        m_jobs = jobs;
    }

    const char *Producer::name() const { return "Producer"; }
    const char *Consumer::name() const { return "Consumer"; }

    std::string Producer::produce()
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        std::string item = "item n. " + std::to_string(++m_items);
        log("produced " + item);
        return item;
    }
    void Consumer::consume(std::string item)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        ++m_items;
        log("consumed " + item);
    }

    bool Producer::server_job(Buffer &buffer)
    {
        if (buffer.capacity() - buffer.size() > 0)
        {
            buffer.put(produce());
            return true;
        }
        return false;
    }

    bool Consumer::server_job(Buffer &buffer)
    {
        if (buffer.size() > 0)
        {
            consume(buffer.pop());
            return true;
        }
        return false;
    }

} // namespace pcp
