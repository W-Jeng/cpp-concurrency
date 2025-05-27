#include <iostream>
#include <spmc_lockfree.h>

int main()
{
    LockFreeQueue<int> lfq;
    std::cout << "Before adding value\n";
    lfq.push(1);
    std::cout << "added one value: " << 1 << "\n";
    std::unique_ptr<int> x = lfq.pop();
    std::cout << "popped val: " << *x << "\n";
    return 0;
}