#ifndef DATABASE_H
#define DATABASE_H

// Разрешенные библиотеки
#include <string>
#include <iostream>
#include <fstream>

// Подключаем файлы структур данных (должны лежать в той же папке)
#include "set.h"
#include "stack.h"
#include "queue.h"
#include "hash_table.h"

using namespace std;

// Класс Database
// Управляет коллекциями структур данных, загрузкой/сохранением и выполнением запросов.
class Database {
private:
    string filename; // Имя файла для хранения данных

    // Массивы указателей на наши структуры.
    // Используем массив указателей вместо vector, чтобы избежать лишних копирований объектов
    // и управлять памятью вручную.
    
    Set** sets;          // Массив указателей на множества
    int setsCount;       // Текущее количество множеств
    int setsCapacity;    // Вместимость массива

    Stack** stacks;      // Массив указателей на стеки
    int stackCount;
    int stackCapacity;

    Queue** queues;      // Массив указателей на очереди
    int queueCount;
    int queueCapacity;

    HashTable** hashTables; // Массив указателей на хеш-таблицы
    int hashCount;
    int hashCapacity;

    // --- Вспомогательные методы управления массивами ---
    // Добавляют указатель в массив, при необходимости расширяя его (realloc)
    void addSet(Set* newSet);
    void addStack(Stack* newStack);
    void addQueue(Queue* newQueue);
    void addHashTable(HashTable* newHash);

    // --- Поиск структуры по имени ---
    // Возвращает указатель на структуру или nullptr, если не найдена
    Set* findSet(const string& name);
    Stack* findStack(const string& name);
    Queue* findQueue(const string& name);
    HashTable* findHashTable(const string& name);

    // --- Парсинг ---
    // Читает следующее слово из строки str, начиная с позиции pos.
    // Обновляет pos, сдвигая его за прочитанное слово.
    string getNextWord(const string& str, int& pos);

    // --- Валидация ---
    // Проверяет, что имя состоит только из букв, цифр и '_'
    bool isValidName(const string& name);

    // --- Очистка памяти ---
    // Удаляет все структуры и освобождает массивы
    void clearMemory();

public:
    // Конструктор: инициализирует массивы и загружает данные из файла
    Database(const string& dbFilename);
    
    // Деструктор: сохраняет данные (опционально) и очищает память
    ~Database();

    // --- Операции Множества (Set) ---
    string SADD(const string& setName, const string& value);
    string SREM(const string& setName, const string& value);
    string SISMEMBER(const string& setName, const string& value);

    // --- Операции Стека (Stack) ---
    string SPUSH(const string& stackName, const string& value);
    string SPOP(const string& stackName);

    // --- Операции Очереди (Queue) ---
    string QPUSH(const string& queueName, const string& value);
    string QPOP(const string& queueName);

    // --- Операции Хеш-таблицы (HashTable) ---
    string HSET(const string& hashName, const string& key, const string& value);
    string HDEL(const string& hashName, const string& key);
    string HGET(const string& hashName, const string& key);

    // --- Работа с файлами ---
    bool saveToFile();    // Сохранение всей базы в файл
    bool loadFromFile();  // Загрузка базы из файла

    // --- Выполнение запроса ---
    // Парсит строку запроса (например, "SADD myset val") и вызывает нужный метод
    string executeQuery(const string& query);
};

#endif // DATABASE_H