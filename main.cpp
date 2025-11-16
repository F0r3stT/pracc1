#include <iostream>
#include <string>
#include <fstream>
#include "stack.h"
#include "queue.h"
#include "set.h"
#include "hash.h"

using namespace std;



struct NamedStack {
    string name;
    Stack* stack;
    NamedStack* next;
};

struct NamedQueue {
    string name;
    Queue* queue;
    NamedQueue* next;
};

struct NamedSet {
    string name;
    Set* set;
    NamedSet* next;
};

struct NamedHash {
    string name;
    HashTable* ht;
    NamedHash* next;
};

// --- Глобальные головы списков ---
NamedStack* stackHead = nullptr;
NamedQueue* queueHead = nullptr;
NamedSet* setHead   = nullptr;
NamedHash* hashHead  = nullptr;

// --- Вспомогательные функции поиска/создания ---

Stack* getOrCreateStack(string name) {
    NamedStack* curr = stackHead;
    while (curr) {
        if (curr->name == name) return curr->stack;
        curr = curr->next;
    }
    // Не нашли - создаем
    NamedStack* newNode = new NamedStack;
    newNode->name = name;
    newNode->stack = new Stack();
    newNode->next = stackHead;
    stackHead = newNode;
    return newNode->stack;
}

Queue* getOrCreateQueue(string name) {
    NamedQueue* curr = queueHead;
    while (curr) {
        if (curr->name == name) return curr->queue;
        curr = curr->next;
    }
    NamedQueue* newNode = new NamedQueue;
    newNode->name = name;
    newNode->queue = new Queue();
    newNode->next = queueHead;
    queueHead = newNode;
    return newNode->queue;
}

Set* getOrCreateSet(string name) {
    NamedSet* curr = setHead;
    while (curr) {
        if (curr->name == name) return curr->set;
        curr = curr->next;
    }
    NamedSet* newNode = new NamedSet;
    newNode->name = name;
    newNode->set = new Set();
    newNode->next = setHead;
    setHead = newNode;
    return newNode->set;
}

HashTable* getOrCreateHash(string name) {
    NamedHash* curr = hashHead;
    while (curr) {
        if (curr->name == name) return curr->ht;
        curr = curr->next;
    }
    NamedHash* newNode = new NamedHash;
    newNode->name = name;
    newNode->ht = new HashTable();
    newNode->next = hashHead;
    hashHead = newNode;
    return newNode->ht;
}

// Функции поиска (без создания) для операций удаления/чтения
Stack* findStack(string name) {
    NamedStack* curr = stackHead;
    while(curr) { if(curr->name == name) return curr->stack; curr = curr->next; }
    return nullptr;
}
Queue* findQueue(string name) {
    NamedQueue* curr = queueHead;
    while(curr) { if(curr->name == name) return curr->queue; curr = curr->next; }
    return nullptr;
}
Set* findSet(string name) {
    NamedSet* curr = setHead;
    while(curr) { if(curr->name == name) return curr->set; curr = curr->next; }
    return nullptr;
}
HashTable* findHash(string name) {
    NamedHash* curr = hashHead;
    while(curr) { if(curr->name == name) return curr->ht; curr = curr->next; }
    return nullptr;
}

// --- Логика работы с файлами ---

void loadData(const string& path) {
    ifstream file(path);
    if (!file.is_open()) return;

    string type, name, key, val;
    // Читаем по словам: TYPE NAME KEY [VALUE]
    while (file >> type >> name >> key) {
        if (type == "STACK") {
            Stack* s = getOrCreateStack(name);
            s->push(key);
        } else if (type == "QUEUE") {
            Queue* q = getOrCreateQueue(name);
            q->push(key);
        } else if (type == "SET") {
            Set* s = getOrCreateSet(name);
            s->add(key);
        } else if (type == "HASH") {
            file >> val; // для хеша нужно еще значение
            HashTable* h = getOrCreateHash(name);
            h->set(key, val);
        }
    }
    file.close();
}

void saveData(const string& path) {
    ofstream file(path);
    if (!file.is_open()) {
        cout << "Ошибка записи в файл" << endl;
        return;
    }

    // Проходимся по всем спискам
    NamedStack* sNode = stackHead;
    while (sNode) {
        sNode->stack->saveFile(file, sNode->name);
        sNode = sNode->next;
    }

    NamedQueue* qNode = queueHead;
    while (qNode) {
        qNode->queue->saveFile(file, qNode->name);
        qNode = qNode->next;
    }

    NamedSet* stNode = setHead;
    while (stNode) {
        stNode->set->saveFile(file, stNode->name);
        stNode = stNode->next;
    }

    NamedHash* hNode = hashHead;
    while (hNode) {
        hNode->ht->saveToFile(file, hNode->name);
        hNode = hNode->next;
    }

    file.close();
}

void parseQuery(const string& qStr, string& cmd, string& name, string& key, string& val) {
    cmd = ""; name = ""; key = ""; val = "";
    int idx = 0;
    int len = qStr.length();

    auto skipSpaces = [&](int& i) { while(i < len && (qStr[i] == ' ' || qStr[i] == '\t')) i++; };
    auto readWord = [&](int& i) -> string {
        string res = "";
        while(i < len && qStr[i] != ' ' && qStr[i] != '\t') {
            res += qStr[i++];
        }
        return res;
    };

    skipSpaces(idx);
    cmd = readWord(idx);

    skipSpaces(idx);
    name = readWord(idx);

    skipSpaces(idx);
    key = readWord(idx);

    skipSpaces(idx);
    val = readWord(idx);
}

string executeQuery(string queryStr) {
    string cmd, name, key, val;
    parseQuery(queryStr, cmd, name, key, val);

    if (cmd == "" || name == "") return "Error: empty command";

    if (cmd == "SPUSH") {
        Stack* s = getOrCreateStack(name);
        s->push(key);
        return key;
    }
    if (cmd == "SPOP") {
        Stack* s = findStack(name);
        if (!s) return "Error: stack not found";
        return s->pop();
    }

    // QUEUE operations
    if (cmd == "QPUSH") {
        Queue* q = getOrCreateQueue(name);
        q->push(key);
        return key;
    }
    if (cmd == "QPOP") {
        Queue* q = findQueue(name);
        if (!q) return "Error: queue not found";
        return q->pop();
    }

    // SET operations
    if (cmd == "SADD") {
        Set* s = getOrCreateSet(name);
        s->add(key);
        return key;
    }
    if (cmd == "SREM") {
        Set* s = findSet(name);
        if (!s) return "Error: set not found";
        return s->remove(key);
    }
    if (cmd == "SISMEMBER") {
        Set* s = findSet(name);
        if (!s) return "FALSE";
        return s->isMember(key) ? "TRUE" : "FALSE";
    }

    // HASH operations
    if (cmd == "HSET") {
        HashTable* h = getOrCreateHash(name);
        h->set(key, val);
        return key + "->" + val;
    }
    if (cmd == "HDEL") {
        HashTable* h = findHash(name);
        if (!h) return "Error: hash not found";
        return h->remove(key);
    }
    if (cmd == "HGET") {
        HashTable* h = findHash(name);
        if (!h) return "Error: hash not found";
        return h->get(key);
    }

    return "Unknown command";
}

// --- Очистка памяти при выходе ---
void cleanup() {
    while (stackHead) {
        NamedStack* temp = stackHead;
        stackHead = stackHead->next;
        delete temp->stack;
        delete temp;
    }
    while (queueHead) {
        NamedQueue* temp = queueHead;
        queueHead = queueHead->next;
        delete temp->queue;
        delete temp;
    }
    while (setHead) {
        NamedSet* temp = setHead;
        setHead = setHead->next;
        delete temp->set;
        delete temp;
    }
    while (hashHead) {
        NamedHash* temp = hashHead;
        hashHead = hashHead->next;
        delete temp->ht;
        delete temp;
    }
}


int main(int argc, char* argv[]) {
    if (argc < 5) {
        cout << "./dbms --file <file> --query '<query>'" << endl;
        return 1;
    }

    string filePath = "";
    string queryStr = "";

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) filePath = argv[++i];
        else if (arg == "--query" && i + 1 < argc) queryStr = argv[++i];
    }

    if (filePath == "" || queryStr == "") return 1;

    loadData(filePath);
    
    string res = executeQuery(queryStr);
    if (res != "") cout << res << endl;

    saveData(filePath);
    cleanup();

    return 0;
}