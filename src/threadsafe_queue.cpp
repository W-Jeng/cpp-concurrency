#include <threadsafe_queue.h>
#include <iostream>

int main()
{
    std::cout << "hello world!\n";
    ThreadsafeQueue<int> q;
    q.push(2);
    std::shared_ptr<int> temp = q.try_pop();
    std::cout << "value: " << *temp << "\n";
    return 0;
}