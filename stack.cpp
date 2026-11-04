#include <cstddef>
#include <algorithm>
#include <stdexcept>
#include <iostream>

using std::copy;
using std::underflow_error;
using std::size_t;
using std::cout;

template <typename T>
class TStack {
private:
    T* mem;
    size_t capacity;
    size_t count;

public:
    explicit TStack(size_t sz = 0): mem(sz == 0 ? nullptr : new T[sz]), capacity(sz), count(0) {}

    TStack(const TStack& other): mem(other.capacity == 0 ? nullptr : new T[other.capacity]), capacity(other.capacity), count(other.count) {
        copy(other.mem, other.mem + other.count, mem);
    }

    ~TStack() {
        delete[] mem;
    }

    TStack& operator=(const TStack& other) {
        if (this == &other)
            return *this;
        
        T* tmp = new T[other.capacity];
        copy(other.mem, other.mem + other.count, tmp);
        
        delete[] mem;
        mem = tmp;
        capacity = other.capacity;
        count = other.count;
        
        return *this;
    }

    bool empty() const {
        return count == 0;
    }

    size_t size() const {
        return count;
    }

    void push(const T& value) {
        if (count == capacity) {
            throw std::overflow_error("stack overflow");
        }
        mem[count++] = value;
    }

    void pop() {
        if (count == 0) {
            throw underflow_error("stack is empty");
        }
        --count;
    }

    T& top() {
        if (count == 0) {
            throw underflow_error("stack is empty");
        }
        return mem[count - 1];
    }

    const T& top() const {
        if (count == 0) {
            throw underflow_error("stack is empty");
        }
        return mem[count - 1];
    }
};

int main() {
    try {
        // Создаём стек вместимостью 5 элементов
        TStack<int> s(5);

        // Добавляем элементы
        s.push(10);
        s.push(20);
        s.push(30);

        // Проверяем размер и верхний элемент
        cout << "Size: " << s.size() << '\n';         // 3
        cout << "Top: " << s.top() << '\n';           // 30

        // Удаляем верхний элемент
        s.pop();
        cout << "Top after pop: " << s.top() << '\n'; // 20

        // Проверяем конструктор копирования
        TStack<int> s2 = s;
        cout << "s2 top: " << s2.top() << '\n';       // 20

        // Изменяем s2, проверяем, что s не изменился
        s2.push(99);
        cout << "s top: " << s.top() << '\n';         // 20 (не изменился!)
        cout << "s2 top: " << s2.top() << '\n';       // 99

        // Проверяем оператор присваивания
        TStack<int> s3(2);
        s3 = s;
        cout << "s3 size: " << s3.size() << '\n';     // 2

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
    }
}