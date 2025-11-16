#include "queue.h"

Queue::Queue() {
    this->frontNode = nullptr;
    this->rearNode = nullptr;
}

Queue::~Queue() {
    while (!isEmpty()) {
        pop();
    }
}

void Queue::push(string value) {
    QNode* newNode = new QNode(value);
    if (isEmpty()) {
        this->frontNode = newNode;
        this->rearNode = newNode;
    } 
    else {
        this->rearNode->next = newNode;
        this->rearNode = newNode;
    }
}

string Queue::pop() {
    if (isEmpty()) 
        return "QUEUE IS EMPTY";

    QNode* temp = this->frontNode;
    string val = temp->value;

    this->frontNode = this->frontNode->next;
    if (this->frontNode == nullptr) {
        this->rearNode = nullptr;
    }
    delete temp;
    return val;
}

bool Queue::isEmpty() const {
    return this->frontNode == nullptr;
}

// Сохраняем: QUEUE <NAME> <VALUE>
void Queue::saveFile(ofstream& fileStream, const string& name) const {
    QNode* current = this->frontNode;
    while (current != nullptr) {
        fileStream << "QUEUE " << name << " " << current->value << endl;
        current = current->next;
    }
}