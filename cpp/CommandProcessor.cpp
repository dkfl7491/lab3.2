#include "CommandProcessor.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace 
{
    // Проверка, соответствует ли количество аргументов команде.
    void requireCount(const std::vector<std::string>& args,
                      std::size_t expected)
    {
        if (args.size() != expected)
            throw std::runtime_error("Incorrect number of arguments");
    }

    // Преобразование строки в индекс, проверка на корректность значения.
    std::size_t toIndex(const std::string& value)
    {
        std::size_t position = 0;
        unsigned long long result = std::stoull(value, &position);

        if (position != value.size())
            throw std::runtime_error("Invalid index");

        return static_cast<std::size_t>(result);
    }

    // Проверка, что имя структуры не пустое.
    std::string requireName(const std::string& name)
    {
        if (name.empty())
            throw std::runtime_error("Structure name is empty");

        return name;
    }
}

// Сохранение ссылки на базу данных и регистрирование доступных команд.
CommandProcessor::CommandProcessor(Database& database,
                                   const std::string& filename)
    : db(database), filename(filename)
{
    registerHandlers();
}

// Вывод элементов структуры через пробел.
void CommandProcessor::printValues(
    const std::vector<std::string>& values) const
{
    for (std::size_t i = 0; i < values.size(); ++i)
    {
        if (i != 0)
            std::cout << ' ';

        std::cout << values[i];
    }

    std::cout << '\n';
}

// Регистрирование команд для работы со всеми структурами данных.
void CommandProcessor::registerHandlers()
{
    // Команды работы с массивом.
    handlers["MPUSH"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.arrays[requireName(a[0])].push(a[1]);
    };

    handlers["MINSERT"] = [this](const Arguments& a)
    {
        requireCount(a, 3);
        db.arrays[requireName(a[0])].insert(toIndex(a[1]), a[2]);
    };

    handlers["MGET"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        std::cout << db.arrays.at(a[0]).get(toIndex(a[1])) << '\n';
    };

    handlers["MDEL"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.arrays.at(a[0]).remove(toIndex(a[1]));
    };

    handlers["MSET"] = [this](const Arguments& a)
    {
        requireCount(a, 3);
        db.arrays.at(a[0]).set(toIndex(a[1]), a[2]);
    };

    handlers["MLEN"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        std::cout << db.arrays.at(a[0]).size() << '\n';
    };

    handlers["MREAD"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        printValues(db.arrays.at(a[0]).values());
    };

    // Команды работы с односвязным списком.
    handlers["FPUSH"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.singlyLists[requireName(a[0])].pushBack(a[1]);
    };

    handlers["FHEAD"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.singlyLists[requireName(a[0])].pushFront(a[1]);
    };

    handlers["FBEFORE"] = [this](const Arguments& a)
    {
        requireCount(a, 3);
        db.singlyLists.at(a[0]).insertBefore(a[1], a[2]);
    };

    handlers["FAFTER"] = [this](const Arguments& a)
    {
        requireCount(a, 3);
        db.singlyLists.at(a[0]).insertAfter(a[1], a[2]);
    };

    handlers["FDEL"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.singlyLists.at(a[0]).removeValue(a[1]);
    };

    handlers["FDELHEAD"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        db.singlyLists.at(a[0]).removeFront();
    };

    handlers["FDELTAIL"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        db.singlyLists.at(a[0]).removeBack();
    };
        handlers["FDELBEFORE"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.singlyLists.at(a[0]).removeBefore(a[1]);
    };

    handlers["FDELAFTER"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.singlyLists.at(a[0]).removeAfter(a[1]);
    };

    handlers["FGET"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        std::cout << db.singlyLists.at(a[0]).get(toIndex(a[1])) << '\n';
    };

    handlers["FSEARCH"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        std::cout << (db.singlyLists.at(a[0]).contains(a[1])
                          ? "TRUE" : "FALSE") << '\n';
    };

    handlers["FREAD"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        printValues(db.singlyLists.at(a[0]).values());
    };

    // Команды работы с двусвязным списком.
    handlers["LPUSH"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.doublyLists[requireName(a[0])].pushBack(a[1]);
    };

    handlers["LHEAD"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.doublyLists[requireName(a[0])].pushFront(a[1]);
    };

    handlers["LBEFORE"] = [this](const Arguments& a)
    {
        requireCount(a, 3);
        db.doublyLists.at(a[0]).insertBefore(a[1], a[2]);
    };

    handlers["LAFTER"] = [this](const Arguments& a)
    {
        requireCount(a, 3);
        db.doublyLists.at(a[0]).insertAfter(a[1], a[2]);
    };

    handlers["LDEL"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.doublyLists.at(a[0]).removeValue(a[1]);
    };

    handlers["LDELHEAD"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        db.doublyLists.at(a[0]).removeFront();
    };

    handlers["LDELTAIL"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        db.doublyLists.at(a[0]).removeBack();
    };
        handlers["LDELBEFORE"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.doublyLists.at(a[0]).removeBefore(a[1]);
    };

    handlers["LDELAFTER"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.doublyLists.at(a[0]).removeAfter(a[1]);
    };

    handlers["LGET"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        std::cout << db.doublyLists.at(a[0]).get(toIndex(a[1])) << '\n';
    };

    handlers["LSEARCH"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        std::cout << (db.doublyLists.at(a[0]).contains(a[1])
                          ? "TRUE" : "FALSE") << '\n';
    };

    handlers["LREAD"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        printValues(db.doublyLists.at(a[0]).values());
    };

    // Команды работы со стеком.
    handlers["SPUSH"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.stacks[requireName(a[0])].push(a[1]);
    };

    handlers["SPOP"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        std::cout << db.stacks.at(a[0]).pop() << '\n';
    };

    handlers["SGET"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        std::cout << db.stacks.at(a[0]).get() << '\n';
    };

    handlers["SREAD"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        printValues(db.stacks.at(a[0]).values());
    };

    // Команды работы с очередью.
    handlers["QPUSH"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.queues[requireName(a[0])].push(a[1]);
    };

    handlers["QPOP"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        std::cout << db.queues.at(a[0]).pop() << '\n';
    };

    handlers["QGET"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        std::cout << db.queues.at(a[0]).get() << '\n';
    };

    handlers["QREAD"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        printValues(db.queues.at(a[0]).values());
    };

    // Команды работы с полным бинарным деревом.
    handlers["TINSERT"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.trees[requireName(a[0])].insert(a[1]);
    };

    handlers["TGET"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        std::cout << (db.trees.at(a[0]).contains(a[1])
                          ? "TRUE" : "FALSE") << '\n';
    };

    // ISMEMBER выполняет ту же операцию, что и TGET.
    handlers["ISMEMBER"] = handlers["TGET"];

    handlers["TFULL"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        std::cout << (db.trees.at(a[0]).isFull()
                          ? "TRUE" : "FALSE") << '\n';
    };

    handlers["TREAD"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        printValues(db.trees.at(a[0]).values());
    };

    // Команды работы с множеством (задание 2).
    // Множество создаётся при первом SETADD, как и остальные структуры.
    auto findSet = [this](const std::string& name) -> HashSet&
    {
        auto it = db.sets.find(name);

        if (it == db.sets.end())
            throw std::runtime_error("Structure not found");

        return it->second;
    };

    handlers["SETADD"] = [this](const Arguments& a)
    {
        requireCount(a, 2);
        db.sets[requireName(a[0])].add(a[1]);
    };

    handlers["SETDEL"] = [findSet](const Arguments& a)
    {
        requireCount(a, 2);

        if (!findSet(a[0]).remove(a[1]))
            throw std::runtime_error("Value not found");
    };

    handlers["SET_AT"] = [findSet](const Arguments& a)
    {
        requireCount(a, 2);
        std::cout << (findSet(a[0]).has(a[1]) ? "TRUE" : "FALSE") << '\n';
    };

    handlers["SETREAD"] = [this, findSet](const Arguments& a)
    {
        requireCount(a, 1);
        printValues(findSet(a[0]).values());
    };

    // PRINT выводит элементы структуры по её имени.
    handlers["PRINT"] = [this](const Arguments& a)
    {
        requireCount(a, 1);
        const std::string& name = a[0];

        if (db.arrays.count(name))
            printValues(db.arrays.at(name).values());
        else if (db.singlyLists.count(name))
            printValues(db.singlyLists.at(name).values());
        else if (db.doublyLists.count(name))
            printValues(db.doublyLists.at(name).values());
        else if (db.stacks.count(name))
            printValues(db.stacks.at(name).values());
        else if (db.queues.count(name))
            printValues(db.queues.at(name).values());
        else if (db.trees.count(name))
            printValues(db.trees.at(name).values());
        else if (db.sets.count(name))
            printValues(db.sets.at(name).values());
        else
            throw std::runtime_error("Structure not found");
    };
}

// Разборка команды, нахождения обработчика, выполнение операции и сохранение данных.
void CommandProcessor::execute(const std::string& query)
{
    std::istringstream input(query);
    std::string command;
    input >> command;

    if (command.empty())
        throw std::runtime_error("Empty command");

    // Разделение строки команды на отдельные аргументы.
    Arguments args;
    std::string argument;

    while (input >> argument)
        args.push_back(argument);

    // Нахождение команды в таблице обработчиков и её выпрление.
    auto handler = handlers.find(command);

    if (handler == handlers.end())
        throw std::runtime_error("Unknown command: " + command);

    handler->second(args);

    // Сохранение изменений в файл после успешного выполнения команды.
    db.save(filename);
}

