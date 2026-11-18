// Модульные тесты C++ части: ./tests (или make test).
#include "HashSet.h"
#include "OpenTable.h"
#include "Tasks.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>

static int failures = 0;

#define CHECK(cond)                                                          \
    do                                                                       \
    {                                                                        \
        if (!(cond))                                                         \
        {                                                                    \
            ++failures;                                                      \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << ": "      \
                      << #cond << '\n';                                      \
        }                                                                    \
    } while (false)

template <typename F>
static bool throws(F&& f)
{
    try
    {
        f();
    }
    catch (const std::exception&)
    {
        return true;
    }

    return false;
}

static std::string lru(const std::string& input)
{
    std::istringstream in(input);
    std::string result;

    for (const std::string& s : runLRU(in))
        result += (result.empty() ? "" : " ") + s;

    return result;
}

static std::vector<long long> sums(const std::vector<std::vector<long long>>& parts)
{
    std::vector<long long> out;

    for (const auto& part : parts)
    {
        long long s = 0;

        for (long long v : part)
            s += v;

        out.push_back(s);
    }

    return out;
}

// Таблицы сравниваются со std::map на случайных операциях.
static void testOpenTable(bool dbl)
{
    OpenTable<int> table(dbl, 8);
    std::map<std::string, int> ref;
    std::mt19937 rng(42);

    for (int step = 0; step < 200000; ++step)
    {
        const std::string key = "key" + std::to_string(rng() % 5000);

        switch (rng() % 3)
        {
        case 0:
        {
            const int value = static_cast<int>(rng());
            table.put(key, value);
            ref[key] = value;
            break;
        }
        case 1:
            CHECK(table.erase(key) == (ref.erase(key) == 1));
            break;
        default:
        {
            const int* found = table.find(key);
            auto it = ref.find(key);
            CHECK((found != nullptr) == (it != ref.end()));

            if (found && it != ref.end())
                CHECK(*found == it->second);
        }
        }

        if (table.size() != ref.size())
        {
            CHECK(table.size() == ref.size());
            return;
        }
    }
}

static void testTombstones(bool dbl)
{
    OpenTable<int> table(dbl, 8, 0.9);

    for (int i = 0; i < 6; ++i)
        table.put("k" + std::to_string(i), i);

    for (int i = 0; i < 6; i += 2)
        table.erase("k" + std::to_string(i));

    for (int i = 1; i < 6; i += 2)
    {
        const int* found = table.find("k" + std::to_string(i));
        CHECK(found && *found == i);
    }

    // Постоянные вставки и удаления только чистят «надгробия».
    for (int round = 0; round < 1000; ++round)
    {
        table.put("t" + std::to_string(round), round);
        table.erase("t" + std::to_string(round));
    }

    CHECK(table.size() == 3);
    CHECK(table.capacity() <= 16);
}

int main()
{
    // HashSet (задание 2)
    {
        HashSet set;
        CHECK(set.add("a") && !set.add("a"));
        set.add("b");
        CHECK(set.has("a") && !set.has("c"));
        CHECK(set.remove("a") && !set.remove("a"));
        CHECK(set.values() == std::vector<std::string>{"b"});

        for (int i = 0; i < 10000; ++i)
            set.add(std::to_string(i));

        CHECK(set.size() == 10001);

        for (int i = 0; i < 10000; ++i)
            CHECK(set.has(std::to_string(i)));
    }

    testOpenTable(false);
    testOpenTable(true);
    testTombstones(false);
    testTombstones(true);

    // Задание 1: DFS
    CHECK((dfs(6, {{1, 2}, {1, 3}, {2, 4}, {2, 5}, {3, 6}}, 1, false) ==
           std::vector<int>{1, 2, 4, 5, 3, 6}));
    CHECK((dfs(4, {{1, 2}, {2, 3}, {3, 1}, {1, 4}}, 1, true) ==
           std::vector<int>{1, 2, 3, 4}));
    CHECK((dfs(5, {{1, 2}, {4, 5}}, 1, false) == std::vector<int>{1, 2}));
    CHECK(throws([] { dfs(3, {}, 4, false); }));
    CHECK(throws([] { dfs(3, {{1, 9}}, 1, false); }));

    // Задание 3: разбиение множества
    {
        const std::vector<long long> s{4, 10, 5, 1, 3, 7};
        CHECK((sums(partitionEqualSum(s, 15)) == std::vector<long long>{15, 15}));
        CHECK((sums(partitionEqualSum(s, 10)) == std::vector<long long>{10, 10, 10}));
        CHECK(partitionEqualSum(s, 0).size() == 3);
        CHECK(partitionEqualSum(s, 30).size() == 1);

        std::vector<long long> all;

        for (const auto& part : partitionEqualSum(s, 10))
            all.insert(all.end(), part.begin(), part.end());

        std::sort(all.begin(), all.end());
        CHECK((all == std::vector<long long>{1, 3, 4, 5, 7, 10}));

        for (long long target : {5, 7, 8, 11, 12})
            CHECK(throws([&] { partitionEqualSum(s, target); }));

        CHECK(partitionEqualSum({5, 4, 3, 2, 1, 6, 7, 8}, 12).size() == 3);
        CHECK(throws([] { partitionEqualSum({1, 2, 2}, 3); }));
        CHECK(throws([] { partitionEqualSum({1, 0}, 1); }));
        CHECK(throws([] { partitionEqualSum({}, 1); }));
        CHECK((formatSubsets({{1, 4, 10}, {3}}) ==
               std::vector<std::string>{"{1, 4, 10}", "{3}"}));
    }

    // Задание 4: подмассивы
    CHECK(formatSubarrays(distinctSubarrays({"x", "y", "z"})) ==
          "[{}, {x}, {y}, {z}, {x, y}, {x, z}, {y, z}, {x, y, z}]");
    CHECK(formatSubarrays(distinctSubarrays({"1", "1"})) == "[{}, {1}, {1, 1}]");
    CHECK(formatSubarrays(distinctSubarrays({})) == "[{}]");
    CHECK(distinctSubarrays({"b", "a", "b"}).size() == 7);
    CHECK(throws([] { distinctSubarrays(std::vector<std::string>(MAX_SUBARRAY_ELEMS + 1, "a")); }));

    // Задание 5: листья BST
    CHECK((bstLeaves({7, 3, 2, 1, 9, 5, 4, 6, 8}) == std::vector<long long>{1, 4, 6, 8}));
    CHECK((bstLeaves({5}) == std::vector<long long>{5}));
    CHECK((bstLeaves({5, 5, 5}) == std::vector<long long>{5}));
    CHECK(bstLeaves({}).empty());
    {
        std::vector<long long> degenerate(5000);

        for (std::size_t i = 0; i < degenerate.size(); ++i)
            degenerate[i] = static_cast<long long>(i) + 1;

        CHECK((bstLeaves(degenerate) == std::vector<long long>{5000}));
    }

    // Задание 6.2: изоморфные строки (обе таблицы)
    for (bool dbl : {false, true})
    {
        CHECK(isomorphic("fall", "redd", dbl));
        CHECK(!isomorphic("mad", "odd", dbl));
        CHECK(isomorphic("dad", "ada", dbl));
        CHECK(!isomorphic("ab", "aa", dbl));
        CHECK(!isomorphic("aa", "ab", dbl));
        CHECK(!isomorphic("abc", "ab", dbl));
        CHECK(isomorphic("", "", dbl));
        CHECK(isomorphic("привет", "абвгде", dbl));
    }

    // Задание 7: LRU
    CHECK(lru("2 2 SET 1 2 GET 1") == "2");
    CHECK(lru("2 8 SET 1 2 SET 2 3 SET 1 5 SET 4 5 SET 6 7 GET 4 SET 1 2 GET 3") == "5 -1");
    CHECK(lru("2 6\nSET 1 1\nSET 2 2\nGET 1\nSET 3 3\nGET 1\nGET 2") == "1 1 -1");
    CHECK(lru("0 2 SET 1 1 GET 1") == "-1");
    CHECK(throws([] { lru("2 1 PUT 1 1"); }));

    if (failures == 0)
        std::cout << "All tests passed\n";

    return failures == 0 ? 0 : 1;
}
