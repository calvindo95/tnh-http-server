#ifndef TSQUEUE_H
#define TSQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <map>

// Thread Safe Queue
template <typename T>
class TSQueue{
    private:
        std::queue<T> m_queue;
        std::mutex m_mutex;
        std::condition_variable m_cond;

    public:
        TSQueue(){};

        void push(T item){
        // Lock the queue
        std::unique_lock<std::mutex> lock(m_mutex);

        m_queue.push(item);
        m_cond.notify_one();
        };

        T pop(){
            // Lock the queue
            std::unique_lock<std::mutex> lock(m_mutex);

            // Wait until queue is not empty
            m_cond.wait(lock, [this]{return !m_queue.empty();});

            // Get item from queue and pop
            T item = m_queue.front();
            m_queue.pop();

            return item;
        };

        int size(){
            // Lock the queue
            std::unique_lock<std::mutex> lock(m_mutex);

            return m_queue.size();
        };
};

#endif