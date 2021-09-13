#pragma once
#include "deque.hpp"
#include "pcp-fwd.hpp"
#include "pcp-buffer.hpp"
#include "pcp-client.hpp"
#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <functional>
#include <thread>
#include <vector>
#include <utility>

namespace pcp
{

    class Server
    {
    public:
        
        Server(int prod_workers, int cons_workers);
        Server(const Server &other);
        Server(Server &&other);
        ~Server();

        bool run(int jobs);
        bool in_progress();

    private:
        std::condition_variable m_awaker;
        std::map<int,std::thread> m_threads;
        std::map<int,std::mutex> m_mutexes;

        //TODO: multiple polymorphic PCP clients
        Producer m_producer;
        Consumer m_consumer;
        Buffer m_buffer;

        std::mutex m_bufmtx;
        std::mutex m_logmtx;

        const int NOF_PRODS;
        const int NOF_CONS;
        bool m_on_service;

        void log(std::string &&msg);
        void worker_serve(int id);
    };

} // namespace pcp
