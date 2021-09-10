/**
 * Producer-Consumer Problem sandbox
 * semaphores solution
 */
#include "pcp-server.hpp"
#include "logger.hpp"

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout
            << "pcp args:               \n"
            << "-j <jobs, def:10000>    \n"
            << "-p <producers, def:10>  \n"
            << "-c <consumers, def:10   \n"
            << "-o <output, def:cout>   \n";
        return 0;
    }

    std::array<std::pair<std::string, unsigned int>, 3> numeric_flags{
        std::pair<std::string, unsigned int>("-j", 10000U),
        std::pair<std::string, unsigned int>("-p", 10U),
        std::pair<std::string, unsigned int>("-c", 10U)};
    const std::string output_flag{"-o"};

    for (int i = 0; i < argc; ++i)
    {
        if (output_flag == argv[i])
        {
            logger::instance().reset(argv[++i]);
        }
        else
        {
            for (auto &flag : numeric_flags)
            {
                if (flag.first == argv[i])
                    try
                    {
                        flag.second = std::stoul(argv[++i]);
                    }
                    catch (const std::exception &e)
                    {
                        std::cout << "invalid " + flag.first + " argument\n";
                        return 0;
                    }
            }
        }
    }

    int n_jobs = numeric_flags[0].second;
    int n_prods = numeric_flags[1].second;
    int n_cons = numeric_flags[2].second;

    std::cout
        << "task params:\n"
        << "jobs:" << n_jobs << "\n"
        << "prods:" << n_prods << "\n"
        << "cons:" << n_cons << "\n";

    //csv header
    logger::instance() << "class;id;msg;items\n";
    
    auto start = std::chrono::steady_clock::now();
    {
        pcp::Server server{n_prods, n_cons};
        server.run(n_jobs);
        int period = 5 * n_jobs;
        if (period > 100)
            period = 100;
        while (server.in_progress())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(period));
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::cout << "working time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    return 0;
}