#ifndef SET_H
#define SET_H

// Разрешенные библиотеки
#include <string>
#include <iostream>

// Используем стандартное пространство имен
using namespace std;

// Структура узла для хеш-таблицы (метод цепочек)
// Хранит ключ, индекс в плотном массиве и указатель на следующий узел
struct HashNode {
    string key;         // Само значение
    int denseIndex;     // Где лежит значение в массиве denseList
    HashNode* next;     // Следующий узел в цепочке (при коллизиях)
};

// Класс Set (Множество)
// Реализует операции O(1) используя "Dense Set" (Плотный массив + Разреженная карта)
class Set {
private:
    string name;            // Имя множества
    
    // Плотный массив для хранения значений (обеспечивает итерацию)
    string* denseList;
    
    // Массив указателей (хеш-таблица) для быстрого поиска
    HashNode** buckets;

    int itemCount;          // Текущее количество элементов
    int capacityVal;        // Размер плотного массива
    int bucketCount;        // Размер хеш-таблицы

    // Хеш-функция для строк
    size_t calc_hash(const string& val) const;

    // Метод расширения памяти (рехеширование)
    void resize_storage();

    // Метод очистки памяти (для деструктора и оператора присваивания)
    void clear_memory();

public:
    // Конструктор по умолчанию
    Set();

    // Конструктор с именем
    Set(const string& setName);

    // Конструктор копирования (Rule of Three)
    Set(const Set& other);

    // Оператор присваивания (Rule of Three)
    Set& operator=(const Set& other);

    // Деструктор
    ~Set();

    // Добавление элемента (SADD)
    // Сложность: O(1) в среднем
    bool SADD(const string& value);

    // Удаление элемента (SREM)
    // Сложность: O(1) в среднем
    bool SREM(const string& value);

    // Проверка наличия элемента (SISMEMBER)
    // Сложность: O(1) в среднем
    bool SISMEMBER(const string& value) const;

    // Получение имени
    const string& getName() const;

    // Методы для range-based for loop
    // Возвращают указатели на плотный массив
    string* begin() const;
    string* end() const;
};

#endif // SET_H