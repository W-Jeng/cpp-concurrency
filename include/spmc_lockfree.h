#include <atomic>
#include <memory>
#include <iostream>

template<typename T>
class LockFreeQueue 
{
private:
    struct node;

    struct counted_node_ptr 
    {
        int external_count;
        node* ptr;
    };

    struct node_counter 
    {
        unsigned internal_count : 30;
        unsigned external_counters : 2;
    };

    struct node 
    {
        std::atomic<T*> data;
        std::atomic<node_counter> count;
        counted_node_ptr next;

        node() 
        {
            T* initial_data = nullptr; // Assumed from context, not explicitly in 7.16's node snippet
            data.store(initial_data); // Assumed from context
            node_counter new_count;
            new_count.internal_count = 0;
            new_count.external_counters = 2;
            count.store(new_count);
            next.ptr = nullptr;
            next.external_count = 0;
        }

        void release_ref() 
        {
            node_counter old_counter = count.load(std::memory_order_relaxed);
            node_counter new_counter;
            do 
            {
                new_counter = old_counter;
                --new_counter.internal_count;
            } 
            while (!count.compare_exchange_strong(
                old_counter, new_counter,
                std::memory_order_acquire, std::memory_order_relaxed));

            if (!new_counter.internal_count && !new_counter.external_counters) 
            {
                delete this;
            }
        }
    };

    std::atomic<counted_node_ptr> head;
    std::atomic<counted_node_ptr> tail;

    static void increase_external_count(
        std::atomic<counted_node_ptr>& counter,
        counted_node_ptr& old_counter) 
    {
        counted_node_ptr new_counter;
        do 
        {
            new_counter = old_counter;
            ++new_counter.external_count;
        } 
        while (!counter.compare_exchange_strong(
            old_counter, new_counter,
            std::memory_order_acquire, std::memory_order_relaxed));
        old_counter.external_count = new_counter.external_count;
    }

    static void free_external_counter(counted_node_ptr& old_node_ptr) 
    {
        node* const ptr = old_node_ptr.ptr;
        int const count_increase = old_node_ptr.external_count - 2;

        node_counter old_counter = ptr->count.load(std::memory_order_relaxed);
        node_counter new_counter;
        do 
        {
            new_counter = old_counter;
            --new_counter.external_counters;
            new_counter.internal_count += count_increase;
        } 
        while (!ptr->count.compare_exchange_strong(
            old_counter, new_counter,
            std::memory_order_acquire, std::memory_order_relaxed));

        if (!new_counter.internal_count && !new_counter.external_counters) 
        {
            delete ptr;
        }
    }

public:
    LockFreeQueue() 
    {
        counted_node_ptr cn_ptr;
        cn_ptr.ptr = new node();
        cn_ptr.external_count = 1; 
        head.store(cn_ptr);
        tail.store(cn_ptr);
    }

    ~LockFreeQueue() 
    {
        while (pop().get() != nullptr);
    }

    void push(T new_value) 
    {
        std::unique_ptr<T> new_data(new T(new_value));
        counted_node_ptr new_next;
        new_next.ptr = new node;
        new_next.external_count = 1;
        counted_node_ptr old_tail = tail.load();
        for (;;) 
        {
            std::cout << "Before Increase Tail external count: " << tail.load().external_count << "\n";
            increase_external_count(tail, old_tail);
            std::cout << "After Increase Tail external count: " << tail.load().external_count << "\n";

            T* old_data_ptr = nullptr; 
            if (old_tail.ptr->data.compare_exchange_strong(
                old_data_ptr, new_data.get())) 
            {
                old_tail.ptr->next = new_next;
                old_tail = tail.exchange(new_next);
                std::cout << "Old tail is exchanged, freeing external counter next\n";
                free_external_counter(old_tail);
                std::cout << "After free external c: " << tail.load().external_count << "\n";
                new_data.release();
                break;
            }
            std::cout << "Before release ref, external c: " << tail.load().external_count << "\n";
            old_tail.ptr->release_ref();
            std::cout << "After release ref, external c: " << tail.load().external_count << "\n";
        }
    }

    std::unique_ptr<T> pop() 
    {
        counted_node_ptr old_head = head.load(std::memory_order_relaxed);
        for (;;) 
        {
            increase_external_count(head, old_head);
            node* const ptr = old_head.ptr;
            if (ptr == tail.load().ptr) 
            {
                ptr->release_ref();
                return std::unique_ptr<T>();
            }
            if (head.compare_exchange_strong(old_head, ptr->next)) 
            {
                T* const res = ptr->data.exchange(nullptr);
                free_external_counter(old_head);
                return std::unique_ptr<T>(res);
            }
            ptr->release_ref();
        }
    }
};