#ifndef ARRAY_H
#define ARRAY_H

#include <cstddef>
#include <string>

class ArrayADT
{
private:
    std::string* data;
    std::size_t count;
    std::size_t capacity;

    void resize(std::size_t newCapacity);

public:
    ArrayADT();
    ArrayADT(const ArrayADT& other);
    ArrayADT& operator=(const ArrayADT& other);
    ~ArrayADT();

    void push(const std::string& value);
    void insert(std::size_t index, const std::string& value);
    std::string get(std::size_t index) const;
    void remove(std::size_t index);
    void set(std::size_t index, const std::string& value);

    std::size_t size() const;
    const std::string* values() const;
};

#endif
