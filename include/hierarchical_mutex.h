#pragma once

#include <mutex>
#include <stdexcept>
#include <climits>

class HierarchicalMutex
{
public:
    explicit HierarchicalMutex(unsigned long value):
        hierarchy_value(value),
        previous_hierarchy_value(0)
    {}

    void lock()
    {
        assert_hierarchy_consistency();
        internal_mutex.lock();
        update_hierarchy_value();
    }

    void unlock()
    {
        if (this_thread_hierarchy_value != hierarchy_value)
        {
            throw std::runtime_error("Mutex hierarchy violated");
        }

        this_thread_hierarchy_value = previous_hierarchy_value;
        internal_mutex.unlock();
    }

    bool try_lock()
    {
        assert_hierarchy_consistency();

        if (!internal_mutex.try_lock())
        {
            return false;
        }

        update_hierarchy_value();
        return true;
    }

private:
    std::mutex internal_mutex;
    const unsigned long hierarchy_value;
    unsigned long previous_hierarchy_value;
    static thread_local unsigned long this_thread_hierarchy_value;

    void assert_hierarchy_consistency()
    {
        if (this_thread_hierarchy_value <= hierarchy_value)
        {
            throw std::runtime_error("Exception in assertion. Hierarchy value is higher than this thread's hierarchy value.");
        }
    }

    void update_hierarchy_value()
    {
        previous_hierarchy_value = this_thread_hierarchy_value;
        this_thread_hierarchy_value = hierarchy_value;
    }
};

thread_local unsigned long HierarchicalMutex::this_thread_hierarchy_value(ULONG_MAX);