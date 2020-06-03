#pragma once

#include <filesystem>
#include <iostream>

#include <Json/JsonWorker.h>

class JsonAnalizer
{
public:
    JsonAnalizer(std::filesystem::path file_path);
    ~JsonAnalizer();

    void startAnalizing();
private:
    JsonWorker worker;
};