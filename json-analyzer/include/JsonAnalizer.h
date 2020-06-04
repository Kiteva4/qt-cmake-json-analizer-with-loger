#pragma once

#include <filesystem>
#include <iostream>

#include <Json/JsonWorker.h>
#include <JsonAnalizerLoger.h>

class JsonAnalizer
{
public:
    JsonAnalizer(const std::filesystem::path &  file_path);
    ~JsonAnalizer();

    void startAnalizing();
    const JsonAnalizerLoger & loger();
private:
    JsonAnalizerLoger m_loger;
    JsonWorker worker;
};