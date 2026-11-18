#include "CommandProcessor.h"

#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char* argv[])
{
    // Значения по умолчанию: имя файла и команда пользователя.
    std::string filename = "file.data";
    std::string query;

    // Обработка аргументов, переданных при запуске программы.
    for (int i = 1; i < argc; ++i)
    {
        std::string argument = argv[i];

        if (argument == "--file" && i + 1 < argc)
        {
            filename = argv[++i]; // Указываем файл для хранения данных.
        }
        else if (argument == "--query" && i + 1 < argc)
        {
            query = argv[++i]; // Получаем команду для выполнения.
        }
        else
        {
            // Вывод подсказки, если аргументы заданы неправильно.
            std::cerr << "Usage: ./task2 --file file.data "
                         "--query 'COMMAND arguments'\n";
            return 1;
        }
    }

    // Проверка, была ли передана команда.
    if (query.empty())
    {
        std::cerr << "Query was not specified\n";
        return 1;
    }

    try
    {
        // Загрузка сохранённых данных и выполнение команд.
        Database database;
        database.load(filename);

        CommandProcessor processor(database, filename);
        processor.execute(query);
    }
    catch (const std::exception& error)
    {
        
        std::cerr << "ERROR: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
