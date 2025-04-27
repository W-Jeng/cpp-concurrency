#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <stack>
#include <stdexcept>
#include <shared_mutex>

template<typename T>
class ThreadSafeStack
{
public:
    ThreadSafeStack(){}

    ThreadSafeStack(const ThreadSafeStack& other)
    {
        std::lock_guard<std::shared_mutex> lock(other.m);
        data = other.data;
    }

    ThreadSafeStack& operator=(const ThreadSafeStack&) = delete;

    ThreadSafeStack(ThreadSafeStack&& other) noexcept
    {
        std::lock_guard<std::shared_mutex> lock(other.m);
        data = std::move(other.data);
    }

    void push(T new_value)
    {
        std::lock_guard<std::shared_mutex> lock(m);
        data.push(std::move(new_value));
    }

    void pop()
    {
        std::lock_guard<std::shared_mutex> lock(m);

        if (data.empty())
        {
            return;
        }

        data.pop();
    }

    bool empty() const
    {
        std::shared_lock<std::shared_mutex> lock(m);
        return data.empty();
    }

    std::size_t size() const
    {
        std::shared_lock<std::shared_mutex> lock(m);
        return data.size();
    }

    T top() const
    {
        std::shared_lock<std::shared_mutex> lock(m);

        if (data.empty())
        {
            throw std::runtime_error("Stack data is empty, unable to return top()");
        }

        return data.top();
    }

private:
    std::stack<T> data;
    mutable std::shared_mutex m;
};





