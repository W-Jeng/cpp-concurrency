#include <iostream>
#include <hierarchical_mutex.h>
#include <chrono> 
#include <thread>

HierarchicalMutex mutex_high(10);
HierarchicalMutex mutex_low(3);

void test_basic1()
{
    std::lock_guard<HierarchicalMutex> lck(mutex_high);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void test_basic2()
{
    std::lock_guard<HierarchicalMutex> lck(mutex_low);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void test_lock_high_to_low()
{
    std::lock_guard<HierarchicalMutex> lck(mutex_high);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::lock_guard<HierarchicalMutex> lock(mutex_low);
}

void test_lock_low_to_high()
{
    try
    {
        std::lock_guard<HierarchicalMutex> lck(mutex_low);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::lock_guard<HierarchicalMutex> lock(mutex_high);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}


int main()
{
    std::thread t1(test_basic1);
    t1.join();

    std::thread t2(test_basic2);
    t2.join();

    std::thread t3(test_lock_high_to_low);
    t3.join();

    std::thread t4(test_lock_low_to_high);
    t4.join();
}