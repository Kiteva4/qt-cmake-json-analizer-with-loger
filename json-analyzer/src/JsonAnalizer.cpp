#include <JsonAnalizer.h>

JsonAnalizer::JsonAnalizer(const std::filesystem::path &file_path) :
    m_loger{},
    worker(file_path)
{
}

JsonAnalizer::~JsonAnalizer()
{
}

const JsonAnalizerLoger &JsonAnalizer::loger()
{
    return m_loger;
}

void JsonAnalizer::set_print_error(bool _flag)
{
    worker.set_print_error(_flag);
}

void JsonAnalizer::set_print_ok(bool _flag)
{
    worker.set_print_ok(_flag);
}

void JsonAnalizer::startAnalizing()
{
    if (worker.JsonTest1() &&
        worker.JsonTest2() &&
        worker.JsonTest3() &&
        worker.JsonTest4() &&
        worker.JsonTest5() 
        // && worker.JsonTest6
    )
    {
        std::cout << "SUCCESS" << std::endl;
    }
    else
    {
        std::cout << "ERROR" << std::endl;
    }
}
