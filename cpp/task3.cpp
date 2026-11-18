// Задание 3, вариант 1: разбиение множества на подмножества с равной суммой.
//
// Ввод (stdin): N, затем N натуральных чисел, затем (необязательно) сумма T
// каждого подмножества. Без T берётся наименьшая возможная сумма.
// Вывод: подмножества по одному в строке.
#include "Tasks.h"

#include <iostream>
#include <stdexcept>

int main()
{
    long long n = 0;

    if (!(std::cin >> n) || n < 0)
    {
        std::cerr << "ERROR: expected: N, N numbers and optionally the target sum T\n";
        return 1;
    }

    std::vector<long long> numbers(static_cast<std::size_t>(n));

    for (long long& x : numbers)
    {
        if (!(std::cin >> x))
        {
            std::cerr << "ERROR: not enough numbers\n";
            return 1;
        }
    }

    long long target = 0;
    std::cin >> target; // необязательный параметр

    try
    {
        for (const std::string& line : formatSubsets(partitionEqualSum(numbers, target)))
            std::cout << line << '\n';
    }
    catch (const std::exception& error)
    {
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
