#ifndef TSQUEUE_H
#define TSQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <map>

// Thread Safe Queue
class TSQueue{
    private:
        std::queue<std::map<std::string,std::string>> m_queue;
        std::mutex m_mutex;
        std::condition_variable m_cond;

    public:
        TSQueue();

        void push(std::map<std::string,std::string> item);
        std::map<std::string,std::string> pop();

        int size();
        void print();
};

#endif