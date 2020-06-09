#include <Json/JsonWorker.h>
#include <map>

JsonWorker::JsonWorker(
    const std::filesystem::path &main_json_path)
    : jsonUtils{},
      main_doc{},
      m_loger{},
      print_error{false},
      print_ok{false}
{
    jsonUtils.getJsonDoc(main_doc, main_json_path);
}

JsonWorker::~JsonWorker()
{
}

void JsonWorker::set_print_error(bool _flag)
{
    print_error = _flag;
}

void JsonWorker::set_print_ok(bool _flag)
{
    print_ok = _flag;
}

void JsonWorker::extract_log(const std::string &str)
{
    if(print_ok)
    {
        // QMessageBox::information(&widget, "Error", str.c_str());
        m_loger.doLog(str);
        std::cout << str << std::endl;
    }
}

void JsonWorker::extract_exeption(const std::string &str)
{
    if(print_error)
    {
        // QMessageBox::information(&widget, "Error", str.c_str());
        m_loger.doLog(str);
        std::cerr << str << "\n";
    }
}

bool JsonWorker::JsonTest1()
{
    bool result = true;

    /* Проверка наличия поля "blocks" */
    if (!main_doc.HasMember("blocks"))
    {
        extract_exeption("ERROR: поле 'block' не существует!");
    }
    /* Проверка соответствия типа "blocks" типу Array */
    else if (!main_doc["blocks"].IsArray())
    {
        extract_exeption("ERROR: поле 'block' не является массивом!");
    }
    /* Проверка количества элементов "blocks" */
    else if (main_doc["blocks"].GetArray().Size() == 0)
    {
        extract_exeption("ERROR: поле 'block' является пустым массивом!");
    }
    else
    {
        extract_log("OK:объект 'block' корректен");
    }

    /* Проверка поля " block_name " у каждого элемента массива */
    for (auto it = main_doc["blocks"].Begin(); it != main_doc["blocks"].End(); it++)
    {

        if (!it->HasMember("block_name"))
        {
            extract_exeption("ERROR: объект массива 'blocks' не включает поле 'block_name'");
        }

        if (!it->GetObject()["block_name"].IsString())
        {
            extract_exeption("ERROR: объект 'block_name' массива 'blocks' не является строкой");
        }

        if (it->GetObject()["block_name"] == "")
        {
            extract_exeption("ERROR: объект 'block_name' массива 'blocks' является пустой строкой");
        }

        extract_log("OK:" + std::string(it->GetObject()["block_name"].GetString()) + " корректен ");
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
            extract_exeption("ERROR: поле 'broadcast_ip' не существует");
        }
        /* Вывод ошибки если в документе поле "blocks" не является типом Array */
        else if (!main_doc["broadcast_ip"].IsString())
        {
            extract_exeption("ERROR: поле 'broadcast_ip' не является строкой");
        }
        /* Вывод ошибки если в документе поле "blocks" является пустой строкой */
        else if (main_doc["broadcast_ip"] == "")
        {
            extract_exeption("ERROR: поле 'broadcast_ip' является пустой строкой");
        }
        else
        {
            extract_log("ОК:объект 'broadcast_ip' корректен");
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
                extract_exeption("ERROR: в модуле " + block_name + "поле 'ip' отсутствует");
            }
            /* Вывод ошибки если поле "ip" не является типом String */
            else if (!it->GetObject()["ip"].IsString())
            {
                extract_exeption("ERROR: в модуле " + block_name + " поле 'ip' не является строкой");
            }
            /* Вывод ошибки если поле "ip" является пустой строкой */
            else if (it->GetObject()["ip"] == "")
            {
                extract_exeption("ERROR: в модуле " + block_name + "поле 'ip' является пустой строкой");
            }
            /* Вывод ошибки если поле "ip" не входит в маску broadcast_ip */
            else if (!jsonUtils.isIpValidMask(it->GetObject()["ip"].GetString(), main_doc["broadcast_ip"].GetString()))
            {
                extract_exeption("ERROR: в модуле " + block_name + "поле 'ip' не соотвутетвует диапазону 'broadcast_ip'");
            }
            else
            {
                extract_log(std::string{"OK:поле 'ip' в модуле" + block_name + " корректно "});
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
            std::string block_name(std::string(it->GetObject()["block_name"].GetString()));

            /* Вывод ошибки если поле "ip" существует, но не является Array */
            if (it->HasMember("in"))
            {
                /* Вывод ошибки если поле "ip" не является типом Array */
                if (!it->GetObject()["in"].IsArray())
                {
                    extract_exeption("ERROR: в модуле " + block_name + " объект 'in' существует, но не является массивом");
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
                extract_exeption("ERROR: в модуле " + block_name + " объект 'out' отсутствует");
            }
            /* Вывод ошибки если поле "ip" не является типом Array */
            else if (!it->GetObject()["out"].IsArray())
            {
                extract_exeption("ERROR: в модуле " + block_name + " объект 'out' не является массивом");
            }
            /* Вывод ошибки если поле "ip" является пустой строкой */
            else if (it->GetObject()["out"].GetArray().Size() == 0)
            {
                extract_exeption("ERROR: в модуле " + block_name + " объект 'out' является пустым массивом");
            }
            else
            {
                extract_log(std::string{"OK:блок 'out' в модуле " + block_name + " корректен"});
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

            for (auto a = it->GetObject()["in"].GetArray().Begin(); a != it->GetObject()["in"].GetArray().End(); a++)
            {
                in_ports[a->GetObject()["port"].GetInt()]++;
            }

            for (const auto &a : in_ports)
            {
                if (a.second > 1)
                {
                    extract_exeption("ERROR: в модуле " + block_name + "port '" +
                                     std::to_string(a.first) +
                                     "' используется " +
                                     std::to_string(a.second) +
                                     " раз(а)");
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
                std::string dest_block(a->GetObject()["dest_block"].GetString());

                if (dest_block.find("SELF") != std::string::npos)
                {
                    // std::cout << "Найден " << it->GetObject()["block_name"].GetString() << " : SELF" << std::endl;

                    if (!in_ports_cache[it->GetObject()["block_name"].GetString()].contains(a->GetObject()["dest_port"].GetInt()))
                    {
                        // В блоке it->GetObject()["block_name"].GetString() не заложен прием для запрашиваемого порта a->GetObject()["dest_port"].GetInt()
                        extract_exeption("ERROR: для модуля в [out] " +
                                         block_name +
                                         " не заложен прием для запрашиваемого порта " +
                                         std::to_string(a->GetObject()["dest_port"].GetInt()) +
                                         " сокета " +
                                         std::string(a->GetObject()["name"].GetString()) +
                                         " у модуля в [in]" +
                                         std::string(a->GetObject()["dest_block"].GetString()));
                        continue;
                    }
                    else
                    {
                        // В блоке it->GetObject()["block_name"].GetString() заложен прием для запрашиваемого порта a->GetObject()["dest_port"].GetInt()
                        extract_log("OK:в блоке " +
                                    std::string(it->GetObject()["block_name"].GetString()) +
                                    " заложен прием для запрашиваемого порта " +
                                    std::to_string(a->GetObject()["dest_port"].GetInt()) +
                                    " сокета " +
                                    std::string(a->GetObject()["name"].GetString()));
                        continue;
                    }
                }
                
                for (const auto &it_cache : in_ports_cache)
                {
                    if (dest_block.find(it_cache.first) != std::string::npos)
                    {
                        // std::cout << "Найден запрашиваемый " << dest_block << " :> " <<  it_cache.first << "obj: " << it->GetObject()["block_name"].GetString() << std::endl;

                        if (!in_ports_cache[it_cache.first].contains(a->GetObject()["dest_port"].GetInt()))
                        {
                            // В блоке it->GetObject()["block_name"].GetString() не заложен прием для запрашиваемого порта a->GetObject()["dest_port"].GetInt()
                            extract_exeption("ERROR: для модуля " +
                                             it_cache.first +
                                             " не заложен прием для запрашиваемого порта " +
                                             std::to_string(a->GetObject()["dest_port"].GetInt()) +
                                             " сокета " +
                                             std::string(a->GetObject()["name"].GetString()) +
                                             " у модуля " +
                                             std::string(a->GetObject()["dest_block"].GetString()));
                        }
                        else
                        {
                            // В блоке it_cache.first заложен прием для запрашиваемого порта a->GetObject()["dest_port"].GetInt()
                            extract_log("OK:в блоке " +
                                        it_cache.first +
                                        " заложен прием для запрашиваемого порта " +
                                        std::to_string(a->GetObject()["dest_port"].GetInt()) +
                                        " сокета " +
                                        std::string(a->GetObject()["name"].GetString()));
                        }
                    }
                    // else
                    // {
                    //     // Блок dest_block содержит недействительные данные
                    //     extract_exeption("ERROR: в блоке " + dest_block + " присутствуют недействительные данные. " + it_cache.first +"-> такой блок не найден");
                    // }

                    // std::cout << dest_block << std::endl;
                }
            }
        }
    }

    return result;
}

bool JsonWorker::JsonTest6() { return false; }
