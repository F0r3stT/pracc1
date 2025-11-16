#include "stack.h"

// Конструктор по умолчанию
Stack::Stack() : name(""), data(nullptr), capacity(0), size(0) {}

// Конструктор с именем
Stack::Stack(const string& stackName) : name(stackName), capacity(10), size(0) {
    data = new string[capacity];
}

// Конструктор копирования
// Необходим для корректной работы с массивами объектов
Stack::Stack(const Stack& other) : name(other.name), capacity(other.capacity), size(other.size) {
    data = new string[capacity];
    for (int i = 0; i < size; ++i) {
        data[i] = other.data[i];
    }
}

// Оператор присваивания
Stack& Stack::operator=(const Stack& other) {
    if (this != &other) {
        delete[] data;
        name = other.name;
        capacity = other.capacity;
        size = other.size;
        data = new string[capacity];
        for (int i = 0; i < size; ++i) {
            data[i] = other.data[i];
        }
    }
    return *this;
}

// Деструктор
// Освобождает память
Stack::~Stack() {
    if (data != nullptr) {
        delete[] data;
    }
}

// Увеличение размера массива
// Увеличивает емкость в 2 раза
void Stack::resize() {
    int newCapacity = (capacity == 0) ? 10 : capacity * 2;
    string* newData = new string[newCapacity];
    for (int i = 0; i < size; ++i) {
        newData[i] = data[i];
    }
    delete[] data;
    data = newData;
    capacity = newCapacity;
}

// Добавление элемента
bool Stack::SPUSH(const string& value) {
    if (value.empty()) return false;
    if (size == capacity) {
        resize();
    }
    data[size++] = value;
    return true;
}

// Удаление и получение элемента
string Stack::SPOP() {
    if (size == 0) return "ОШИБКА: Стек пуст";
    return data[--size];
}

// Проверка на пустоту
bool Stack::isEmpty() const {
    return size == 0;
}

// Получение имени
const string& Stack::getName() const {
    return name;
}

// Итератор начала (для for-loop)
string* Stack::begin() const {
    return data;
}

// Итератор конца (для for-loop)
string* Stack::end() const {
    return data + size;
}