#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include "Database.h"

#include <functional>
#include <map>
#include <string>
#include <vector>

class CommandProcessor
{
private:
    Database& db;
    std::string filename;

    using Arguments = std::vector<std::string>;
    using Handler = std::function<void(const Arguments&)>;

    std::map<std::string, Handler> handlers;

    void registerHandlers();
    void printValues(const std::vector<std::string>& values) const;

public:
    CommandProcessor(Database& database, const std::string& filename);
    void execute(const std::string& query);
};

#endif
