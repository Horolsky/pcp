#include "pcp-server.hpp"
#include "logger.hpp"

#include <sstream>

static pcp::Producer stup_prod{};
static pcp::Consumer stup_cons{};

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
          m_producer(stup_prod),
          m_consumer(stup_cons),
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

    bool Server::run(Producer& prod, Consumer& cons)
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
        return m_producer.in_progress() && m_consumer.in_progress();
    }

    void Server::worker_serve(int id)
    {
        log("starting worker " + std::to_string(id));
        Client &client = id < NOF_PRODS ? static_cast<Client&>(m_producer) : static_cast<Client&>(m_consumer);
        while (m_on_service)
        {
            while (client.in_progress() && m_on_service)
            {
                std::lock_guard<std::mutex> lock(m_bufmtx);
                client.server_job(m_buffer);
            }
            std::unique_lock<std::mutex> lock(m_mutexes[id]);
            log("lulling worker " + std::to_string(id));
            m_awaker.wait(lock,[&]{ 
                return m_on_service ? client.in_progress() : true;
                });
            log("awaking worker " + std::to_string(id));
        }

        log("finishing worker " + std::to_string(id));
    }

} // namespace pcp
