#include "hash.h"

// Реализация хеш-функции внутри класса
int HashTable::hashFunc(const string& key, int cap) const {
    int hash = 0;
    for (char ch : key) {
        hash = (hash * 31 + abs((int)ch)) % cap;
    }
    return hash;
}

HashTable::HashTable(int size) {
    this->capacity = size;
    this->count = 0;
    this->buckets = new HashNode*[size];
    for (int i = 0; i < size; i++) {
        this->buckets[i] = nullptr;
    }
}

HashTable::~HashTable() {
    for (int i = 0; i < capacity; i++) {
        HashNode* curr = buckets[i];
        while (curr != nullptr) {
            HashNode* next = curr->next;
            delete curr;
            curr = next;
        }
    }
    delete[] buckets;
}

void HashTable::resize() {
    int newCapacity = capacity * 2;
    HashNode** newBuckets = new HashNode*[newCapacity];
    for(int i=0; i<newCapacity; i++) newBuckets[i] = nullptr;

    for (int i = 0; i < capacity; i++) {
        HashNode* curr = buckets[i];
        while (curr != nullptr) {
            HashNode* next = curr->next;
            
            int newIdx = hashFunc(curr->key, newCapacity);
            
            curr->next = newBuckets[newIdx];
            newBuckets[newIdx] = curr;

            curr = next;
        }
    }
    delete[] buckets;
    buckets = newBuckets;
    capacity = newCapacity;
}

void HashTable::set(string key, string value) {
    int idx = hashFunc(key, capacity);
    HashNode* curr = buckets[idx];

    while (curr != nullptr) {
        if (curr->key == key) {
            curr->value = value;
            return;
        }
        curr = curr->next;
    }

    if ((double)count / capacity >= 0.7) {
        resize();
        idx = hashFunc(key, capacity);
    }

    HashNode* newNode = new HashNode(key, value, buckets[idx]);
    buckets[idx] = newNode;
    count++;
}

string HashTable::get(string key) const {
    int idx = hashFunc(key, capacity);
    HashNode* curr = buckets[idx];
    while (curr != nullptr) {
        if (curr->key == key) return curr->value;
        curr = curr->next;
    }
    return "";
}

string HashTable::remove(string key) {
    int idx = hashFunc(key, capacity);
    HashNode* curr = buckets[idx];
    HashNode* prev = nullptr;

    while (curr != nullptr) {
        if (curr->key == key) {
            if (prev == nullptr) buckets[idx] = curr->next;
            else prev->next = curr->next;

            string val = curr->value;
            delete curr;
            count--;
            return val;
        }
        prev = curr;
        curr = curr->next;
    }
    return "";
}

void HashTable::saveToFile(ofstream& fileStream, const string& name) const {
    for (int i = 0; i < capacity; i++) {
        HashNode* curr = buckets[i];
        while (curr != nullptr) {
            fileStream << "HASH " << name << " " << curr->key << " " << curr->value << endl;
            curr = curr->next;
        }
    }
}