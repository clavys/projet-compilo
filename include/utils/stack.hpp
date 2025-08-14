#ifndef STACK_HPP
#define STACK_HPP

#include <vector>
#include <stdexcept>

template <typename T>
class Stack {
private:
    std::vector<T> elements;

public:
    void push(const T& value) {
        elements.push_back(value);
    }

    void pop() {
        if (!elements.empty()) {
            elements.pop_back();
        } else {
            throw std::runtime_error("Stack is empty. Cannot pop.");
        }
    }

    T top() const {
        if (!elements.empty()) {
            return elements.back();
        } else {
            throw std::runtime_error("Stack is empty. Cannot access top.");
        }
    }

    bool isEmpty() const {
        return elements.empty();
    }

    size_t size() const {
        return elements.size();
    }

    void clear() {
        elements.clear();
    }
};

#endif // STACK_HPP
