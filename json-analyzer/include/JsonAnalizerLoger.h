#pragma once
#include <iostream>
#include <fstream>

#include <string>

class JsonAnalizerLoger
{
public:
    JsonAnalizerLoger();
    ~JsonAnalizerLoger();

    void doLog(std::string _log);


private:

    std::string stream_filename;
    std::ofstream m_logToStream;
    std::ifstream m_readFromStream;
};
