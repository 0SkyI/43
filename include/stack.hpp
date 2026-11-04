#ifndef TSTACK_HPP
#define TSTACK_HPP

#include <cstddef>
#include <stdexcept>

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
    explicit TStack(size_t sz = 0)
        : mem(sz == 0 ? nullptr : new T[sz]), capacity(sz), count(0) {}

    TStack(const TStack& other)
        : mem(other.capacity == 0 ? nullptr : new T[other.capacity]),
          capacity(other.capacity), count(other.count) {
        copyElements(other);
    }

    ~TStack() {
        delete[] mem;
    }

    TStack& operator=(const TStack& other) {
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

    bool empty() const {
        return count == 0;
    }

    size_t size() const {
        return count;
    }

    size_t cap() const {
        return capacity;
    }

    void push(const T& value) {
        if (count == capacity) {
            throw std::overflow_error("stack overflow");
        }
        mem[count++] = value;
    }

    void pop() {
        if (count == 0) {
            throw std::underflow_error("stack is empty");
        }
        --count;
    }

    T& top() {
        if (count == 0) {
            throw std::underflow_error("stack is empty");
        }
        return mem[count - 1];
    }

    const T& top() const {
        if (count == 0) {
            throw std::underflow_error("stack is empty");
        }
        return mem[count - 1];
    }

private:
    void copyElements(const TStack& other) {
        if (mem == nullptr)
            return;
        for (size_t i = 0; i < other.count; ++i) {
            mem[i] = other.mem[i];
        }
    }
};

#endif // TSTACK_HPP
