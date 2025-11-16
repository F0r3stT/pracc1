#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class QNode{
public:
    string value;
    QNode* next;
    QNode(string val) : value(val), next(nullptr) {}
};

class Queue{
private:
    QNode* frontNode;
    QNode* rearNode;

public:
    Queue();
    ~Queue();

    void push(string value);
    string pop();
    bool isEmpty() const;
    void saveFile(ofstream& fileStream, const string& name) const;
};

#endif
