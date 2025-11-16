#ifndef SET_H
#define SET_H

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>

using namespace std;

class SetNode{
public:
    string key;
    SetNode* next;

    SetNode(string k, SetNode* nxt = nullptr) : key(k), next(nxt){}
};

class Set
{
private:
    SetNode** buckets;
    int capac;
    int count;

    int hashFunc(const string& key, int cap) const;

    void resize();

public:
    Set(int size=10);
    ~Set();

    void add(string key);
    string remove(string key);
    bool isMember(string key) const;

    void saveFile(ofstream& fstream, const string& name) const;
};

#endif