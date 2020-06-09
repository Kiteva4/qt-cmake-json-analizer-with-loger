#include <Json/JsonUtils.h>
#include <iostream>
#include <filesystem>

#include <arpa/inet.h>

JsonUtils::JsonUtils() {}
JsonUtils::~JsonUtils() {}

bool JsonUtils::isKeyExist(const rapidjson::Document &document, const std::string &key)
{
    return true;
}

void JsonUtils::cleanComments(char *buf, size_t size)
{
    size_t j = 0;
    bool notCopy = false;
    for (size_t i = 0; i < size - 1; i++)
    {
        if (buf[i] == '/' && buf[i + 1] == '*')
        {
            notCopy = true;
        }
        if (!notCopy)
        {
            buf[j++] = buf[i];
        }
        if (buf[i] == '*' && buf[i + 1] == '/')
        {
            notCopy = false;
            i++;
        }
    }
    if (!notCopy)
    {
        buf[j++] = buf[size];
    }
    for (; j < size; j++)
    {
        buf[j] = 0;
    }
}

bool JsonUtils::getJsonDoc(rapidjson::Document &doc, const std::filesystem::path &path)
{
    char readBuffer[1000000] = {};

    FILE *file = fopen(path.c_str(), "rb");

    if (file == nullptr)
        return false;

    fread(readBuffer, 1, 1000000, file);

    cleanComments(readBuffer, 1000000);

    if (doc.Parse(readBuffer).HasParseError())
    {
        std::cout << "Ошибка, формат файла " << path << " некорректен" << std::endl;
        return false;
    }

    fclose(file);

    return true;
}

bool JsonUtils::isIpValidMask(const std::string &mask, const std::string &ip)
{
    uint32_t _mask;
    uint32_t _ip;

    int result_1 = inet_pton(AF_INET, mask.c_str(), &(_mask));
    int result_2 = inet_pton(AF_INET, ip.c_str(), &(_ip));

    bool res = (result_1 != 0 && result_2 != 0);

    bool res_2 = ((_mask & _ip) == _mask);

    return (result_1 != 0 && result_2 != 0 && ((_mask & _ip) == _mask));
}
