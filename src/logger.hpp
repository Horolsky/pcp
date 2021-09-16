#pragma once

#include <fstream>
#include <iostream>
#include <ostream>
#include <mutex>
#include <string>

class logger
{
public:
    static logger &instance();
    void reset(std::string &&output);
    ~logger();
    std::ostream *m_out;
    std::mutex _mtx;

private:
    static logger *_instance_p;
    static std::string _path;

    logger() { m_out = &std::cout; };

    template <typename T>
    friend logger &operator<<(logger &log, const T &item)
    {
        std::lock_guard<std::mutex> lock(log._mtx);
        *(log.m_out) << item;
        return log;
    }

    friend logger &operator<<(logger &log, std::ostream &(*function)(std::ostream &))
    {
        std::lock_guard<std::mutex> lock(log._mtx);
        *(log.m_out) << function;
        return log;
    }
};
