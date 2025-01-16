#ifndef THREADSAFEBASE_H
#define THREADSAFEBASE_H

#include <mutex>

class ThreadSafeBase
{
protected:
    std::mutex m_lock;

public:
    std::mutex& mutex;

public:
    ThreadSafeBase();
    virtual ~ThreadSafeBase();

    /// Manually lock or unlock this container
    void lock(bool);
};

#endif // THREADSAFEBASE_H
