#include <iostream>
#include <TSQueue.h>


TSQueue::TSQueue(){}

void TSQueue::push(std::map<std::string,std::string> item){
    // Lock the queue
    std::unique_lock<std::mutex> lock(m_mutex);

    m_queue.push(item);
    m_cond.notify_one();
}

std::map<std::string,std::string> TSQueue::pop(){
    // Lock the queue
    std::unique_lock<std::mutex> lock(m_mutex);

    // Wait until queue is not empty
    m_cond.wait(lock, [this]{
        return !m_queue.empty();
    });

    // Get item from queue and pop
    std::map<std::string,std::string> item = m_queue.front();
    m_queue.pop();

    return item;
}

int TSQueue::size(){
    // Lock the queue
    std::unique_lock<std::mutex> lock(m_mutex);

    return m_queue.size();
}

void TSQueue::print(){
    std::cout << sizeof(std::map<std::string,std::string>) << std::endl;
}