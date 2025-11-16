#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <string>

using namespace std;

// Класс Queue (Очередь)

class Queue {
private:
    string name;       
    string* data;      //динамический массив для хранения элементов
    int capacity;      // Размер выделенной памяти
    int head;          // Индекс первого элемента (откуда забираем)
    int tail;          // Индекс следующего свободного места (куда кладем)

    // Вспомогательный метод для перераспределения памяти
    // Вызывается, когда массив заполнен или требует оптимизации
    void reorganize_memory();

public:
    // Конструктор по умолчанию
    Queue();

    // Конструктор с именем очереди
    Queue(const string& queueName);

    // Конструктор копирования (Rule of Three)
    // Необходим для корректного копирования очередей
    Queue(const Queue& other);

    // Оператор присваивания (Rule of Three)
    Queue& operator=(const Queue& other);

    // Деструктор
    ~Queue();

    // Добавление элемента в конец очереди (QPUSH)
    // Сложность: O(1) амортизированная
    bool QPUSH(const string& value);

    // Чтение и удаление элемента из начала очереди (QPOP)
    // Сложность: O(1)
    string QPOP();

    // Проверка, пуста ли очередь
    bool isEmpty() const;

    // Получение имени очереди
    const string& getName() const;

    // Методы для поддержки Range-based for loop (for (auto x : queue))
    // begin() возвращает указатель на текущее начало данных (индекс head)
    string* begin() const;

    // end() возвращает указатель на позицию после последнего элемента (индекс tail)
    string* end() const;
};

#endif // QUEUE_H