/**
 * Producer-Consumer Problem sandbox
 * semaphores solution
 */
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <condition_variable>
#include "deque.hpp"

std::mutex buffer_mutex;
std::mutex factory_mutex;
std::atomic<int> fillCount; 
std::atomic<int> emptyCount;
bool logging {false};

class ItemFactory
{
    public:
    ItemFactory() : m_jobs(100) { log("init"); }
    ItemFactory(int jobs) : m_jobs(jobs) { log("init"); }
    
    std::string produce()
    {
        factory_mutex.lock();
        std::string item = "item n. " + std::to_string(++produced_items);
        if (logging) log("produced "+item);
        factory_mutex.unlock();
        return item;
    }
    void consume(std::string item)
    {   
        factory_mutex.lock();
        log("consumed "+item);
        ++consumed_items;
        factory_mutex.unlock();
    }
    bool consuming_in_progress() const 
    {
        return consumed_items < m_jobs;
    }
    bool producing_in_progress() const 
    {
        return produced_items < m_jobs;
    }
    private:
    void log(std::string &&msg)
    {
        if (logging) {
        std::cout << "ItFact [" << this << "] " << msg << std::endl;
        std::cout << " - prod: " << produced_items << ", cons: " << consumed_items << std::endl;
        }
    }
    int produced_items {0};
    int consumed_items {0};
    int m_jobs;
};

class Buffer
{
public:
    static const int CAPACITY{32};
    // Buffer() { m_deque = deque<std::string, CAPACITY>(); log("init"); };
    // ~Buffer() { delete m_deque; }
    void put(std::string item)
    {
        log("put call with " + item);
        m_deque.push_front(item);
        log("pushed " + item);
    }
    std::string remove()
    {
        log("remove call");
        auto item = m_deque.pop_back();
        log("removed " + item);
        return item;
    }
    int size() const
    {
        return m_deque.size();
    }
private:
    deque<std::string, CAPACITY> m_deque {};

    void log(std::string &&msg)
    {
        if (logging) {
        std::cout << "Buffer [" << this << "] " << msg << std::endl;
        std::cout << " - size: " << m_deque.size() << std::endl;
        }
    }
};

ItemFactory factory;
Buffer buffer;

void producer() 
{
    while (factory.producing_in_progress()) 
    {
        buffer_mutex.lock();
        if (emptyCount > 0)
        {
            try
            {
                auto item = factory.produce();
                --emptyCount;
                buffer.put(item);
                ++fillCount;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                std::cerr << "emptyCount: " << emptyCount << '\n';
                exit(1);
            }
        }   
        buffer_mutex.unlock();
    }
}

void consumer() 
{
    while (factory.consuming_in_progress()) 
    {
        buffer_mutex.lock();
        if (fillCount > 0)
        {   
            try
            {
                --fillCount;
                auto item = buffer.remove();
                ++emptyCount;
                factory.consume(item);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
                std::cerr << "fillCount: " << fillCount << '\n';
                exit(1);
            }
        }
        buffer_mutex.unlock();
    }
}


int main (int argc, char* argv[])
{
    if (argc < 4)
    {
        std::cout << "pcp args: <n of jobs> <nof producers> <n of consumers>\n";
    }
    int n_jobs = std::stoi(argv[1]);
    int n_prods = std::stoi(argv[2]);
    int n_cons = std::stoi(argv[3]);
    if (argc == 5 && std::stoi(argv[4]) == 1) logging = true;

    fillCount = 0;
    emptyCount = Buffer::CAPACITY; 
    factory = ItemFactory(n_jobs);
    buffer = Buffer();

    std::thread *prods = new std::thread[n_prods];
    std::thread *cons = new std::thread[n_cons];

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < n_prods; ++i)
    {
        prods[i] = std::thread(producer);
    }
    for (int i = 0; i < n_cons; ++i)
    {
        cons[i] = std::thread(consumer);
    }

    for (int i = 0; i < n_prods; ++i)
    {
        prods[i].join();
    }
    for (int i = 0; i < n_cons; ++i)
    {
        cons[i].join();
    }

    auto end =  std::chrono::steady_clock::now();
    
    std::cout << "working time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    delete[] prods;
    delete[] cons;
    return 0;
}