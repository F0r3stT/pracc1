#include "database.h"

// Конструктор
// Инициализирует массивы с начальной емкостью 10 и загружает данные
Database::Database(const string& dbFilename) : filename(dbFilename) {
    setsCapacity = 10; setsCount = 0; sets = new Set*[setsCapacity];
    stackCapacity = 10; stackCount = 0; stacks = new Stack*[stackCapacity];
    queueCapacity = 10; queueCount = 0; queues = new Queue*[queueCapacity];
    hashCapacity = 10; hashCount = 0; hashTables = new HashTable*[hashCapacity];

    loadFromFile();
}

// Деструктор
// Освобождает всю выделенную память
Database::~Database() {
    clearMemory();
}

// Очистка памяти
void Database::clearMemory() {
    for (int i = 0; i < setsCount; ++i) delete sets[i];
    delete[] sets;

    for (int i = 0; i < stackCount; ++i) delete stacks[i];
    delete[] stacks;

    for (int i = 0; i < queueCount; ++i) delete queues[i];
    delete[] queues;

    for (int i = 0; i < hashCount; ++i) delete hashTables[i];
    delete[] hashTables;
}

// Простой парсер слов
// Пропускает пробелы, находит конец слова, возвращает подстроку
string Database::getNextWord(const string& str, int& pos) {
    // Пропуск разделителей (пробел, табуляция, перенос строки)
    while (pos < (int)str.length() && (str[pos] == ' ' || str[pos] == '\t' || str[pos] == '\n' || str[pos] == '\r')) {
        pos++;
    }

    if (pos >= (int)str.length()) return "";

    int start = pos;
    // Читаем до следующего разделителя
    while (pos < (int)str.length() && str[pos] != ' ' && str[pos] != '\t' && str[pos] != '\n' && str[pos] != '\r') {
        pos++;
    }

    return str.substr(start, pos - start);
}

// --- Вспомогательные методы добавления (аналог push_back) ---

void Database::addSet(Set* newSet) {
    if (setsCount == setsCapacity) {
        setsCapacity *= 2;
        Set** temp = new Set*[setsCapacity];
        for (int i = 0; i < setsCount; ++i) temp[i] = sets[i];
        delete[] sets;
        sets = temp;
    }
    sets[setsCount++] = newSet;
}

void Database::addStack(Stack* newStack) {
    if (stackCount == stackCapacity) {
        stackCapacity *= 2;
        Stack** temp = new Stack*[stackCapacity];
        for (int i = 0; i < stackCount; ++i) temp[i] = stacks[i];
        delete[] stacks;
        stacks = temp;
    }
    stacks[stackCount++] = newStack;
}

void Database::addQueue(Queue* newQueue) {
    if (queueCount == queueCapacity) {
        queueCapacity *= 2;
        Queue** temp = new Queue*[queueCapacity];
        for (int i = 0; i < queueCount; ++i) temp[i] = queues[i];
        delete[] queues;
        queues = temp;
    }
    queues[queueCount++] = newQueue;
}

void Database::addHashTable(HashTable* newHash) {
    if (hashCount == hashCapacity) {
        hashCapacity *= 2;
        HashTable** temp = new HashTable*[hashCapacity];
        for (int i = 0; i < hashCount; ++i) temp[i] = hashTables[i];
        delete[] hashTables;
        hashTables = temp;
    }
    hashTables[hashCount++] = newHash;
}

// --- Поиск структур по имени (Линейный поиск) ---

Set* Database::findSet(const string& name) {
    for (int i = 0; i < setsCount; ++i) {
        if (sets[i]->getName() == name) return sets[i];
    }
    return nullptr;
}

Stack* Database::findStack(const string& name) {
    for (int i = 0; i < stackCount; ++i) {
        if (stacks[i]->getName() == name) return stacks[i];
    }
    return nullptr;
}

Queue* Database::findQueue(const string& name) {
    for (int i = 0; i < queueCount; ++i) {
        if (queues[i]->getName() == name) return queues[i];
    }
    return nullptr;
}

HashTable* Database::findHashTable(const string& name) {
    for (int i = 0; i < hashCount; ++i) {
        if (hashTables[i]->getName() == name) return hashTables[i];
    }
    return nullptr;
}

// --- Валидация ---
bool Database::isValidName(const string& name) {
    if (name.empty()) return false;
    for (char c : name) {
        if (!isalnum(c) && c != '_') return false;
    }
    return true;
}

// --- Реализация операций ---

string Database::SADD(const string& setName, const string& value) {
    if (!isValidName(setName)) return "ОШИБКА: Некорректное имя";
    if (value.empty()) return "ОШИБКА: Пустое значение";

    Set* s = findSet(setName);
    if (!s) {
        s = new Set(setName);
        addSet(s);
    }
    
    if (s->SADD(value)) {
        saveToFile();
        return value; // Возвращаем добавленное значение при успехе
    }
    return "FALSE"; // Элемент уже был
}

string Database::SREM(const string& setName, const string& value) {
    Set* s = findSet(setName);
    if (s && s->SREM(value)) {
        saveToFile();
        return value;
    }
    return "FALSE";
}

string Database::SISMEMBER(const string& setName, const string& value) {
    Set* s = findSet(setName);
    if (s && s->SISMEMBER(value)) return "TRUE";
    return "FALSE";
}

string Database::SPUSH(const string& stackName, const string& value) {
    if (!isValidName(stackName)) return "ОШИБКА: Некорректное имя";
    
    Stack* s = findStack(stackName);
    if (!s) {
        s = new Stack(stackName);
        addStack(s);
    }
    
    if (s->SPUSH(value)) {
        saveToFile();
        return value;
    }
    return "ОШИБКА";
}

string Database::SPOP(const string& stackName) {
    Stack* s = findStack(stackName);
    if (!s || s->isEmpty()) return "ОШИБКА: Стек пуст или не существует";
    
    string val = s->SPOP();
    saveToFile();
    return val;
}

string Database::QPUSH(const string& queueName, const string& value) {
    if (!isValidName(queueName)) return "ОШИБКА: Некорректное имя";

    Queue* q = findQueue(queueName);
    if (!q) {
        q = new Queue(queueName);
        addQueue(q);
    }

    if (q->QPUSH(value)) {
        saveToFile();
        return value;
    }
    return "ОШИБКА";
}

string Database::QPOP(const string& queueName) {
    Queue* q = findQueue(queueName);
    if (!q || q->isEmpty()) return "ОШИБКА: Очередь пуста или не существует";
    
    string val = q->QPOP();
    saveToFile();
    return val;
}

string Database::HSET(const string& hashName, const string& key, const string& value) {
    if (!isValidName(hashName)) return "ОШИБКА: Некорректное имя";

    HashTable* ht = findHashTable(hashName);
    if (!ht) {
        ht = new HashTable(hashName);
        addHashTable(ht);
    }

    if (ht->HSET(key, value)) {
        saveToFile();
        return value; // Успешная вставка/обновление возвращает значение
    }
    return "FALSE";
}

string Database::HDEL(const string& hashName, const string& key) {
    HashTable* ht = findHashTable(hashName);
    if (ht && ht->HDEL(key)) {
        saveToFile();
        return key; // Возвращаем удаленный ключ
    }
    return "FALSE";
}

string Database::HGET(const string& hashName, const string& key) {
    HashTable* ht = findHashTable(hashName);
    if (ht) return ht->HGET(key);
    return "ОШИБКА: Хеш-таблица не найдена";
}

// --- Сохранение в файл ---
// Формат: ТИП ИМЯ ЭЛЕМЕНТ1 ЭЛЕМЕНТ2 ...
bool Database::saveToFile() {
    ofstream file(filename);
    if (!file.is_open()) return false;

    // 1. Сохраняем множества
    for (int i = 0; i < setsCount; ++i) {
        file << "SET " << sets[i]->getName();
        // Используем итераторы begin()/end(), реализованные в set.h
        for (string* ptr = sets[i]->begin(); ptr != sets[i]->end(); ++ptr) {
            file << " " << *ptr;
        }
        file << endl;
    }

    // 2. Сохраняем стеки
    for (int i = 0; i < stackCount; ++i) {
        file << "STACK " << stacks[i]->getName();
        // Важно: сохраняем порядок для корректного восстановления
        for (string* ptr = stacks[i]->begin(); ptr != stacks[i]->end(); ++ptr) {
            file << " " << *ptr;
        }
        file << endl;
    }

    // 3. Сохраняем очереди
    for (int i = 0; i < queueCount; ++i) {
        file << "QUEUE " << queues[i]->getName();
        for (string* ptr = queues[i]->begin(); ptr != queues[i]->end(); ++ptr) {
            file << " " << *ptr;
        }
        file << endl;
    }

    // 4. Сохраняем хеш-таблицы
    for (int i = 0; i < hashCount; ++i) {
        file << "HASH " << hashTables[i]->getName();
        for (KeyValuePair* ptr = hashTables[i]->begin(); ptr != hashTables[i]->end(); ++ptr) {
            // Формат сохранения ключ:значение
            file << " " << ptr->key << ":" << ptr->value;
        }
        file << endl;
    }

    file.close();
    return true;
}

// --- Загрузка из файла ---
bool Database::loadFromFile() {
    ifstream file(filename);
    if (!file.is_open()) return true; // Файла нет - это норма для новой БД

    string line;
    while (getline(file, line)) {
        int pos = 0;
        string type = getNextWord(line, pos);
        if (type.empty()) continue;

        string name = getNextWord(line, pos);
        if (name.empty()) continue;

        // Восстановление множества
        if (type == "SET") {
            Set* s = new Set(name);
            addSet(s);
            while (true) {
                string val = getNextWord(line, pos);
                if (val.empty()) break;
                s->SADD(val);
            }
        }
        // Восстановление стека
        else if (type == "STACK") {
            Stack* s = new Stack(name);
            addStack(s);
            while (true) {
                string val = getNextWord(line, pos);
                if (val.empty()) break;
                s->SPUSH(val);
            }
        }
        // Восстановление очереди
        else if (type == "QUEUE") {
            Queue* q = new Queue(name);
            addQueue(q);
            while (true) {
                string val = getNextWord(line, pos);
                if (val.empty()) break;
                q->QPUSH(val);
            }
        }
        // Восстановление хеш-таблицы
        else if (type == "HASH") {
            HashTable* ht = new HashTable(name);
            addHashTable(ht);
            while (true) {
                string pair = getNextWord(line, pos);
                if (pair.empty()) break;
                
                // Ручной разбор строки вида "key:value"
                size_t colon = pair.find(':');
                if (colon != string::npos) {
                    string k = pair.substr(0, colon);
                    string v = pair.substr(colon + 1);
                    ht->HSET(k, v);
                }
            }
        }
    }
    file.close();
    return true;
}

// --- Обработка запроса ---
string Database::executeQuery(const string& query) {
    int pos = 0;
    string op = getNextWord(query, pos);    // Операция (SADD, SPOP...)
    string name = getNextWord(query, pos);  // Имя контейнера
    string arg1 = getNextWord(query, pos);  // Аргумент 1 (значение или ключ)
    string arg2 = getNextWord(query, pos);  // Аргумент 2 (значение для HSET)

    if (op.empty() || name.empty()) return "ОШИБКА: Неверный формат запроса";

    if (op == "SADD") return SADD(name, arg1);
    if (op == "SREM") return SREM(name, arg1);
    if (op == "SISMEMBER") return SISMEMBER(name, arg1);
    
    if (op == "SPUSH") return SPUSH(name, arg1);
    if (op == "SPOP") return SPOP(name);
    
    if (op == "QPUSH") return QPUSH(name, arg1);
    if (op == "QPOP") return QPOP(name);
    
    if (op == "HSET") return HSET(name, arg1, arg2);
    if (op == "HDEL") return HDEL(name, arg1);
    if (op == "HGET") return HGET(name, arg1);

    return "ОШИБКА: Неизвестная команда";
}