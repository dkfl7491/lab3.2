#include "Database.h"

#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>

// Загрузка структур данных из файла.
void Database::load(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file)
        return;

    char type;
    std::string name;
    std::size_t count;

    // Считывание типов структуры  (имя и количество элементов).
    while (file >> type >> std::quoted(name) >> count)
    {
        // Пустое множество тоже должно существовать после загрузки.
        if (type == 'U')
            sets[name];

        for (std::size_t i = 0; i < count; ++i)
        {
            std::string value;

            if (!(file >> std::quoted(value)))
                throw std::runtime_error("Invalid database file");

            // Добавление элементы в соответствующую структуру.
            switch (type)
            {
                case 'M':
                    arrays[name].push(value);
                    break;

                case 'F':
                    singlyLists[name].pushBack(value);
                    break;

                case 'L':
                    doublyLists[name].pushBack(value);
                    break;

                case 'S':
                    stacks[name].push(value);
                    break;

                case 'Q':
                    queues[name].push(value);
                    break;

                case 'T':
                    trees[name].insert(value);
                    break;

                case 'U':
                    sets[name].add(value);
                    break;

                default:
                    throw std::runtime_error("Unknown structure in database");
            }
        }
    }
}

// Сохранение всех структур данных в файл.
void Database::save(const std::string& filename) const
{
    std::ofstream file(filename);

    if (!file)
        throw std::runtime_error("Cannot write database file");

    // Запись типов, имен и элементов одной структуры.
    auto writeRecord = [&file](char type,
                               const std::string& name,
                               const std::vector<std::string>& values)
    {
        file << type << ' ' << std::quoted(name)
             << ' ' << values.size();

        for (const std::string& value : values)
            file << ' ' << std::quoted(value);

        file << '\n';
    };

    // Сохранение структур каждого типа.
    for (const auto& [name, structure] : arrays)
        writeRecord('M', name, structure.values());

    for (const auto& [name, structure] : singlyLists)
        writeRecord('F', name, structure.values());

    for (const auto& [name, structure] : doublyLists)
        writeRecord('L', name, structure.values());

    for (const auto& [name, structure] : stacks)
        writeRecord('S', name, structure.values());

    for (const auto& [name, structure] : queues)
        writeRecord('Q', name, structure.values());

    for (const auto& [name, structure] : trees)
        writeRecord('T', name, structure.insertionValues());

    for (const auto& [name, structure] : sets)
        writeRecord('U', name, structure.values());
}
