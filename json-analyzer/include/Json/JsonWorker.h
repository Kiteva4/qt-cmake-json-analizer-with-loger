#pragma once

#include <filesystem>

#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"

#include <Json/JsonUtils.h>

class JsonWorker
{
public:
    JsonWorker(const std::filesystem::path & main_json_path);
    ~JsonWorker();

    /* 1. Проверить для каждого блока (из blocks) проверить block_name на наличие (не пусто и что является строкой) */
    bool JsonTest1();
    /* 2. Проверить для каждого блока (из blocks) наличие "ip" (есть и является строкой) и соответствует маске корневого объекта "broadcast_ip" */
    bool JsonTest2();
    /* 3. Проверить для каждого блока (из blocks) наличие "in" и "out" (есть и являются массивами) */
    bool JsonTest3();
    /* 4. Проверить что для каждого блока (из blocks) массивы in на уникальность port */
    bool JsonTest4();
    /* 5. Проверить что для каждого блока (из blocks) массивы out наличие в массиве in соответствующего принимаего устройства */
    bool JsonTest5();
    /* 6. Проверить что для каждого блока (из blocks) массивы out на уникальность dest_port в рамках одного dest_block */
    bool JsonTest6();


private:

    void extract_exeption(const std::string & str);

    JsonUtils jsonUtils;
    rapidjson::Document main_doc;
};

/*
1. Проверить для каждого блока (из blocks) проверить block_name на наличие (не пусто и что является строкой)
2. Проверить для каждого блока (из blocks) наличие "ip" (есть и является строкой) и соответствует маске корневого объекта "roadcast_ip"
3. Проверить для каждого блока (из blocks) наличие "in" и "out" (есть и являются массивами)
4. Проверить что для каждого блока (из blocks) массивы in на уникальность port
5. Проверить что для каждого блока (из blocks) массивы out наличие в массиве in соответствующего принимаего устройства 
6. Проверить что для каждого блока (из blocks) массивы out на уникальность dest_port в рамках одного dest_block
*/
