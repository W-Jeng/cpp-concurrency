#include <iostream>
#include <threadsafe_stack.h>
#include <cassert>
#include <thread>
#include <vector>

void single_threaded_test()
{
    ThreadSafeStack<int> stk;
    stk.push(1);
    stk.push(2);
    stk.push(3);
    assert(stk.size() == 3);
    stk.pop();
    assert(stk.size() == 2);
    assert(stk.top() == 2);
}

void multithreaded_test()
{
    ThreadSafeStack<int> stk2;
    constexpr int num_threads = 10;
    constexpr int num_pushes = 10000;

    auto push_data = [&stk2, num_pushes](int id)
    {
        for (int i = 0; i < num_pushes; ++i)
        {
            stk2.push(id * num_pushes + i);
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i)
    {
        threads.emplace_back(push_data, i);
    }

    for (auto& t: threads)
    {
        t.join();
    }

    assert(!stk2.empty());
    assert(stk2.size() == num_threads*num_pushes);
}

void edge_case_test()
{
    ThreadSafeStack<int> stk3;

    try
    {
        int top_val = stk3.top();
    }
    catch(const std::exception& e)
    {
        std::cout << "Error successfully caught\n";
        std::cerr << e.what() << "\n";
    }
}

int main()
{
    single_threaded_test();
    multithreaded_test();
    edge_case_test();
}

