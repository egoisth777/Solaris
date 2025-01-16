#include "threadsafebase.h"

ThreadSafeBase::ThreadSafeBase() :
    m_lock(),
    mutex(m_lock)
{}

ThreadSafeBase::~ThreadSafeBase()
{}


void ThreadSafeBase::lock(bool isLock)
{
    isLock ? m_lock.lock() : m_lock.unlock();
}
