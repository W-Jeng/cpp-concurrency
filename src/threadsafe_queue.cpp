#include <threadsafe_queue.h>
#include <iostream>

int main()
{
    std::cout << "hello world!\n";
    ThreadsafeQueue<int> q;
    q.push(2);
    return 0;
}