#ifndef HASH_TABLE_H
#define HASH_TABLE_H

// Разрешенные библиотеки
#include <string>
#include <iostream>

// Используем стандартное пространство имен
using namespace std;

// Структура для хранения пары Ключ-Значение
// Используется в плотном массиве для поддержки итерации
class KeyValuePair {
public:
    string key;
    string value;
};

// Переименовали HashNode в TableNode, чтобы избежать конфликта с set.h
// Узел хеш-таблицы (для метода цепочек)
class TableNode {
public:
    string key;         // Ключ поиска
    int denseIndex;     // Индекс в массиве pairsList
    TableNode* next;    // Следующий узел в случае коллизии
};

// Класс HashTable
// Реализует ассоциативный массив с операциями O(1)
class HashTable {
private:
    string name;            // Имя таблицы
    
    // Плотный массив для хранения пар (обеспечивает begin/end)
    KeyValuePair* pairsList;
    
    // Разреженная хеш-таблица для поиска индексов
    // Используем TableNode вместо HashNode
    TableNode** buckets;

    int itemCount;          // Текущее количество элементов
    int capacityVal;        // Емкость плотного массива
    int bucketCount;        // Количество корзин (ведер) в хеш-таблице

    // Хеш-функция
    size_t calc_hash(const string& key) const;

    // Увеличение размера хранилища
    void resize_storage();

    // Очистка памяти (помощник для деструктора)
    void clear_memory();

    // Поиск узла по ключу (возвращает указатель на узел или nullptr)
    TableNode* find_node(const string& key) const;

public:
    // Конструктор по умолчанию
    HashTable();

    // Конструктор с именем
    HashTable(const string& hashName);

    // Конструктор копирования (Rule of Three)
    HashTable(const HashTable& other);

    // Оператор присваивания (Rule of Three)
    HashTable& operator=(const HashTable& other);

    // Деструктор
    ~HashTable();

    // Добавление или обновление элемента (HSET)
    // Сложность: O(1) в среднем
    bool HSET(const string& key, const string& value);

    // Удаление элемента по ключу (HDEL)
    // Сложность: O(1) в среднем
    bool HDEL(const string& key);

    // Получение значения по ключу (HGET)
    // Сложность: O(1) в среднем
    string HGET(const string& key) const;

    // Проверка наличия ключа
    bool contains(const string& key) const;

    // Получение имени
    const string& getName() const;

    // Методы для поддержки цикла for (auto& pair : table)
    KeyValuePair* begin() const;
    KeyValuePair* end() const;
};

#endif // HASH_TABLE_H