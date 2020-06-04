#include <Json/JsonWorker.h>
#include <map>

JsonWorker::JsonWorker(
    const std::filesystem::path &main_json_path)
    : jsonUtils{},
      main_doc{},
      m_loger{}
{
    jsonUtils.getJsonDoc(main_doc, main_json_path);
}

JsonWorker::~JsonWorker()
{
}

void JsonWorker::extract_log(const std::string &str)
{
    // QMessageBox::information(&widget, "Error", str.c_str());
    m_loger.doLog(str);
    std::cout << str << std::endl;
}

void JsonWorker::extract_exeption(const std::string &str)
{
    // QMessageBox::information(&widget, "Error", str.c_str());
    m_loger.doLog(str);
    throw std::runtime_error(str);
}

bool JsonWorker::JsonTest1()
{
    bool result = true;

    /* Проверка наличия поля "blocks" */
    if (!main_doc.HasMember("blocks"))
    {
        extract_exeption("Json Incorrect: \"block\" don`t exist !");
    }
    /* Проверка соответствия типа "blocks" типу Array */
    else if (!main_doc["blocks"].IsArray())
    {
        extract_exeption("Json Incorrect: \"block\" is not a Array type!");
    }
    /* Проверка количества элементов "blocks" */
    else if (main_doc["blocks"].GetArray().Size() == 0)
    {
        extract_exeption("Json Incorrect: \"block\" is empty Array!");
    }
    else
    {
        extract_log("object 'block' exist and has Array type");
    }

    /* Проверка поля " block_name " у каждого элемента массива */
    for (auto it = main_doc["blocks"].Begin(); it != main_doc["blocks"].End(); it++)
    {

        if (!it->HasMember("block_name"))
        {
            extract_exeption("Json Incorrect: element of 'blocks' Array don`t have 'block_name' module");
        }

        if (!it->GetObject()["block_name"].IsString())
        {
            extract_exeption("Json Incorrect: element 'block_name' of 'blocks' Array is not String format");
        }

        if (it->GetObject()["block_name"] == "")
        {
            extract_exeption("Json Incorrect: element 'block_name' in 'blocks' Array is empty string");
        }
    }

    return result;
}

bool JsonWorker::JsonTest2()
{
    bool result = true;

    /* Валидация поля "broadcast_ip" */
    {
        /* Вывод ошибки если в документе отсутствует поле "blocks" */
        if (!main_doc.HasMember("broadcast_ip"))
        {
            extract_exeption("Json Incorrect: \"broadcast_ip\" don`t exist !");
        }
        /* Вывод ошибки если в документе поле "blocks" не является типом Array */
        else if (!main_doc["broadcast_ip"].IsString())
        {
            extract_exeption("Json Incorrect: \"broadcast_ip\" is not a String type!");
        }
        /* Вывод ошибки если в документе поле "blocks" является пустой строкой */
        else if (main_doc["broadcast_ip"] == "")
        {
            extract_exeption("Json Incorrect: \"broadcast_ip\" is a empty String!");
        }
        else
        {
            extract_log("object 'broadcast_ip' exist and has non-empty String type");
        }
    }

    /* Валидация диапазонов IP */
    {
        for (auto it = main_doc["blocks"].Begin(); it != main_doc["blocks"].End(); it++)
        {
            std::string block_name(std::string(it->GetObject()["block_name"].GetString()));
            /* Вывод ошибки если в отсутствует поле "ip" */
            if (!it->HasMember("ip"))
            {
                extract_exeption(block_name + " Json Incorrect: element of 'ip' Array don`t have 'ip' module");
            }
            /* Вывод ошибки если поле "ip" не является типом String */
            else if (!it->GetObject()["ip"].IsString())
            {
                extract_exeption(block_name + " Json Incorrect: element 'ip' of 'blocks' Array is not String format");
            }
            /* Вывод ошибки если поле "ip" является пустой строкой */
            else if (it->GetObject()["ip"] == "")
            {
                extract_exeption(block_name + " Json Incorrect: element 'ip' of 'blocks' Array is empty string");
            }
            /* Вывод ошибки если поле "ip" не входит в маску broadcast_ip */
            else if (!jsonUtils.isIpValidMask(it->GetObject()["ip"].GetString(), main_doc["broadcast_ip"].GetString()))
            {
                extract_exeption(block_name + " Json Incorrect: 'ip' don`t included on vadil mask 'broadcast_ip' ");
            }
            else
            {
                extract_log(std::string{"'ip' on mudule " + block_name + " is correct"});
            }
        }
    }

    return result;
}

bool JsonWorker::JsonTest3()
{
    bool result = true;

    /* Валидация in */
    {
        for (auto it = main_doc["blocks"].Begin(); it != main_doc["blocks"].End(); it++)
        {
            std::string block_name("block " + std::string(it->GetObject()["block_name"].GetString()));

            /* Вывод ошибки если поле "ip" существует, но не является Array */
            if (it->HasMember("in"))
            {
                /* Вывод ошибки если поле "ip" не является типом Array */
                if (!it->GetObject()["in"].IsArray())
                {
                    extract_exeption(block_name + " Json Incorrect: element 'in' of 'blocks' Array is not Array format");
                }
            }
        }

        /* Валидация out */
        for (auto it = main_doc["blocks"].Begin(); it != main_doc["blocks"].End(); it++)
        {
            std::string block_name(std::string(it->GetObject()["block_name"].GetString()));
            /* Вывод ошибки если в отсутствует поле "ip" */
            if (!it->HasMember("out"))
            {
                extract_exeption(block_name + " Json Incorrect: element of 'out' Array don`t have 'in' module");
            }
            /* Вывод ошибки если поле "ip" не является типом Array */
            else if (!it->GetObject()["out"].IsArray())
            {
                extract_exeption(block_name + " Json Incorrect: element 'out' of 'blocks' Array is not Array format");
            }
            /* Вывод ошибки если поле "ip" является пустой строкой */
            else if (it->GetObject()["out"].GetArray().Size() == 0)
            {
                extract_exeption(block_name + " Json Incorrect: element 'out' of 'blocks' Array is empty Array");
            }
            else
            {
                extract_log(std::string{"'out' on mudule " +block_name + "is correct Array"});
            }
        }
    }

    return result;
}

bool JsonWorker::JsonTest4()
{
    bool result = true;
    std::map<uint32_t, uint32_t> in_ports;

    /* Валидация портов в блоке in у всех устройств */
    {
        for (auto it = main_doc["blocks"].Begin(); it != main_doc["blocks"].End(); it++)
        {
            std::string block_name("block " + std::string(it->GetObject()["block_name"].GetString()));

            std::cout << block_name << std::endl;
            for (auto a = it->GetObject()["in"].GetArray().Begin(); a != it->GetObject()["in"].GetArray().End(); a++)
            {
                in_ports[a->GetObject()["port"].GetInt()]++;
            }

            for (const auto &a : in_ports)
            {
                if (a.second > 1)
                {
                    extract_exeption(block_name + " Json Incorrect: port '" + std::to_string(a.first) + "' is used " + std::to_string(a.second) + " time");
                }
            }

            in_ports.clear();
        }
    }

    return result;
}

bool JsonWorker::JsonTest5()
{
    bool result = true;

    std::map<std::string, std::map<uint32_t, uint32_t>> in_ports_cache;

    /* Кешируем данные */
    {
        for (auto it = main_doc["blocks"].Begin(); it != main_doc["blocks"].End(); it++)
        {
            std::map<uint32_t, uint32_t> block_in_port_cache;
            std::string block_name(std::string(it->GetObject()["block_name"].GetString()));

            for (auto a = it->GetObject()["in"].GetArray().Begin(); a != it->GetObject()["in"].GetArray().End(); a++)
            {
                block_in_port_cache[a->GetObject()["port"].GetInt()]++;
            }

            in_ports_cache[block_name] = block_in_port_cache;
        }
    }

    /* Проверяем на наличие соответствующего порта, указанного блоке out в блоке in*/
    {
        for (auto it = main_doc["blocks"].Begin(); it != main_doc["blocks"].End(); it++)
        {
            std::map<uint32_t, int32_t> block_in_port_cache;
            std::string block_name(std::string(it->GetObject()["block_name"].GetString()));

            for (auto a = it->GetObject()["out"].GetArray().Begin(); a != it->GetObject()["out"].GetArray().End(); a++)
            {
                if (!in_ports_cache[(a->GetObject()["dest_block"]) == "SELF" ? block_name : a->GetObject()["dest_block"].GetString()].contains(a->GetObject()["dest_port"].GetInt()))
                {
                    extract_exeption(block_name + " Json Incorrect: port '" + std::to_string(a->GetObject()["dest_port"].GetInt()) + "' is not find on 'in' block of: " + a->GetObject()["dest_block"].GetString());
                }
            }
        }
    }

    return result;
}

bool JsonWorker::JsonTest6() { return false; }
