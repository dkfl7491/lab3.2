// Задание 4, вариант 1: все различные подмассивы массива.
//
// Ввод (stdin): элементы массива через пробелы. Вывод: [{}, {x}, {x, y}, ...].
#include "Tasks.h"

#include <iostream>
#include <stdexcept>

int main()
{
    std::vector<std::string> elems;
    std::string word;

    while (std::cin >> word)
        elems.push_back(word);

    try
    {
        std::cout << formatSubarrays(distinctSubarrays(elems)) << '\n';
    }
    catch (const std::exception& error)
    {
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
