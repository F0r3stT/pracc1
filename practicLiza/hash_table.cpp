#include "hash_table.h"

// Конструктор по умолчанию
HashTable::HashTable() 
    : name("Default"), pairsList(nullptr), buckets(nullptr), 
      itemCount(0), capacityVal(0), bucketCount(0) {}

// Конструктор с именем
// Выделяем начальную память
HashTable::HashTable(const string& hashName) 
    : name(hashName), itemCount(0), capacityVal(10), bucketCount(20) {
    
    pairsList = new KeyValuePair[capacityVal];
    buckets = new TableNode*[bucketCount];
    
    // Инициализируем ведра нулями
    for (int i = 0; i < bucketCount; ++i) {
        buckets[i] = nullptr;
    }
}

// Хеш-функция
// Использует простой алгоритм Горнера
size_t HashTable::calc_hash(const string& key) const {
    size_t hashVal = 0;
    size_t primeNum = 31;
    for (char c : key) {
        hashVal = hashVal * primeNum + c;
    }
    return hashVal % bucketCount;
}

// Очистка памяти
void HashTable::clear_memory() {
    if (buckets != nullptr) {
        for (int i = 0; i < bucketCount; ++i) {
            TableNode* current = buckets[i];
            while (current != nullptr) {
                TableNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] buckets;
    }
    if (pairsList != nullptr) {
        delete[] pairsList;
    }
}

// Деструктор
HashTable::~HashTable() {
    clear_memory();
}

// Конструктор копирования
HashTable::HashTable(const HashTable& other) 
    : name(other.name), itemCount(other.itemCount), 
      capacityVal(other.capacityVal), bucketCount(other.bucketCount) {
    
    pairsList = new KeyValuePair[capacityVal];
    buckets = new TableNode*[bucketCount];

    for (int i = 0; i < bucketCount; ++i) {
        buckets[i] = nullptr;
    }

    // Пересоздаем таблицу путем повторной вставки
    itemCount = 0; // Сброс перед вставкой
    for (int i = 0; i < other.itemCount; ++i) {
        HSET(other.pairsList[i].key, other.pairsList[i].value);
    }
}

// Оператор присваивания
HashTable& HashTable::operator=(const HashTable& other) {
    if (this != &other) {
        clear_memory();

        name = other.name;
        capacityVal = other.capacityVal;
        bucketCount = other.bucketCount;
        
        pairsList = new KeyValuePair[capacityVal];
        buckets = new TableNode*[bucketCount];
        
        for (int i = 0; i < bucketCount; ++i) {
            buckets[i] = nullptr;
        }

        itemCount = 0;
        for (int i = 0; i < other.itemCount; ++i) {
            HSET(other.pairsList[i].key, other.pairsList[i].value);
        }
    }
    return *this;
}

// Увеличение размера таблицы (Рехеширование)
void HashTable::resize_storage() {
    // Сохраняем старые данные
    int oldCount = itemCount;
    KeyValuePair* oldPairs = pairsList;
    
    // Увеличиваем параметры
    capacityVal *= 2;
    bucketCount *= 2;
    
    // Очищаем старую хеш-сетку (но не данные пар, они нам нужны для переноса)
    for (int i = 0; i < bucketCount / 2; ++i) {
        TableNode* curr = buckets[i];
        while (curr != nullptr) {
            TableNode* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
    }
    delete[] buckets;

    // Выделяем новую память
    pairsList = new KeyValuePair[capacityVal];
    buckets = new TableNode*[bucketCount];
    for (int i = 0; i < bucketCount; ++i) buckets[i] = nullptr;

    // Переносим данные (просто вызываем HSET для каждого старого элемента)
    itemCount = 0; // Сбрасываем счетчик
    for (int i = 0; i < oldCount; ++i) {
        HSET(oldPairs[i].key, oldPairs[i].value);
    }

    delete[] oldPairs;
}

// Добавление или обновление (HSET)
bool HashTable::HSET(const string& key, const string& value) {
    if (key.empty() || value.empty()) return false;

    // Инициализация, если пустой
    if (pairsList == nullptr) {
        capacityVal = 10;
        bucketCount = 20;
        pairsList = new KeyValuePair[capacityVal];
        buckets = new TableNode*[bucketCount];
        for (int i = 0; i < bucketCount; ++i) buckets[i] = nullptr;
    }

    // 1. Проверяем, есть ли ключ. Если есть - обновляем значение.
    size_t hIdx = calc_hash(key);
    TableNode* current = buckets[hIdx];
    while (current != nullptr) {
        if (current->key == key) {
            // Ключ найден, обновляем значение в плотном массиве
            pairsList[current->denseIndex].value = value;
            return true;
        }
        current = current->next;
    }

    // 2. Если ключа нет, добавляем новый
    if (itemCount >= capacityVal) {
        resize_storage();
        // Пересчитываем хеш, так как bucketCount изменился
        hIdx = calc_hash(key); 
    }

    // Добавляем в конец плотного массива
    pairsList[itemCount].key = key;
    pairsList[itemCount].value = value;

    // Добавляем узел в хеш-таблицу
    TableNode* newNode = new TableNode;
    newNode->key = key;
    newNode->denseIndex = itemCount;
    newNode->next = buckets[hIdx];
    buckets[hIdx] = newNode;

    itemCount++;
    return true;
}

// Получение значения (HGET)
string HashTable::HGET(const string& key) const {
    if (key.empty() || bucketCount == 0) return "ОШИБКА: Пустой ключ";

    size_t hIdx = calc_hash(key);
    TableNode* current = buckets[hIdx];

    while (current != nullptr) {
        if (current->key == key) {
            // Берем значение из плотного массива по индексу
            return pairsList[current->denseIndex].value;
        }
        current = current->next;
    }

    return "ОШИБКА: Ключ не найден";
}

// Удаление (HDEL)
// Использует паттерн "Swap and Pop" для O(1) удаления из массива
bool HashTable::HDEL(const string& key) {
    if (key.empty() || itemCount == 0) return false;

    size_t hIdx = calc_hash(key);
    TableNode* current = buckets[hIdx];
    TableNode* prev = nullptr;

    while (current != nullptr) {
        if (current->key == key) {
            // Нашли элемент для удаления
            int indexToRemove = current->denseIndex;
            int indexLast = itemCount - 1;

            // Если удаляем не последний элемент, перемещаем последний на место удаляемого
            if (indexToRemove != indexLast) {
                // Переносим данные
                pairsList[indexToRemove] = pairsList[indexLast];

                // Находим узел хеш-таблицы, который ссылался на последний элемент
                // и обновляем его индекс
                string lastKey = pairsList[indexLast].key;
                size_t lastHIdx = calc_hash(lastKey);
                TableNode* nodeToUpdate = buckets[lastHIdx];
                
                while (nodeToUpdate != nullptr) {
                    if (nodeToUpdate->key == lastKey) {
                        nodeToUpdate->denseIndex = indexToRemove;
                        break;
                    }
                    nodeToUpdate = nodeToUpdate->next;
                }
            }

            // Удаляем узел из цепочки
            if (prev == nullptr) {
                buckets[hIdx] = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;

            itemCount--;
            return true;
        }
        prev = current;
        current = current->next;
    }

    return false;
}

// Проверка наличия
bool HashTable::contains(const string& key) const {
    if (key.empty() || bucketCount == 0) return false;
    
    size_t hIdx = calc_hash(key);
    TableNode* current = buckets[hIdx];
    while (current != nullptr) {
        if (current->key == key) return true;
        current = current->next;
    }
    return false;
}

// Имя таблицы
const string& HashTable::getName() const {
    return name;
}

// Начало итерации
KeyValuePair* HashTable::begin() const {
    return pairsList;
}

// Конец итерации
KeyValuePair* HashTable::end() const {
    return pairsList + itemCount;
}