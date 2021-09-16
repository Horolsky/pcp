#include "logger.hpp" 

static logger* INSTANCE {nullptr};
static std::ofstream OFSTREAM;

std::string logger::_path = "cout";

logger& logger::instance()
{
    if (!INSTANCE) INSTANCE = new logger();
    return *INSTANCE;
}

logger::~logger()
{
        std::lock_guard<std::mutex> lock(_mtx);
        if(OFSTREAM.is_open()) OFSTREAM.close();
        if (INSTANCE) delete INSTANCE;
}
void logger::reset(std::string &&output)
{
    if (output == "cout")
    {
        m_out = &std::cout;
        _path = output;
    }
    else if (output == "cerr")
    {
        m_out = &std::cerr;
        _path = output;
    }
    else 
    {
        try
        {
            OFSTREAM.open(output, std::ios::out | std::ios::trunc);
            m_out = &OFSTREAM;
            _path = output;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }   
    }
}
