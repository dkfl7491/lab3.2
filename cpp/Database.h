#ifndef DATABASE_H
#define DATABASE_H

#include "HashSet.h"
#include "Structures.h"

#include <map>
#include <string>

class Database
{
public:
    std::map<std::string, ArrayADT> arrays;
    std::map<std::string, SinglyList> singlyLists;
    std::map<std::string, DoublyList> doublyLists;
    std::map<std::string, StackADT> stacks;
    std::map<std::string, QueueADT> queues;
    std::map<std::string, FullBinaryTree> trees;
    std::map<std::string, HashSet> sets;

    void load(const std::string& filename);
    void save(const std::string& filename) const;
};

#endif
