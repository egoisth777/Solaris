#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include "threadsafebase.h"
#include <queue>

// A wrapper for thread-safe std::queue
template<typename T>
class ThreadSafeQueue : public ThreadSafeBase
{
private:
    std::queue<T> m_queue;

public:
    /// The reference for the underlying set
    /// Caution: this is NOT thread-safe
    /// you should lock before get
    std::queue<T>& getUnsafe;

public:
    ThreadSafeQueue() :
        ThreadSafeBase(),
        m_queue(),
        getUnsafe(m_queue){};

    // Add an element to the queue
    void push(const T& value) {
        std::lock_guard<std::mutex> lock(m_lock);
        m_queue.push(value);
    }

    // Get the "front" element
    T front() {
        std::lock_guard<std::mutex> lock(m_lock);
        return m_queue.front();
    }

    void pop(){
        std::lock_guard<std::mutex> lock(m_lock);
        m_queue.pop();
    }

    size_t size(){
        std::lock_guard<std::mutex> lock(m_lock);
        return m_queue.size();
    }

    bool empty(){
        std::lock_guard<std::mutex> lock(m_lock);
        return m_queue.empty();
    }
};

#endif // THREADSAFEQUEUE_H
