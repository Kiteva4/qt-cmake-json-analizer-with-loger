#include <JsonAnalizerLoger.h>
#include <string>
#include <chrono>

JsonAnalizerLoger::JsonAnalizerLoger()
    : stream_filename{"analyzer_logs.txt"},
      m_logToStream{stream_filename},
      m_readFromStream{stream_filename}
{
    if (!m_logToStream)
        std::cerr << "m_logToStream open Error" << std::endl;

    if (!m_readFromStream)
        std::cerr << "m_readFromStream open Error" << std::endl;
}

JsonAnalizerLoger::~JsonAnalizerLoger()
{
}

void JsonAnalizerLoger::doLog(std::string _log)
{
    std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    if (m_logToStream)
    {
        m_logToStream << "Date: " << std::ctime(&current_time)
                      << "Log:  " << _log
                      << std::endl;

        if (m_logToStream.bad())
        {
            std::cerr << "Unrecoverable write error" << std::endl;
        }
    }

    if(m_readFromStream)
    {
        m_readFromStream.seekg(0, m_readFromStream.end);
        int64_t length = m_readFromStream.tellg();
        m_readFromStream.seekg (0, m_readFromStream.beg);

        char * buffer = new char [length];

        std::cout << "Reading " << length << " characters... ";
        // read data as a block:
        m_readFromStream.read (buffer,length);

        if (m_readFromStream)
            std::cout << "all characters read successfully.";
    }
}
