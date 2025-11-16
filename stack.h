#ifndef STACK_H
#define STACK_H

#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class StackNode{
public:
    string value;
    StackNode* next;

    StackNode(string val, StackNode* nxt = nullptr): value(val), next(nxt){}
};

class Stack{
private:
    StackNode* topNode;

public:
    Stack();
    ~Stack();

    void push(string value);
    string pop();
    string peek() const;
    bool isEmpty() const;

    void saveFile(ofstream& fileStream, const string& name) const;
};

#endif