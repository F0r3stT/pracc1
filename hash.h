#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;

class HashNode {
public:
    string key;
    string value;
    HashNode* next;

    HashNode(string k, string v, HashNode* nxt = nullptr) 
        : key(k), value(v), next(nxt) {}
};

class HashTable {
private:
    HashNode** buckets;
    int capacity;
    int count;

    int hashFunc(const string& key, int cap) const;
    
    void resize();

public:
    HashTable(int size = 10);
    ~HashTable();

    void set(string key, string value);
    string get(string key) const;
    string remove(string key);
    
    void saveToFile(ofstream& fileStream, const string& name) const;
};

#endif