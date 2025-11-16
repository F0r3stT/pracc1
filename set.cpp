#include "set.h"

int Set::hashFunc(const string& key, int cap) const
{
    int hash=0;
    for(char ch : key)
    {
        hash = (hash*31 + abs((int)ch)) % cap;
    }
    return hash;
}

Set::Set(int size)
{
    this->capac = size;
    this->count = 0;
    this->buckets = new SetNode*[size];
    for(int i=0; i<size; i++)
    {
        this->buckets[i]=nullptr;
    }
}

Set::~Set()
{
    for(int i=0; i< capac; i++)
    {
        SetNode* curr = buckets[i];
        while(curr != nullptr)
        {
            SetNode* next = curr->next;
            delete curr;
            curr=next;
        }
    }
    delete[] buckets;
}


void Set::resize() {
    int newCapacity = capac * 2;

    // 1. Создаем новый массив
    SetNode** newBuckets = new SetNode*[newCapacity];
    for (int i = 0; i < newCapacity; i++) {
        newBuckets[i] = nullptr;
    }

    for (int i = 0; i < capac; i++) {
        SetNode* curr = buckets[i];
        while (curr != nullptr) {
            SetNode* next = curr->next; 

            int newIdx = hashFunc(curr->key, newCapacity);
            
            // Вставляем в голову списка нового бакета
            curr->next = newBuckets[newIdx];
            newBuckets[newIdx] = curr;

            curr = next;
        }
    }

    // 3. Удаляем старый массив (сами узлы не удаляем, мы их переиспользовали)
    delete[] buckets;

    // 4. Обновляем поля класса
    buckets = newBuckets;
    capac = newCapacity;
}

void Set::add(string key) {
    if (isMember(key)) return;

    if ((double)count / capac >= 0.7) {
        resize();
    }

    int idx = hashFunc(key, capac);
    SetNode* newNode = new SetNode(key, buckets[idx]);
    buckets[idx] = newNode;
    count++;
}

string Set::remove(string key) {
    int idx = hashFunc(key, capac);
    SetNode* curr = buckets[idx];
    SetNode* prev = nullptr;

    while (curr != nullptr) {
        if (curr->key == key) {
            if (prev == nullptr) buckets[idx] = curr->next;
            else prev->next = curr->next;

            string val = curr->key;
            delete curr;
            count--;
            return val;
        }
        prev = curr;
        curr = curr->next;
    }
    return "";
}

bool Set::isMember(string key) const {
    int idx = hashFunc(key, capac);
    SetNode* curr = buckets[idx];
    while (curr != nullptr) {
        if (curr->key == key) return true;
        curr = curr->next;
    }
    return false;
}

void Set::saveFile(ofstream& fileStream, const string& name) const {
    for (int i = 0; i < capac; i++) {
        SetNode* curr = buckets[i];
        while (curr != nullptr) {
            fileStream << "SET " << name << " " << curr->key << endl;
            curr = curr->next;
        }
    }
}