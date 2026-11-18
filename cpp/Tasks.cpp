#include "Tasks.h"

#include "HashSet.h"
#include "OpenTable.h"
#include "Structures.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <istream>
#include <ostream>
#include <random>
#include <stdexcept>

// ---------------------------------------------------------------------------
// Задание 1, вариант 6. DFS со стеком.
// ---------------------------------------------------------------------------

// В стек кладётся стартовая вершина. Пока стек не пуст, вершина снимается;
// если она ещё не посещена, она отмечается и выводится, а её непосещённые
// соседи кладутся в стек в обратном порядке, чтобы первым обработался первый
// сосед. StackADT из ЛР1 хранит строки, поэтому номера вершин хранятся как
// строки. У StackADT нет size(), глубину стека считаем отдельно.
std::vector<int> dfs(int n, const std::vector<std::pair<int, int>>& edges,
                     int start, bool directed)
{
    if (start < 1 || start > n)
        throw std::runtime_error("Start vertex is out of range");

    std::vector<ArrayADT> adjacency(static_cast<std::size_t>(n) + 1);

    for (const auto& [u, v] : edges)
    {
        if (u < 1 || u > n || v < 1 || v > n)
            throw std::runtime_error("Edge vertex is out of range");

        adjacency[u].push(std::to_string(v));

        if (!directed)
            adjacency[v].push(std::to_string(u));
    }

    std::vector<bool> visited(static_cast<std::size_t>(n) + 1, false);
    std::vector<int> order;

    StackADT stack;
    std::size_t depth = 0;

    stack.push(std::to_string(start));
    ++depth;

    while (depth > 0)
    {
        const int vertex = std::stoi(stack.pop());
        --depth;

        if (visited[vertex])
            continue;

        visited[vertex] = true;
        order.push_back(vertex);

        const std::size_t degree = adjacency[vertex].size();

        for (std::size_t i = degree; i-- > 0;)
        {
            const int neighbour = std::stoi(adjacency[vertex].get(i));

            if (!visited[neighbour])
            {
                stack.push(std::to_string(neighbour));
                ++depth;
            }
        }
    }

    return order;
}

// ---------------------------------------------------------------------------
// Задание 3, вариант 1. Разбиение множества.
// ---------------------------------------------------------------------------

namespace
{
    // Перебор с возвратом: числа по убыванию раскладываются по «корзинам»
    // ёмкости target. Корзины с одинаковым остатком равноценны, поэтому на
    // каждом шаге пробуется только одна из них. Корзины — множества HashSet.
    class Partitioner
    {
    private:
        const std::vector<long long>& sorted;
        std::vector<HashSet> buckets;
        std::vector<long long> remaining;

        bool place(std::size_t i)
        {
            if (i == sorted.size())
                return true;

            const long long x = sorted[i];
            const std::string key = std::to_string(x);
            HashSet tried; // остатки, которые уже пробовали на этом шаге

            for (std::size_t b = 0; b < buckets.size(); ++b)
            {
                if (remaining[b] < x || !tried.add(std::to_string(remaining[b])))
                    continue;

                buckets[b].add(key);
                remaining[b] -= x;

                if (place(i + 1))
                    return true;

                remaining[b] += x;
                buckets[b].remove(key);
            }

            return false;
        }

    public:
        Partitioner(const std::vector<long long>& sortedNumbers,
                    long long sum, long long target)
            : sorted(sortedNumbers),
              buckets(static_cast<std::size_t>(sum / target)),
              remaining(static_cast<std::size_t>(sum / target), target)
        {
        }

        bool run(std::vector<std::vector<long long>>& parts)
        {
            if (!place(0))
                return false;

            parts.clear();

            for (const HashSet& bucket : buckets)
            {
                std::vector<long long> part;

                for (const std::string& s : bucket.values())
                    part.push_back(std::stoll(s));

                std::sort(part.begin(), part.end());
                parts.push_back(part);
            }

            std::sort(parts.begin(), parts.end(),
                      [](const std::vector<long long>& a,
                         const std::vector<long long>& b)
                      { return a.front() < b.front(); });
            return true;
        }
    };

    bool partitionInto(const std::vector<long long>& sorted, long long sum,
                       long long target,
                       std::vector<std::vector<long long>>& parts)
    {
        if (sorted.front() > target || sum % target != 0)
            return false;

        Partitioner partitioner(sorted, sum, target);
        return partitioner.run(parts);
    }
}

std::vector<std::vector<long long>> partitionEqualSum(
    const std::vector<long long>& numbers, long long target)
{
    if (numbers.empty())
        throw std::runtime_error("Set is empty");

    HashSet unique;
    long long sum = 0;
    long long largest = 0;

    for (long long x : numbers)
    {
        if (x <= 0)
            throw std::runtime_error("Elements must be natural numbers");

        if (!unique.add(std::to_string(x)))
            throw std::runtime_error("Duplicate element in set: " +
                                     std::to_string(x));

        sum += x;
        largest = std::max(largest, x);
    }

    if (target < 0)
        throw std::runtime_error("Target sum must be positive");

    std::vector<long long> sorted = numbers;
    std::sort(sorted.begin(), sorted.end(), std::greater<long long>());

    std::vector<std::vector<long long>> parts;

    if (target > 0)
    {
        if (partitionInto(sorted, sum, target, parts))
            return parts;

        throw std::runtime_error(
            "Set cannot be partitioned into subsets with equal sums");
    }

    // Без суммы — наименьшая возможная.
    for (long long t = largest; t <= sum; ++t)
    {
        if (sum % t == 0 && partitionInto(sorted, sum, t, parts))
            return parts;
    }

    throw std::runtime_error(
        "Set cannot be partitioned into subsets with equal sums");
}

std::vector<std::string> formatSubsets(
    const std::vector<std::vector<long long>>& parts)
{
    std::vector<std::string> lines;

    for (const auto& part : parts)
    {
        std::string line = "{";

        for (std::size_t i = 0; i < part.size(); ++i)
        {
            if (i != 0)
                line += ", ";

            line += std::to_string(part[i]);
        }

        lines.push_back(line + "}");
    }

    return lines;
}

// ---------------------------------------------------------------------------
// Задание 4, вариант 1. Различные подмассивы.
// ---------------------------------------------------------------------------

namespace
{
    // Перебор сочетаний индексов в лексикографическом порядке. Ключ для
    // проверки на повтор — элементы, склеенные через '\n' (в элементах,
    // прочитанных как слова, перевода строки быть не может).
    void generate(const std::vector<std::string>& elems, std::size_t start,
                  std::size_t remaining, std::vector<std::string>& current,
                  HashSet& seen, ArrayADT& result)
    {
        if (remaining == 0)
        {
            std::string key;
            std::string text = "{";

            for (std::size_t i = 0; i < current.size(); ++i)
            {
                key += current[i];
                key += '\n';

                if (i != 0)
                    text += ", ";

                text += current[i];
            }

            if (seen.add(key))
                result.push(text + "}");

            return;
        }

        for (std::size_t i = start; i + remaining <= elems.size(); ++i)
        {
            current.push_back(elems[i]);
            generate(elems, i + 1, remaining - 1, current, seen, result);
            current.pop_back();
        }
    }
}

std::vector<std::string> distinctSubarrays(const std::vector<std::string>& elems)
{
    if (elems.size() > MAX_SUBARRAY_ELEMS)
        throw std::runtime_error("Array is too long (more than 24 elements)");

    HashSet seen;
    ArrayADT result;
    std::vector<std::string> current;

    for (std::size_t size = 0; size <= elems.size(); ++size)
        generate(elems, 0, size, current, seen, result);

    return result.values();
}

std::string formatSubarrays(const std::vector<std::string>& subarrays)
{
    std::string text = "[";

    for (std::size_t i = 0; i < subarrays.size(); ++i)
    {
        if (i != 0)
            text += ", ";

        text += subarrays[i];
    }

    return text + "]";
}

// ---------------------------------------------------------------------------
// Задание 5, вариант 6. Листья бинарного дерева поиска.
// ---------------------------------------------------------------------------

// Узлы дерева лежат в векторе и ссылаются друг на друга по индексам
// (-1 — нет потомка). Вставка — O(h). Симметричный обход со стеком StackADT
// (в нём хранятся индексы узлов в виде строк) даёт листья по возрастанию
// за O(n) и не переполняет стек вызовов на вырожденном дереве.
std::vector<long long> bstLeaves(const std::vector<long long>& sequence)
{
    struct Node
    {
        long long value;
        int left = -1;
        int right = -1;
    };

    std::vector<Node> nodes;

    for (long long value : sequence)
    {
        if (nodes.empty())
        {
            nodes.push_back({value});
            continue;
        }

        int current = 0;

        while (true)
        {
            if (value == nodes[current].value)
                break; // повтор: ничего не добавляем

            int& child = value < nodes[current].value ? nodes[current].left
                                                      : nodes[current].right;

            if (child == -1)
            {
                const int created = static_cast<int>(nodes.size());
                child = created; // ссылка действительна: push_back ниже
                nodes.push_back({value});
                break;
            }

            current = child;
        }
    }

    std::vector<long long> leaves;
    StackADT stack;
    std::size_t depth = 0;
    int current = nodes.empty() ? -1 : 0;

    while (current != -1 || depth > 0)
    {
        while (current != -1)
        {
            stack.push(std::to_string(current));
            ++depth;
            current = nodes[current].left;
        }

        current = std::stoi(stack.pop());
        --depth;

        if (nodes[current].left == -1 && nodes[current].right == -1)
            leaves.push_back(nodes[current].value);

        current = nodes[current].right;
    }

    return leaves;
}

// ---------------------------------------------------------------------------
// Задание 6.2, вариант 6. Изоморфные строки.
// ---------------------------------------------------------------------------

namespace
{
    // Разбиение строки UTF-8 на символы (кодовые точки).
    std::vector<std::string> splitUtf8(const std::string& s)
    {
        std::vector<std::string> symbols;

        for (std::size_t i = 0; i < s.size();)
        {
            const unsigned char c = static_cast<unsigned char>(s[i]);
            std::size_t length = 1;

            if ((c >> 5) == 0x6)
                length = 2;
            else if ((c >> 4) == 0xE)
                length = 3;
            else if ((c >> 3) == 0x1E)
                length = 4;

            length = std::min(length, s.size() - i);
            symbols.push_back(s.substr(i, length));
            i += length;
        }

        return symbols;
    }
}

// Две хеш-таблицы (прямое и обратное соответствие), O(n) в среднем.
bool isomorphic(const std::string& a, const std::string& b, bool doubleHashing)
{
    const std::vector<std::string> sa = splitUtf8(a);
    const std::vector<std::string> sb = splitUtf8(b);

    if (sa.size() != sb.size())
        return false;

    OpenTable<std::string> forward(doubleHashing);
    OpenTable<std::string> backward(doubleHashing);

    for (std::size_t i = 0; i < sa.size(); ++i)
    {
        if (const std::string* mapped = forward.find(sa[i]))
        {
            if (*mapped != sb[i])
                return false;
        }
        else
        {
            forward.put(sa[i], sb[i]);
        }

        if (const std::string* mapped = backward.find(sb[i]))
        {
            if (*mapped != sa[i])
                return false;
        }
        else
        {
            backward.put(sb[i], sa[i]);
        }
    }

    return true;
}

// ---------------------------------------------------------------------------
// Задание 7, вариант 1. LRU-кэш.
// ---------------------------------------------------------------------------

namespace
{
    // Двусвязный список записей (давние в начале) и хеш-таблица
    // «ключ -> узел». DoublyList из ЛР1 здесь не подходит: он ищет узел по
    // значению за O(n), а для O(1) нужны прямые ссылки на узлы, поэтому узлы
    // списка здесь свои. Любое обращение (get, а также set существующего
    // ключа) переносит узел в конец списка, при переполнении удаляется первый.
    class LRUCache
    {
    private:
        struct Node
        {
            std::string key;
            std::string value;
            Node* prev = nullptr;
            Node* next = nullptr;
        };

        std::size_t capacity;
        std::size_t count = 0;
        Node* head = nullptr; // самая давняя запись
        Node* tail = nullptr; // самая свежая запись
        OpenTable<Node*> items{true};

        void unlink(Node* node)
        {
            if (node->prev)
                node->prev->next = node->next;
            else
                head = node->next;

            if (node->next)
                node->next->prev = node->prev;
            else
                tail = node->prev;

            node->prev = node->next = nullptr;
        }

        void append(Node* node)
        {
            node->prev = tail;

            if (tail)
                tail->next = node;
            else
                head = node;

            tail = node;
        }

    public:
        explicit LRUCache(long long cap)
            : capacity(cap > 0 ? static_cast<std::size_t>(cap) : 0)
        {
        }

        LRUCache(const LRUCache&) = delete;
        LRUCache& operator=(const LRUCache&) = delete;

        ~LRUCache()
        {
            while (head)
            {
                Node* next = head->next;
                delete head;
                head = next;
            }
        }

        bool get(const std::string& key, std::string& value)
        {
            Node** found = items.find(key);

            if (!found)
                return false;

            unlink(*found);
            append(*found);
            value = (*found)->value;
            return true;
        }

        void set(const std::string& key, const std::string& value)
        {
            if (capacity == 0)
                return;

            if (Node** found = items.find(key))
            {
                (*found)->value = value;
                unlink(*found);
                append(*found);
                return;
            }

            if (count >= capacity)
            {
                Node* oldest = head;
                unlink(oldest);
                items.erase(oldest->key);
                delete oldest;
                --count;
            }

            Node* node = new Node{key, value};
            append(node);
            items.put(key, node);
            ++count;
        }
    };
}

std::vector<std::string> runLRU(std::istream& in)
{
    long long capacity = 0;
    long long queries = 0;

    if (!(in >> capacity >> queries))
        throw std::runtime_error("Expected: <capacity> <Q> and then Q queries");

    LRUCache cache(capacity);
    std::vector<std::string> output;

    for (long long i = 0; i < queries; ++i)
    {
        std::string command;

        if (!(in >> command))
            throw std::runtime_error("Not enough queries");

        if (command == "SET")
        {
            std::string key, value;

            if (!(in >> key >> value))
                throw std::runtime_error("SET needs key and value");

            cache.set(key, value);
        }
        else if (command == "GET")
        {
            std::string key, value;

            if (!(in >> key))
                throw std::runtime_error("GET needs key");

            output.push_back(cache.get(key, value) ? value : "-1");
        }
        else
        {
            throw std::runtime_error("Unknown command: " + command);
        }
    }

    return output;
}

// ---------------------------------------------------------------------------
// Задание 6.1. Эмпирический анализ хеширования.
// ---------------------------------------------------------------------------

namespace
{
    std::vector<std::string> randomKeys(std::size_t n, const std::string& prefix,
                                        unsigned seed)
    {
        std::mt19937 rng(seed);
        const std::string alphabet = "abcdefghijklmnopqrstuvwxyz0123456789";
        std::uniform_int_distribution<std::size_t> pick(0, alphabet.size() - 1);

        std::vector<std::string> keys(n);

        for (std::string& key : keys)
        {
            key = prefix;

            for (int i = 0; i < 12; ++i)
                key += alphabet[pick(rng)];
        }

        return keys;
    }

    template <typename F>
    double nanosPerOp(std::size_t n, F&& action)
    {
        const auto begin = std::chrono::steady_clock::now();
        action();
        const auto end = std::chrono::steady_clock::now();
        return std::chrono::duration<double, std::nano>(end - begin).count() /
               static_cast<double>(n);
    }

    // Теоретическое среднее число проб (Кнут) при заполнении a.
    void theoryLinear(double a, double& hit, double& miss)
    {
        hit = 0.5 * (1 + 1 / (1 - a));
        miss = 0.5 * (1 + 1 / ((1 - a) * (1 - a)));
    }

    void theoryDouble(double a, double& hit, double& miss)
    {
        hit = std::log(1 / (1 - a)) / a;
        miss = 1 / (1 - a);
    }

    // Дополняет строку пробелами до width СИМВОЛОВ (а не байтов): printf считает
    // ширину в байтах, а русская буква в UTF-8 занимает 2 байта, из-за чего
    // колонки с русским текстом получались уже остальных.
    std::string fit(const std::string& text, std::size_t width, bool left)
    {
        std::size_t chars = 0;

        for (unsigned char c : text)
            if ((c & 0xC0) != 0x80)
                ++chars;

        const std::string padding(width > chars ? width - chars : 0, ' ');
        return left ? text + padding : padding + text;
    }

    void print(std::ostream& out, const char* format, ...)
        __attribute__((format(printf, 2, 3)));

    void print(std::ostream& out, const char* format, ...)
    {
        char buffer[512];
        va_list args;
        va_start(args, format);
        std::vsnprintf(buffer, sizeof buffer, format, args);
        va_end(args);
        out << buffer;
    }
}

// Часть 1: таблица фиксированного размера заполняется до заданной доли.
// Измеряется время (нс на операцию) и среднее число проб при успешном и
// неуспешном поиске, рядом — теоретическое значение.
// Часть 2: обычное использование — таблица растёт сама при заполнении 0.7.
void runHashBenchmark(std::ostream& out)
{
    const std::size_t capacity = 1 << 19;

    print(out, "Часть 1. Таблица на %zu ячеек, перестроение отключено\n", capacity);
    print(out, "%s %s %s %s %s %s %s %s\n", fit("метод", 10, true).c_str(),
          fit("альфа", 6, true).c_str(), fit("вставка", 8, false).c_str(),
          fit("поиск+", 8, false).c_str(), fit("поиск-", 8, false).c_str(),
          fit("проб при поиске+", 18, false).c_str(),
          fit("проб при поиске-", 18, false).c_str(),
          fit("коллизий", 10, false).c_str());
    print(out, "%s %s %s %s %s %s %s %s\n", fit("", 10, true).c_str(),
          fit("", 6, true).c_str(), fit("нс", 8, false).c_str(),
          fit("нс", 8, false).c_str(), fit("нс", 8, false).c_str(),
          fit("изм. (теор.)", 18, false).c_str(),
          fit("изм. (теор.)", 18, false).c_str(), fit("", 10, false).c_str());

    for (double load : {0.5, 0.7, 0.9})
    {
        const std::size_t n = static_cast<std::size_t>(load * capacity);
        const auto keys = randomKeys(n, "k", 1);
        const auto missing = randomKeys(n, "m", 2); // другой префикс: таких ключей нет

        for (bool dbl : {false, true})
        {
            OpenTable<int> table(dbl, capacity, 0.99);

            const double insert = nanosPerOp(n, [&]
            {
                for (std::size_t i = 0; i < n; ++i)
                    table.put(keys[i], static_cast<int>(i));
            });

            std::size_t before = table.probes;
            const double hit = nanosPerOp(n, [&]
            {
                for (const auto& key : keys)
                    if (!table.find(key))
                        throw std::runtime_error("key lost");
            });
            const double hitProbes = static_cast<double>(table.probes - before) / n;

            before = table.probes;
            const double miss = nanosPerOp(n, [&]
            {
                for (const auto& key : missing)
                    if (table.find(key))
                        throw std::runtime_error("phantom key");
            });
            const double missProbes = static_cast<double>(table.probes - before) / n;

            double theoryHit = 0, theoryMiss = 0;

            if (dbl)
                theoryDouble(load, theoryHit, theoryMiss);
            else
                theoryLinear(load, theoryHit, theoryMiss);

            print(out, "%s %-6.2f %8.0f %8.0f %8.0f %9.2f (%5.2f) %9.2f (%5.2f) %10zu\n",
                  fit(dbl ? "двойное" : "линейное", 10, true).c_str(), load, insert, hit, miss, hitProbes,
                  theoryHit, missProbes, theoryMiss, table.collisions);
        }
    }

    print(out, "\nЧасть 2. Обычное использование: таблица растёт сама (предел заполнения 0.7)\n");
    print(out, "%s %s %s %s %s %s %s\n", fit("метод", 10, true).c_str(),
          fit("N", 8, true).c_str(), fit("вставка", 8, false).c_str(),
          fit("поиск+", 8, false).c_str(), fit("коллизий", 10, false).c_str(),
          fit("перестроек", 10, false).c_str(),
          fit("проб/поиск+", 12, false).c_str());

    for (std::size_t n : {10000u, 100000u, 500000u})
    {
        const auto keys = randomKeys(n, "k", 1);

        for (bool dbl : {false, true})
        {
            OpenTable<int> table(dbl, 16, 0.7);

            const double insert = nanosPerOp(n, [&]
            {
                for (std::size_t i = 0; i < n; ++i)
                    table.put(keys[i], static_cast<int>(i));
            });

            const std::size_t before = table.probes;
            const double hit = nanosPerOp(n, [&]
            {
                for (const auto& key : keys)
                    if (!table.find(key))
                        throw std::runtime_error("key lost");
            });

            print(out, "%s %-8zu %8.0f %8.0f %10zu %10zu %12.2f\n",
                  fit(dbl ? "двойное" : "линейное", 10, true).c_str(), n, insert, hit, table.collisions,
                  table.resizes, static_cast<double>(table.probes - before) / n);
        }
    }
}
