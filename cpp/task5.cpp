// Задание 5, вариант 6: листья бинарного дерева поиска.
//
// Ввод: целые числа, оканчивающиеся нулём (0 в дерево не входит).
// Вывод: листья по возрастанию.
#include "Tasks.h"

#include <iostream>

int main()
{
    std::vector<long long> sequence;
    long long value = 0;

    while (std::cin >> value && value != 0)
        sequence.push_back(value);

    const std::vector<long long> leaves = bstLeaves(sequence);

    for (std::size_t i = 0; i < leaves.size(); ++i)
        std::cout << (i ? " " : "") << leaves[i];

    std::cout << '\n';
    return 0;
}
