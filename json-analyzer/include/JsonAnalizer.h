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

    void set_print_error(bool _flag);
    void set_print_ok(bool _flag);

private:
    JsonAnalizerLoger m_loger;
    JsonWorker worker;

};
