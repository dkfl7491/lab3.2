// Задание 1, вариант 6: обход деревьев и графов в глубину со стеком.
//
// Ввод (stdin): "n m start", затем m рёбер "u v" (вершины 1..n).
// Ключ --directed: рёбра ориентированные (по умолчанию неориентированные).
// Вывод: вершины в порядке обхода.
#include "Tasks.h"

#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    bool directed = false;

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--directed")
        {
            directed = true;
        }
        else
        {
            std::cerr << "Usage: ./task1 [--directed] < input\n";
            return 1;
        }
    }

    int n = 0, m = 0, start = 0;

    if (!(std::cin >> n >> m >> start) || n < 1 || m < 0)
    {
        std::cerr << "ERROR: expected: n m start, then m edges 'u v'\n";
        return 1;
    }

    std::vector<std::pair<int, int>> edges(static_cast<std::size_t>(m));

    for (auto& edge : edges)
    {
        if (!(std::cin >> edge.first >> edge.second))
        {
            std::cerr << "ERROR: not enough edges\n";
            return 1;
        }
    }

    try
    {
        const std::vector<int> order = dfs(n, edges, start, directed);

        for (std::size_t i = 0; i < order.size(); ++i)
            std::cout << (i ? " " : "") << order[i];

        std::cout << '\n';
    }
    catch (const std::exception& error)
    {
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
