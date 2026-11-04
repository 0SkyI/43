// Шаблонный стек фиксированной вместимости TStack<T> + демонстрационный пример.
//
// Всё в одном файле: определение класса, реализация методов и main().
// Юнит-тесты Google Test (tests/stack_test.cpp) подключают этот файл
// через #include и проверяют каждый метод стека.

#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <string>

using std::size_t;

// Стек с фиксированной вместимостью (capacity).
// При переполнении push() бросает std::overflow_error,
// при обращении к пустому стеку pop()/top() бросают std::underflow_error.
template <typename T>
class TStack {
private:
    T* mem;
    size_t capacity;
    size_t count;

public:
    explicit TStack(size_t sz = 0);

    TStack(const TStack& other);

    ~TStack();

    TStack& operator=(const TStack& other);

    bool empty() const;

    size_t size() const;

    size_t cap() const;

    void push(const T& value);

    void pop();

    T& top();

    const T& top() const;

private:
    void copyElements(const TStack& other);
};

// ==================== Реализация TStack<T> ====================

template <typename T>
TStack<T>::TStack(size_t sz)
    : mem(sz == 0 ? nullptr : new T[sz]), capacity(sz), count(0) {}

template <typename T>
TStack<T>::TStack(const TStack& other)
    : mem(other.capacity == 0 ? nullptr : new T[other.capacity]),
      capacity(other.capacity), count(other.count) {
    copyElements(other);
}

template <typename T>
TStack<T>::~TStack() {
    delete[] mem;
}

template <typename T>
TStack<T>& TStack<T>::operator=(const TStack& other) {
    if (this == &other)
        return *this;

    T* tmp = other.capacity == 0 ? nullptr : new T[other.capacity];
    if (tmp != nullptr) {
        for (size_t i = 0; i < other.count; ++i) {
            tmp[i] = other.mem[i];
        }
    }

    delete[] mem;
    mem = tmp;
    capacity = other.capacity;
    count = other.count;

    return *this;
}

template <typename T>
bool TStack<T>::empty() const {
    return count == 0;
}

template <typename T>
size_t TStack<T>::size() const {
    return count;
}

template <typename T>
size_t TStack<T>::cap() const {
    return capacity;
}

template <typename T>
void TStack<T>::push(const T& value) {
    if (count == capacity) {
        throw std::overflow_error("stack overflow");
    }
    mem[count++] = value;
}

template <typename T>
void TStack<T>::pop() {
    if (count == 0) {
        throw std::underflow_error("stack is empty");
    }
    --count;
}

template <typename T>
T& TStack<T>::top() {
    if (count == 0) {
        throw std::underflow_error("stack is empty");
    }
    return mem[count - 1];
}

template <typename T>
const T& TStack<T>::top() const {
    if (count == 0) {
        throw std::underflow_error("stack is empty");
    }
    return mem[count - 1];
}

template <typename T>
void TStack<T>::copyElements(const TStack& other) {
    if (mem == nullptr)
        return;
    for (size_t i = 0; i < other.count; ++i) {
        mem[i] = other.mem[i];
    }
}

// ==================== Демонстрационный пример ====================

// Точка входа приложения stack_app.
// В тестовой сборке (tests/stack_test.cpp) определение main() подавляется
// макросом STACK_SKIP_MAIN — там собственная точка входа Google Test.
#ifndef STACK_SKIP_MAIN
int main() {
    int steps = 0;
    try {
        // Создаём стек вместимостью 5 элементов
        TStack<int> s(5);

        // Добавляем элементы
        s.push(10);
        s.push(20);
        s.push(30);
        ++steps;

        // Проверяем размер и верхний элемент
        std::cout << "Size: " << s.size() << '\n';
        std::cout << "Top: " << s.top() << '\n';

        // Удаляем верхний элемент
        s.pop();
        ++steps;
        std::cout << "Top after pop: " << s.top() << '\n';

        // Проверяем конструктор копирования
        TStack<int> s2 = s;
        ++steps;
        std::cout << "s2 top: " << s2.top() << '\n';

        // Изменяем s2, проверяем, что s не изменился
        s2.push(99);
        ++steps;
        std::cout << "s top: " << s.top() << '\n';
        std::cout << "s2 top: " << s2.top() << '\n';

        // Проверяем оператор присваивания
        TStack<int> s3(2);
        s3 = s;
        ++steps;
        std::cout << "s3 size: " << s3.size() << '\n';

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        return -1;
    }
    std::cout << "Demo finished, steps: " << steps << '\n';
    return 0;
}

#endif // STACK_SKIP_MAIN
