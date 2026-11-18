// Задание 7, вариант 1: LRU-кэш.
//
// Ввод: "cap Q", затем Q запросов "SET x y" / "GET x" (можно в одну строку
// или по одному в строке). Вывод: результаты GET через пробел.
#include "Tasks.h"

#include <iostream>
#include <stdexcept>

int main()
{
    try
    {
        const std::vector<std::string> output = runLRU(std::cin);

        for (std::size_t i = 0; i < output.size(); ++i)
            std::cout << (i ? " " : "") << output[i];

        std::cout << '\n';
    }
    catch (const std::exception& error)
    {
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
