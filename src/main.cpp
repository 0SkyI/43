#include "stack.hpp"

#include <iostream>

int main() {
    try {
        // Создаём стек вместимостью 5 элементов
        TStack<int> s(5);

        // Добавляем элементы
        s.push(10);
        s.push(20);
        s.push(30);

        // Проверяем размер и верхний элемент
        std::cout << "Size: " << s.size() << '\n';         // 3
        std::cout << "Top: " << s.top() << '\n';           // 30

        // Удаляем верхний элемент
        s.pop();
        std::cout << "Top after pop: " << s.top() << '\n'; // 20

        // Проверяем конструктор копирования
        TStack<int> s2 = s;
        std::cout << "s2 top: " << s2.top() << '\n';       // 20

        // Изменяем s2, проверяем, что s не изменился
        s2.push(99);
        std::cout << "s top: " << s.top() << '\n';         // 20 (не изменился!)
        std::cout << "s2 top: " << s2.top() << '\n';       // 99

        // Проверяем оператор присваивания
        TStack<int> s3(2);
        s3 = s;
        std::cout << "s3 size: " << s3.size() << '\n';     // 2

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
    }
    return 0;
}
