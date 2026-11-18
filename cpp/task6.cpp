// Задание 6: хеш-таблица (вариант 1: линейное апробирование и двойное
// хеширование), подзадание 6.2 — вариант 6: изоморфные строки.
//
//   ./task6                      две строки на stdin (по одной в строке)
//   ./task6 --table double       то же, но на таблице с двойным хешированием
//   ./task6 --bench              эмпирический анализ методов (подзадание 6.1)
#include "Tasks.h"

#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    bool doubleHashing = false;

    for (int i = 1; i < argc; ++i)
    {
        const std::string argument = argv[i];

        if (argument == "--bench")
        {
            runHashBenchmark(std::cout);
            return 0;
        }

        if (argument == "--table" && i + 1 < argc)
        {
            const std::string kind = argv[++i];

            if (kind == "double")
                doubleHashing = true;
            else if (kind != "linear")
            {
                std::cerr << "ERROR: --table must be linear or double\n";
                return 1;
            }
        }
        else
        {
            std::cerr << "Usage: ./task6 [--table linear|double] | --bench\n";
            return 1;
        }
    }

    std::string first, second;

    if (!std::getline(std::cin, first))
    {
        std::cerr << "ERROR: expected two lines\n";
        return 1;
    }

    std::getline(std::cin, second);

    for (std::string* line : {&first, &second})
        while (!line->empty() && (line->back() == '\r' || line->back() == '\n'))
            line->pop_back();

    std::cout << (isomorphic(first, second, doubleHashing) ? "Изоморфны"
                                                          : "Не изоморфны")
              << '\n';
    return 0;
}
