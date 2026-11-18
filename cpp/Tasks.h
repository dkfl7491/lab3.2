#ifndef TASKS_H
#define TASKS_H

#include <cstddef>
#include <iosfwd>
#include <string>
#include <utility>
#include <vector>

// Задание 1, вариант 6: обход графа или дерева в глубину со стеком (StackADT).
// Вершины нумеруются с 1, edges — рёбра (u, v). Если directed == false,
// каждое ребро двунаправленное. Порядок совпадает с рекурсивным DFS. O(n + m).
std::vector<int> dfs(int n, const std::vector<std::pair<int, int>>& edges,
                     int start, bool directed);

// Задание 3, вариант 1: разбиение множества натуральных чисел на
// непересекающиеся подмножества с одинаковой суммой.
// target > 0 — требуемая сумма каждого подмножества; target == 0 — наименьшая
// возможная (максимальное число подмножеств). Подмножества хранятся в HashSet
// из задания 2. Бросает std::runtime_error, если разбиения нет.
std::vector<std::vector<long long>> partitionEqualSum(
    const std::vector<long long>& numbers, long long target);

std::vector<std::string> formatSubsets(
    const std::vector<std::vector<long long>>& parts);

// Задание 4, вариант 1: все различные подмассивы (наборы элементов с
// сохранением порядка). Сначала короче, при равной длине — по позициям
// в массиве. Повторы по содержимому выводятся один раз. Каждый подмассив
// возвращается строкой вида "{x, y}". Бросает std::runtime_error при n > 24.
const std::size_t MAX_SUBARRAY_ELEMS = 24;
std::vector<std::string> distinctSubarrays(const std::vector<std::string>& elems);
std::string formatSubarrays(const std::vector<std::string>& subarrays);

// Задание 5, вариант 6: листья бинарного дерева поиска по возрастанию
// (повторы в последовательности игнорируются). Обход — симметричный,
// итеративный, со стеком StackADT.
std::vector<long long> bstLeaves(const std::vector<long long>& sequence);

// Задание 6.2, вариант 6: изоморфные строки на хеш-таблице OpenTable
// (линейное апробирование или двойное хеширование). Работает по символам UTF-8.
bool isomorphic(const std::string& a, const std::string& b, bool doubleHashing);

// Задание 7, вариант 1: читает "cap Q" и Q запросов (SET x y / GET x),
// возвращает результаты GET ("-1", если ключа нет). Бросает std::runtime_error.
std::vector<std::string> runLRU(std::istream& in);

// Задание 6.1: эмпирическое сравнение линейного апробирования и двойного хеширования.
void runHashBenchmark(std::ostream& out);

#endif
