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
        struct Work
        {
            Client &client;
            std::mutex mtx;
            Work(Client &client) : client(client){};

            Work(const Work &other) : client(other.client){};
            Work(Work &&other) : client(other.client){};
        };
        using client_in_progress_fp = bool (Client::*)() const;
        Server(
            int prod_workers,
            int cons_workers);

        Server(const Server &other);
        Server(Server &&other);
        ~Server();

        bool run(int nof_items);
        bool in_progress();

    private:
        std::condition_variable m_awaker;
        std::vector<std::thread> m_threads;
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
