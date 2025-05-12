// queue implementation using nodes
#include <memory>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadsafeQueue
{
public:
    ThreadsafeQueue():
        head(new node), tail(head.get())
    {}

    ThreadsafeQueue(const ThreadsafeQueue& other)=delete; //copy consstructor
    ThreadsafeQueue& operator=(const ThreadsafeQueue& other)=delete; //copy assignment
    std::shared_ptr<T> try_pop();
    bool try_pop(T& value);
    std::shared_ptr<T> wait_and_pop();
    void wait_and_pop(T& value);
    void push(T new_value);
    bool empty();

private:
    struct node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };

    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node* tail;
    std::condition_variable data_cond;
};

template<typename T>
void ThreadsafeQueue<T>::push(T new_value)
{
    std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
    std::unique_ptr<node> p(new node);

    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        tail -> data = new_data;
        node* new_tail = p.get();
        tail -> next = std::move(p);
        tail = new_tail;
    }

    data_cond.notify_one();
}