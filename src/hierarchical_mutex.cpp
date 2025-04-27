#include <iostream>
#include <hierarchical_mutex.h>
#include <chrono> 
#include <thread>:w

HierarchicalMutex mutex_high(10);
HierarchicalMutex mutex_medium(5);
HierarchicalMutex mutex_low(3);

int slow_func1()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 10;
}

int slow_func2()
{
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    return 5;
}

int low_level_func()
{
    std::lock_guard<HierarchicalMutex> lock(mutex_low);
    return slow_func1();
}

int high_level_func()
{
    std::lock_guard<HierarchicalMutex> lock(mutex_high);

}



int main()
{

}