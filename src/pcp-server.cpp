#include "pcp-server.hpp"
#include "logger.hpp"

#include <sstream>

namespace pcp
{
    void Server::log(std::string &&msg)
    {
        std::lock_guard<std::mutex> lock(m_logmtx);
        std::ostringstream log_msg;
        log_msg
            << "Server;"
            << this << ";"
            << msg << ";"
            << m_buffer.size() << "\n";
        logger::instance() << log_msg.str();
    }

    Server::Server(int prod_workers, int cons_workers)
        : NOF_PRODS(prod_workers), NOF_CONS(cons_workers),
          m_producer(nullptr),
          m_consumer(nullptr),
          m_buffer(),
          m_on_service(true)
    {
        log("ctor");
        for (int i = 0; i < NOF_PRODS + NOF_CONS; i++)
        {
            m_threads[i] = std::thread(&Server::worker_serve, this, i);
        }
    }

    Server::Server(const Server &other)
        : NOF_PRODS(other.NOF_PRODS),
          NOF_CONS(other.NOF_CONS),
          m_producer(other.m_producer),
          m_consumer(other.m_consumer),
          m_on_service(true)
    {
        log("cp ctor");
        for (int i = 0; i < NOF_PRODS + NOF_CONS; i++)
        {
            m_threads[i] = std::thread(&Server::worker_serve, this, i);
        }
    }

    Server::Server(Server &&other)
        : NOF_PRODS(other.NOF_PRODS),
          NOF_CONS(other.NOF_CONS),
          m_producer(other.m_producer),
          m_consumer(other.m_consumer),
          m_on_service(true)
    {
        log("mv ctor");
        m_threads = std::move(other.m_threads);
    }

    Server::~Server()
    {
        m_on_service = false;
        m_awaker.notify_all();
        log("joining workers");
        for (int i = 0; i < m_threads.size(); i++)
        {
            m_threads[i].join();
        }
        m_threads.clear();
        log("dtor");
    }

    bool Server::run(Client *prod, Client *cons)
    {
        if (in_progress())
            return false;
        std::lock_guard<std::mutex> lock(m_bufmtx);
        m_producer = prod;
        m_consumer = cons;
        m_buffer.clear();
        log("running new connection");
        m_awaker.notify_all();
        return true;
    }

    bool Server::in_progress()
    {
        return m_producer ? m_producer->in_progress() : false 
        && m_consumer ? m_consumer->in_progress() : false;
    }

    void Server::worker_serve(int id)
    {
        log("starting worker " + std::to_string(id));
        while (m_on_service)
        {
            Client *client = id < NOF_PRODS ? m_producer : m_consumer;
            while (client != nullptr && client->in_progress() && m_on_service)
            {
                std::lock_guard<std::mutex> lock(m_bufmtx);
                client->server_job(m_buffer);
            }
            std::unique_lock<std::mutex> lock(m_mutexes[id]);
            log("lulling worker " + std::to_string(id));
            m_awaker.wait(lock, [&]
                          { 
                            if(!m_on_service) return true;  
                            else return client ? client->in_progress() : true; 
                          });
            log("awaking worker " + std::to_string(id));
        }

        log("finishing worker " + std::to_string(id));
    }

} // namespace pcp
