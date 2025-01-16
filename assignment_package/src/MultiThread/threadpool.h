#ifndef THREADPOOL_H
#define THREADPOOL_H

// Reference: https://github.com/mtrebi/thread-pool/blob/master/include/ThreadPool.h

#include "threadsafequeue.h"
#include <functional>
#include <thread>
#include <iostream>
#include <optional>



class ThreadPool : public ThreadSafeBase
{
public:
    int m_workerCount;
    ThreadSafeQueue<std::function<void()>> m_queue;
    std::vector<std::thread> m_workers;

public:
    ThreadPool(int workerCount);

    /// Enqueue a task to the tread-pool.
    /// The task should be a callable return [void].
    /// Same interface as std::thread.
    template <typename F, typename... Args>
    void thread(F &&f, Args &&...args)
    {
        std::function<void()> func =
            std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        m_queue.push(func);
    }

private:
    /// Worker's job function, repeatedly ask pool for task
    static void doThread(ThreadPool* pool, int id);

    /// Pop a task from the pool
    /// When there is no task in the pool, just return {}
    static void getTask(ThreadPool* pool, std::optional<std::function<void()>>&);
};


#endif // THREADPOOL_H
