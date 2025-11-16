#include "queue.h"

// Конструктор по умолчанию
// Инициализируем нулевые индексы и пустой указатель
Queue::Queue() : name("Default"), data(nullptr), capacity(0), head(0), tail(0) {}

// Конструктор с именем
// Сразу выделяем небольшую память (например, под 10 элементов)
Queue::Queue(const string& queueName) : name(queueName), capacity(10), head(0), tail(0) {
    data = new string[capacity];
}

// Конструктор копирования
// Создает глубокую копию объекта
Queue::Queue(const Queue& other) 
    : name(other.name), head(0), tail(0) {
    
    // Вычисляем реальное количество элементов в копируемой очереди
    int count = other.tail - other.head;
    
    // Если элементов нет, создаем минимальный буфер
    if (count <= 0) {
        capacity = 10;
        data = new string[capacity];
    } else {
        // Выделяем память ровно под данные + запас
        capacity = count + 10;
        data = new string[capacity];
        
        // Копируем только активные элементы (от head до tail)
        // В новой очереди они встанут с индекса 0
        for (int i = 0; i < count; ++i) {
            data[i] = other.data[other.head + i];
        }
        tail = count; // Устанавливаем новый хвост
    }
}

// Оператор присваивания
Queue& Queue::operator=(const Queue& other) {
    // Защита от самоприсваивания
    if (this != &other) {
        // Освобождаем старую память
        if (data != nullptr) {
            delete[] data;
        }

        name = other.name;
        
        // Логика копирования данных (аналогично конструктору копирования)
        int count = other.tail - other.head;
        if (count <= 0) {
            capacity = 10;
            data = new string[capacity];
            head = 0;
            tail = 0;
        } else {
            capacity = count + 10;
            data = new string[capacity];
            for (int i = 0; i < count; ++i) {
                data[i] = other.data[other.head + i];
            }
            head = 0;
            tail = count;
        }
    }
    return *this;
}

// Деструктор
Queue::~Queue() {
    if (data != nullptr) {
        delete[] data;
    }
}

// Метод расширения/оптимизации памяти
void Queue::reorganize_memory() {
    // Если массив не создан, создаем
    if (capacity == 0) {
        capacity = 10;
        data = new string[capacity];
        return;
    }

    // Вычисляем текущее количество элементов
    int count = tail - head;
    
    // Новая емкость: либо удваиваем, либо просто выравниваем, если было много "мертвых" душ в начале
    // Здесь для простоты всегда увеличиваем емкость в 2 раза от текущей
    int newCapacity = capacity * 2;
    string* newData = new string[newCapacity];

   
    for (int i = 0; i < count; ++i) {
        newData[i] = data[head + i];
    }

    delete[] data;
    data = newData;
    capacity = newCapacity;
    
    // Сбрасываем индексы
    head = 0;
    tail = count;
}

//Добавление элемента 
bool Queue::QPUSH(const string& value) {
    // Проверка на пустое значение
    if (value.empty()) return false;

    // Если массив не инициализирован
    if (data == nullptr) {
        reorganize_memory();
    }

    
    if (tail == capacity) {
        reorganize_memory();
    }

    //записываем значение и сдвигаем хвост
    data[tail++] = value;
    return true;
}

//чтение и удаление элемента (QPOP)
string Queue::QPOP() {
    // Проверка на пустоту (если голова догнала хвост)
    if (head == tail) {
        // Возвращаем сообщение об ошибке
        return "ОШИБКА: Очередь пуста";
    }

    // Возвращаем элемент и сдвигаем голову вперед
    // Память физически не освобождается, но логически ячейка становится свободной
    // Это обеспечивает O(1)
    return data[head++];
}

// Проверка на пустоту
bool Queue::isEmpty() const {
    return head == tail;
}

// Получение имени
const string& Queue::getName() const {
    return name;
}

// Итератор на начало (для range-based for)
string* Queue::begin() const {
    // Возвращаем адрес элемента, на который указывает head
    return data + head;
}

// Итератор на конец (для range-based for)
string* Queue::end() const {
    // Возвращаем адрес элемента, на который указывает tail (первый свободный)
    return data + tail;
}