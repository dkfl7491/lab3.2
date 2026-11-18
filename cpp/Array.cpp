#include "Array.h"
#include <stdexcept>

// Создание пустого массива.
ArrayADT::ArrayADT()
{
    count = 0;
    capacity = 4;
    data = new std::string[capacity];
}

// Создание копии массива.
ArrayADT::ArrayADT(const ArrayADT& other)
{
    count = other.count;
    capacity = other.capacity;

    data = new std::string[capacity];

    for (std::size_t i = 0; i < count; i++)
        data[i] = other.data[i];
}

// Оператор присваивания.
ArrayADT& ArrayADT::operator=(const ArrayADT& other)
{
    if (this == &other)
        return *this;

    std::string* newData = new std::string[other.capacity];

    for (std::size_t i = 0; i < other.count; i++)
        newData[i] = other.data[i];

    delete[] data;

    data = newData;
    count = other.count;
    capacity = other.capacity;

    return *this;
}

// Освобождение памяти.
ArrayADT::~ArrayADT()
{
    delete[] data;
}

// Изменение вместимости массива.
void ArrayADT::resize(std::size_t newCapacity)
{
    std::string* newData = new std::string[newCapacity];

    for (std::size_t i = 0; i < count; i++)
        newData[i] = data[i];

    delete[] data;

    data = newData;
    capacity = newCapacity;
}

// Добавление элемента в конец массива.
void ArrayADT::push(const std::string& value)
{
    if (count == capacity)
        resize(capacity * 2);

    data[count] = value;
    count++;
}

// Вставка элемента по индексу.
void ArrayADT::insert(std::size_t index, const std::string& value)
{
    if (index > count)
        throw std::out_of_range("Invalid array index");

    if (count == capacity)
        resize(capacity * 2);

    for (std::size_t i = count; i > index; i--)
        data[i] = data[i - 1];

    data[index] = value;
    count++;
}

// Получение элемента по индексу.
std::string ArrayADT::get(std::size_t index) const
{
    if (index >= count)
        throw std::out_of_range("Invalid array index");

    return data[index];
}

// Удаление элемента по индексу.
void ArrayADT::remove(std::size_t index)
{
    if (index >= count)
        throw std::out_of_range("Invalid array index");

    for (std::size_t i = index; i < count - 1; i++)
        data[i] = data[i + 1];

    count--;
}

// Замена элемента по индексу.
void ArrayADT::set(std::size_t index, const std::string& value)
{
    if (index >= count)
        throw std::out_of_range("Invalid array index");

    data[index] = value;
}

// Получение количества элементов.
std::size_t ArrayADT::size() const
{
    return count;
}

// Получение указателя на элементы массива.
const std::string* ArrayADT::values() const
{
    return data;
}
