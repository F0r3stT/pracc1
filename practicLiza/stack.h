#ifndef STACK_H
#define STACK_H

#include <string>
#include <iostream>

using namespace std;

// Класс Стек
// Реализован на основе динамического массива
class Stack {
private:
    string name;
    string* data;    // Динамический массив
    int capacity;    // Текущая вместимость
    int size;        // Текущее количество элементов

    // Метод расширения массива
    void resize();

public:
    // Конструкторы и деструктор
    Stack();
    Stack(const string& stackName);
    Stack(const Stack& other); // Конструктор копирования
    Stack& operator=(const Stack& other); // Оператор присваивания
    ~Stack();

    // Основные операции
    bool SPUSH(const string& value);
    string SPOP();
    bool isEmpty() const;
    const string& getName() const;

    // Методы для поддержки Range-based for loop
    string* begin() const;
    string* end() const;
};

#endif