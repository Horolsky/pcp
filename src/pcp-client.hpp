#pragma once
#include "pcp-fwd.hpp"
#include "pcp-buffer.hpp"
#include <mutex>
#include <string>

class Client
{
protected:
    std::mutex m_mtx;
    int m_items;
    int m_jobs;
    void log(std::string &&msg);
    virtual const char *name() const;

public:
    Client();
    Client(int jobs);
    virtual ~Client();

    Client(Client &&other);
    Client(const Client &other);
    Client &operator=(const Client &other);
    Client &operator=(Client &&other);


    bool in_progress();
    virtual bool server_job(Buffer &buffer) {return false; };
    void reset(int jobs);
};

class Producer : public Client
{
    virtual const char *name() const;

public:
    using Client::Client;
    std::string produce();
    bool server_job(Buffer &buffer);
};

class Consumer : public Client
{
    virtual const char *name() const;

public:
    using Client::Client;
    void consume(std::string item);
    bool server_job(Buffer &buffer);
};