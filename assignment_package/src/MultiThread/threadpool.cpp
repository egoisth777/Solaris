#include "threadpool.h"

ThreadPool::ThreadPool(int workerCount) :
    ThreadSafeBase(),
    m_workerCount(workerCount),
    m_workers()
{

    /// Emplace enough worker threads
    m_workers.reserve(workerCount);
    for(int i = 0; i < workerCount; ++i){
        m_workers.push_back(std::thread(&doThread, this, i));
    }
    std::cout << "Pool start with " << workerCount << " threads" << std::endl;
}



void ThreadPool::doThread(ThreadPool *pool, int id){
    if(pool == nullptr){
        throw std::invalid_argument("Invalide pool");
    }

    std::optional<std::function<void()>> myTask;
    while(1){
        ThreadPool::getTask(pool, myTask);
        if(myTask.has_value()){
            myTask.value()();   // Call the task function
        }
    }
}

void ThreadPool::getTask(ThreadPool *pool,
                         std::optional<std::function<void ()> > & task)
{
    /// Lock until get the task
    std::lock_guard<std::mutex> lock(pool->m_queue.mutex);

    if(pool->m_queue.getUnsafe.empty()){
        task = {};
        return;
    }

    std::function<void()> nextTask = pool->m_queue.getUnsafe.front();
    pool->m_queue.getUnsafe.pop();
    task = nextTask;
}

