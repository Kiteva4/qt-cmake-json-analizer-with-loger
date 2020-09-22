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
        m_logToStream << "--------------------------" << std::endl
                      << " Date: " << std::ctime(&current_time)
                      << " Log:  " << _log << std::endl;

        if (m_logToStream.bad())
        {
            std::cerr << "Unrecoverable write error" << std::endl;
        }
    }
}


