#pragma once

#include <string>
#include <filesystem>

#include "rapidjson/document.h"

class JsonUtils
{
public:
    JsonUtils();
    ~JsonUtils();

    bool getJsonDoc(rapidjson::Document &doc, const std::filesystem::path &path);
    bool isIpValidMask(const std::string &mask, const std::string &ip);
private:
    bool isKeyExist(const rapidjson::Document &document, const std::string &key);
    static void cleanComments(char *buf, size_t size);
};