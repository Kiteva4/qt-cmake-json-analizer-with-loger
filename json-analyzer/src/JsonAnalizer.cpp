#include <JsonAnalizer.h>

JsonAnalizer::JsonAnalizer(std::filesystem::path file_path) : worker{file_path}
{
}

JsonAnalizer::~JsonAnalizer()
{
}

void JsonAnalizer::startAnalizing()
{
    if (worker.JsonTest1() && worker.JsonTest2() && worker.JsonTest3() && worker.JsonTest4()
        // && worker.JsonTest5
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