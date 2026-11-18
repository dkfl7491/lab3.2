#ifndef HASH_H
#define HASH_H

#include <cstdint>
#include <string>

// Финальное перемешивание битов (из MurmurHash3): остаток от деления на
// размер таблицы должен зависеть от всех битов хеша.
inline std::uint64_t mixBits(std::uint64_t h)
{
    h ^= h >> 33;
    h *= 0xff51afd7ed558ccdULL;
    h ^= h >> 33;
    h *= 0xc4ceb9fe1a85ec53ULL;
    h ^= h >> 33;
    return h;
}

// Первая хеш-функция: FNV-1a.
inline std::uint64_t hashFNV(const std::string& s, std::uint64_t seed = 0)
{
    std::uint64_t h = 14695981039346656037ULL ^ seed;

    for (unsigned char c : s)
    {
        h ^= c;
        h *= 1099511628211ULL;
    }

    return mixBits(h);
}

// Вторая хеш-функция (для двойного хеширования): полиномиальный хеш.
inline std::uint64_t hashPoly(const std::string& s, std::uint64_t seed = 0)
{
    std::uint64_t h = seed;

    for (unsigned char c : s)
        h = h * 1000003ULL + c + 1;

    return mixBits(h);
}

#endif
