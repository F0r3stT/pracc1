#include <iostream>
#include <string>
#include "database.h"

using namespace std;

// Вывод справки по использованию
void printUsage() {
    cout << "Использование: ./dbms --file <файл> --query '<запрос>'" << endl;
    cout << "Примеры:" << endl;
    cout << "  --query 'SADD myset value'" << endl;
    cout << "  --query 'SPUSH mystack item'" << endl;
    cout << "  --query 'HSET myhash key val'" << endl;
}

// Точка входа
int main(int argc, char* argv[]) {
    string filename;
    string query;

    // Простой цикл для разбора аргументов командной строки
    // Ищем ключи --file и --query
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "--file" && i + 1 < argc) {
            filename = argv[++i];
        } 
        else if (arg == "--query" && i + 1 < argc) {
            query = argv[++i];
        }
        else if (arg == "--help") {
            printUsage();
            return 0;
        }
    }

    // Проверка обязательных параметров
    if (filename.empty() || query.empty()) {
        cout << "ОШИБКА: Не указаны обязательные параметры (--file и --query)." << endl;
        printUsage();
        return 1;
    }

    // Создание объекта базы данных и выполнение запроса
    // Используем стековую память для объекта db
    Database db(filename);
    
    string result = db.executeQuery(query);
    
    // Вывод результата в стандартный поток
    cout << result << endl;

    return 0;
}