#ifndef TSTACK_HPP
#define TSTACK_HPP

#include <cstddef>
#include <stdexcept>
#include <iostream>   // нужен демо-примеру run_demo() в src/main.cpp
#include <string>     // используется тестами (TStack<std::string>)

using std::size_t;

// Стек с фиксированной вместимостью (capacity).
// При переполнении push() бросает std::overflow_error,
// при обращении к пустому стеку pop()/top() бросают std::underflow_error.
//
// В заголовочном файле находится только определение класса;
// вся реализация методов вынесена в src/main.cpp.
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

// Подключение реализации шаблонных методов.
// Тела шаблонных методов должны быть видны компилятору в точке
// инстанцирования, поэтому реализация из src/main.cpp подключается
// через #include сразу после определения класса.
#include "../src/main.cpp"

#endif // TSTACK_HPP
