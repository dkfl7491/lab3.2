#ifndef OPEN_TABLE_H
#define OPEN_TABLE_H

#include "Hash.h"

#include <cstddef>
#include <string>
#include <vector>

// Задание 6.1, вариант 1: хеш-таблица с открытой адресацией.
// Все пары лежат прямо в массиве ячеек; при коллизии ищется другая ячейка
// по последовательности проб idx, idx + step, idx + 2*step, ... (по модулю
// размера):
//   - линейное апробирование: step = 1;
//   - двойное хеширование: step = h2(key), вторая хеш-функция (нечётная).
// Размер таблицы — степень двойки, поэтому нечётный шаг взаимно прост с ним
// и последовательность проб обходит все ячейки. Удаление помечает ячейку
// «надгробием». Если заполнение (с учётом надгробий) превышает maxLoad,
// таблица перестраивается: удваивается или просто очищается от надгробий.
template <typename V>
class OpenTable
{
private:
    enum class State : unsigned char { Empty, Used, Deleted };

    struct Slot
    {
        State state = State::Empty;
        std::string key;
        V value{};
    };

    std::vector<Slot> slots;
    std::size_t mask;
    std::size_t live = 0;
    std::size_t tombs = 0;
    bool doubleHashing;
    double maxLoad;

    std::size_t start(const std::string& key) const
    {
        return static_cast<std::size_t>(hashFNV(key) & mask);
    }

    std::size_t step(const std::string& key) const
    {
        if (!doubleHashing)
            return 1;

        return static_cast<std::size_t>(hashPoly(key) & mask) | 1;
    }

    // Индекс ячейки с ключом или slots.size(), если ключа нет.
    std::size_t locate(const std::string& key, bool countProbes)
    {
        std::size_t index = start(key);
        const std::size_t delta = step(key);

        for (std::size_t i = 0; i <= mask; ++i)
        {
            if (countProbes)
                ++probes;

            const Slot& slot = slots[index];

            if (slot.state == State::Empty)
                break;

            if (slot.state == State::Used && slot.key == key)
                return index;

            index = (index + delta) & mask;
        }

        return slots.size();
    }

    // Новый массив ячеек и повторная вставка всех пар.
    void rebuild(std::size_t size)
    {
        std::vector<Slot> old(size);
        old.swap(slots);
        mask = size - 1;
        live = 0;
        tombs = 0;
        ++resizes;

        for (Slot& slot : old)
        {
            if (slot.state != State::Used)
                continue;

            std::size_t index = start(slot.key);
            const std::size_t delta = step(slot.key);

            while (slots[index].state != State::Empty)
                index = (index + delta) & mask;

            slots[index] = std::move(slot);
            ++live;
        }
    }

    void rebuildIfNeeded()
    {
        const double size = static_cast<double>(slots.size());

        if (static_cast<double>(live + tombs) <= maxLoad * size)
            return;

        if (static_cast<double>(live) > maxLoad / 2 * size)
            rebuild(slots.size() * 2);
        else
            rebuild(slots.size()); // много надгробий — достаточно их вычистить
    }

public:
    std::size_t collisions = 0; // новые ключи, попавшие не в свою первую ячейку
    std::size_t resizes = 0;    // перестроения таблицы
    std::size_t probes = 0;     // просмотренные ячейки во всех вызовах find

    // capacity округляется вверх до степени двойки.
    explicit OpenTable(bool doubleHashing, std::size_t capacity = 16,
                       double maxLoad = 0.7)
        : doubleHashing(doubleHashing), maxLoad(maxLoad)
    {
        std::size_t size = 8;

        while (size < capacity)
            size <<= 1;

        slots.resize(size);
        mask = size - 1;
    }

    // Добавляет пару или обновляет значение существующего ключа.
    void put(const std::string& key, const V& value)
    {
        const std::size_t first = start(key);
        const std::size_t delta = step(key);
        std::size_t index = first;
        std::size_t deleted = slots.size(); // первое «надгробие» на пути проб

        for (std::size_t i = 0; i <= mask; ++i)
        {
            Slot& slot = slots[index];

            if (slot.state == State::Used)
            {
                if (slot.key == key)
                {
                    slot.value = value;
                    return;
                }
            }
            else if (slot.state == State::Deleted)
            {
                if (deleted == slots.size())
                    deleted = index;
            }
            else
            {
                // Ключа в таблице нет: вставляем, предпочтительно на «надгробие».
                std::size_t target = index;

                if (deleted != slots.size())
                {
                    target = deleted;
                    --tombs;
                }

                slots[target].state = State::Used;
                slots[target].key = key;
                slots[target].value = value;
                ++live;

                if (target != first)
                    ++collisions;

                rebuildIfNeeded();
                return;
            }

            index = (index + delta) & mask;
        }

        // Пустых ячеек не осталось (возможно только при maxLoad около 1).
        if (deleted != slots.size())
        {
            slots[deleted].state = State::Used;
            slots[deleted].key = key;
            slots[deleted].value = value;
            --tombs;
            ++live;
            return;
        }

        rebuild(slots.size() * 2);
        put(key, value);
    }

    // Возвращает указатель на значение или nullptr, если ключа нет.
    V* find(const std::string& key)
    {
        const std::size_t index = locate(key, true);
        return index == slots.size() ? nullptr : &slots[index].value;
    }

    bool contains(const std::string& key)
    {
        return find(key) != nullptr;
    }

    // Удаляет ключ; возвращает true, если он был в таблице.
    bool erase(const std::string& key)
    {
        const std::size_t index = locate(key, false);

        if (index == slots.size())
            return false;

        slots[index] = Slot();
        slots[index].state = State::Deleted;
        --live;
        ++tombs;
        return true;
    }

    std::size_t size() const
    {
        return live;
    }

    std::size_t capacity() const
    {
        return slots.size();
    }

    double loadFactor() const
    {
        return static_cast<double>(live) / static_cast<double>(slots.size());
    }
};

#endif
