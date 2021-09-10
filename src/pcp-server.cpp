#include "pcp-server.hpp"
#include "pcp-logger.hpp"

// template class std::vector<std::pair<std::thread, Work>>;

Server::Server(int prod_workers, int cons_workers) 
: NOF_PRODS(prod_workers), NOF_CONS(cons_workers),
                            m_producer(),
                            m_consumer(),
                            m_buffer(),
                            m_on_service(true)
{
    m_threads = std::vector<std::thread>();
    m_works = std::vector<Work>();

    for (int i = 0; i < NOF_PRODS; i++)
    {
        m_works.emplace_back<Work>({m_producer});
        m_threads.emplace_back(&Server::worker_serve,this,i);
    }
    for (int i = NOF_PRODS; i < NOF_PRODS+NOF_CONS; i++)
    {
        m_works.emplace_back<Work>({m_consumer});
        m_threads.emplace_back(&Server::worker_serve,this,i);
    }
}

Server::~Server()
{
    m_on_service = false;
    for (int i = 0; i < m_threads.size(); i++)
    {
        m_threads[i].join();
    }
    m_works.clear();
    m_threads.clear();
}

Server::Server(const Server &other)
: NOF_PRODS(other.NOF_PRODS), 
NOF_CONS(other.NOF_CONS), 
m_producer(other.m_producer), 
m_consumer(other.m_consumer),
m_on_service(true)
{
    m_threads = std::vector<std::thread>();
    m_works = std::vector<Work>();

    for (int i = 0; i < NOF_PRODS; i++)
    {
        m_works.emplace_back<Work>({m_producer});
        m_threads.emplace_back(&Server::worker_serve,this,i);
    }
    for (int i = NOF_PRODS; i < NOF_PRODS+NOF_CONS; i++)
    {
        m_works.emplace_back<Work>({m_consumer});
        m_threads.emplace_back(&Server::worker_serve,this,i);
    }
}
Server::Server(Server &&other)
: NOF_PRODS(other.NOF_PRODS), 
NOF_CONS(other.NOF_CONS), 
m_producer(other.m_producer), 
m_consumer(other.m_consumer),
m_on_service(true)
{
    m_threads = std::move(other.m_threads);
    m_works = std::move(other.m_works);
}

bool Server::run(int nof_items)
{
    if (in_progress())
        return false;
    m_producer.reset(nof_items);
    m_consumer.reset(nof_items);
    m_buffer.clear();
    m_awaker.notify_all();
    return true;
}

bool Server::in_progress()
{
    return m_producer.in_progress() && m_consumer.in_progress();
}

void Server::log(std::string &&msg)
{
    logger::instance()
        << "Server;"
        << this << ";"
        << msg << ";"
        << m_buffer.size() << "\n";
}

void Server::worker_reset(int id, Client& client)
{
    std::unique_lock<std::mutex> lock(m_works[id].mtx);
    m_works[id].client = client;
}
void Server::worker_serve(int id)
{
    while (m_on_service)
    {
        while (m_works[id].client.in_progress() || !m_on_service)
        {
            std::lock_guard<std::mutex> lock(m_bufmtx);
            m_works[id].client.server_job(m_buffer);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        // std::unique_lock<std::mutex> lock(m_works[id].mtx);
        // m_awaker.wait(lock,[&]{ return (m_works[id].client.in_progress() || !m_on_service ); });
    }
}

