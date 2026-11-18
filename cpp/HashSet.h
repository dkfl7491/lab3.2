#ifndef HASH_SET_H
#define HASH_SET_H

#include "Hash.h"
#include "Structures.h"

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

// Задание 2: АТД «Множество».
// Хеш-таблица с цепочками: каждая корзина — односвязный список SinglyList
// из лабораторной работы №1. При заполнении больше 0.75 число корзин
// удваивается, поэтому добавление, удаление и проверка наличия выполняются
// за O(1) в среднем.
class HashSet
{
private:
    std::vector<SinglyList> buckets;
    std::size_t count = 0;

    std::size_t indexOf(const std::string& value) const
    {
        return static_cast<std::size_t>(hashFNV(value) % buckets.size());
    }

    // Удваивает число корзин и заново распределяет элементы.
    void grow()
    {
        std::vector<SinglyList> bigger(buckets.size() * 2);
        std::swap(buckets, bigger);

        for (const SinglyList& bucket : bigger)
            for (const std::string& value : bucket.values())
                buckets[indexOf(value)].pushBack(value);
    }

public:
    HashSet() : buckets(16) {}

    // Добавляет элемент; возвращает true, если его не было.
    bool add(const std::string& value)
    {
        SinglyList& bucket = buckets[indexOf(value)];

        if (bucket.contains(value))
            return false;

        bucket.pushBack(value);
        ++count;

        if (count * 4 > buckets.size() * 3)
            grow();

        return true;
    }

    // Удаляет элемент; возвращает true, если он был во множестве.
    bool remove(const std::string& value)
    {
        SinglyList& bucket = buckets[indexOf(value)];

        if (!bucket.contains(value))
            return false;

        bucket.removeValue(value);
        --count;
        return true;
    }

    bool has(const std::string& value) const
    {
        return buckets[indexOf(value)].contains(value);
    }

    std::size_t size() const
    {
        return count;
    }

    // Элементы в лексикографическом порядке.
    std::vector<std::string> values() const
    {
        std::vector<std::string> result;
        result.reserve(count);

        for (const SinglyList& bucket : buckets)
            for (const std::string& value : bucket.values())
                result.push_back(value);

        std::sort(result.begin(), result.end());
        return result;
    }
};

#endif
