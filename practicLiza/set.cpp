#include "set.h"

// Конструктор по умолчанию
Set::Set() : name("Default"), denseList(nullptr), buckets(nullptr), 
             itemCount(0), capacityVal(0), bucketCount(0) {}

// Конструктор с именем
// Инициализируем начальные размеры
Set::Set(const string& setName) 
    : name(setName), itemCount(0), capacityVal(10), bucketCount(20) {
    
    denseList = new string[capacityVal];
    buckets = new HashNode*[bucketCount];
    
    // Зануляем ведра хеш-таблицы
    for (int i = 0; i < bucketCount; ++i) {
        buckets[i] = nullptr;
    }
}

// Вспомогательный метод очистки памяти
void Set::clear_memory() {
    if (buckets != nullptr) {
        for (int i = 0; i < bucketCount; ++i) {
            HashNode* current = buckets[i];
            while (current != nullptr) {
                HashNode* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] buckets;
    }
    if (denseList != nullptr) {
        delete[] denseList;
    }
}

// Деструктор
Set::~Set() {
    clear_memory();
}

// Конструктор копирования
Set::Set(const Set& other) 
    : name(other.name), itemCount(other.itemCount), 
      capacityVal(other.capacityVal), bucketCount(other.bucketCount) {
    
    // Выделяем новую память
    denseList = new string[capacityVal];
    buckets = new HashNode*[bucketCount];

    // Зануляем ведра
    for (int i = 0; i < bucketCount; ++i) {
        buckets[i] = nullptr;
    }

    // Копируем данные
    // Проходим по плотному массиву оригинала и добавляем в копию
    // Это проще, чем копировать структуру указателей вручную
    itemCount = 0; // Сбрасываем счетчик, так как SADD его увеличит
    for (int i = 0; i < other.itemCount; ++i) {
        SADD(other.denseList[i]); // Используем SADD для корректного заполнения хеша
    }
}

// Оператор присваивания
Set& Set::operator=(const Set& other) {
    if (this != &other) {
        clear_memory();

        name = other.name;
        capacityVal = other.capacityVal;
        bucketCount = other.bucketCount;
        
        denseList = new string[capacityVal];
        buckets = new HashNode*[bucketCount];

        for (int i = 0; i < bucketCount; ++i) {
            buckets[i] = nullptr;
        }

        itemCount = 0;
        for (int i = 0; i < other.itemCount; ++i) {
            SADD(other.denseList[i]);
        }
    }
    return *this;
}

// Простая хеш-функция (полиномиальный хеш)
size_t Set::calc_hash(const string& val) const {
    size_t hashVal = 0;
    size_t primeNum = 31;
    for (char c : val) {
        hashVal = hashVal * primeNum + c;
    }
    return hashVal % bucketCount;
}

// Расширение памяти
void Set::resize_storage() {
    int oldSize = capacityVal;
    int oldBucketCount = bucketCount;
    
    // Увеличиваем размеры
    capacityVal *= 2;
    bucketCount *= 2;

    string* oldDense = denseList;
    HashNode** oldBuckets = buckets;

    // Создаем новые массивы
    denseList = new string[capacityVal];
    buckets = new HashNode*[bucketCount];
    for (int i = 0; i < bucketCount; ++i) buckets[i] = nullptr;

    // Перехеширование
    // Мы берем элементы из старого плотного массива и вставляем в новые структуры
    // Это быстрее, чем перебирать узлы старой хеш-таблицы
    int oldItemCount = itemCount;
    itemCount = 0; // Сбрасываем для корректной работы SADD

    for (int i = 0; i < oldItemCount; ++i) {
        SADD(oldDense[i]);
    }

    // Удаляем старое
    delete[] oldDense;
    // Удаляем старые узлы хеша
    for (int i = 0; i < oldBucketCount; ++i) {
        HashNode* curr = oldBuckets[i];
        while (curr != nullptr) {
            HashNode* tmp = curr;
            curr = curr->next;
            delete tmp;
        }
    }
    delete[] oldBuckets;
}

// Проверка наличия элемента (SISMEMBER)
bool Set::SISMEMBER(const string& value) const {
    if (value.empty() || bucketCount == 0) return false;

    size_t index = calc_hash(value);
    HashNode* current = buckets[index];

    // Ищем в цепочке
    while (current != nullptr) {
        if (current->key == value) {
            return true;
        }
        current = current->next;
    }
    return false;
}

// Добавление элемента (SADD)
bool Set::SADD(const string& value) {
    if (value.empty()) return false;

    // Если массив не создан
    if (denseList == nullptr) {
        capacityVal = 10;
        bucketCount = 20;
        denseList = new string[capacityVal];
        buckets = new HashNode*[bucketCount];
        for (int i = 0; i < bucketCount; ++i) buckets[i] = nullptr;
    }

    // Сначала проверяем, есть ли элемент
    if (SISMEMBER(value)) {
        return false; // Элемент уже существует
    }

    // Проверяем заполненность (Load Factor)
    if (itemCount >= capacityVal) {
        resize_storage();
    }

    // 1. Добавляем в конец плотного массива
    denseList[itemCount] = value;

    // 2. Добавляем в хеш-таблицу
    size_t hIdx = calc_hash(value);
    HashNode* newNode = new HashNode;
    newNode->key = value;
    newNode->denseIndex = itemCount; // Запоминаем, где он лежит в массиве
    newNode->next = buckets[hIdx];   // Вставка в начало списка (цепочки)
    buckets[hIdx] = newNode;

    itemCount++;
    return true;
}

// Удаление элемента (SREM)
bool Set::SREM(const string& value) {
    if (value.empty() || itemCount == 0) return false;

    size_t hIdx = calc_hash(value);
    HashNode* current = buckets[hIdx];
    HashNode* prev = nullptr;

    // 1. Ищем узел в хеш-таблице
    while (current != nullptr) {
        if (current->key == value) {
            // Элемент найден. Начинаем процесс удаления.
            
            // Индекс удаляемого элемента в плотном массиве
            int indexToRemove = current->denseIndex;
            // Индекс последнего элемента в плотном массиве
            int indexLast = itemCount - 1;

            // Если удаляемый элемент НЕ последний, делаем Swap
            if (indexToRemove != indexLast) {
                // Берем строку последнего элемента
                string lastVal = denseList[indexLast];
                
                // Перемещаем последний элемент на место удаляемого
                denseList[indexToRemove] = lastVal;

                // ТЕПЕРЬ САМОЕ ВАЖНОЕ: Нужно обновить индекс перемещенного элемента в хеш-таблице
                size_t lastHash = calc_hash(lastVal);
                HashNode* nodeToUpdate = buckets[lastHash];
                while (nodeToUpdate != nullptr) {
                    if (nodeToUpdate->key == lastVal) {
                        // Обновляем его ссылку на массив
                        nodeToUpdate->denseIndex = indexToRemove;
                        break;
                    }
                    nodeToUpdate = nodeToUpdate->next;
                }
            }

            // Удаляем узел из цепочки хеш-таблицы
            if (prev == nullptr) {
                buckets[hIdx] = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;

            // Уменьшаем размер
            itemCount--;
            return true;
        }
        prev = current;
        current = current->next;
    }

    return false; // Элемент не найден
}

// Получение имени
const string& Set::getName() const {
    return name;
}

// Начало итерации (указатель на начало массива)
string* Set::begin() const {
    return denseList;
}

// Конец итерации (указатель на элемент за последним)
string* Set::end() const {
    return denseList + itemCount;
}