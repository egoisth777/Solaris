#ifndef THREADSAFESET_H
#define THREADSAFESET_H

#include "threadsafebase.h"

#include <iterator>
#include <mutex>
#include <unordered_set>


/// A wrapper of std::unordered_set
/// With all publiic functions thread-safe
template<typename T>
class ThreadSafeSet : public ThreadSafeBase
{
private:
    std::unordered_set<T> m_set;

public:
    /// The reference for the underlying set
    /// Caution: this is NOT thread-safe
    /// you should lock before get
    std::unordered_set<T>& getUnsafe;

public:
    ThreadSafeSet();

    bool insert(T&&);

    bool insert(const T& value);

    bool erase(const T& value);

    bool contains(const T& value);

    bool empty();

    size_t size();

    void clear();
};


template<typename T>
ThreadSafeSet<T>::ThreadSafeSet() :
    ThreadSafeBase(),
    m_set(),
    getUnsafe(m_set)
{}

template<typename T>
bool ThreadSafeSet<T>::insert(T &&val)
{
    std::lock_guard<std::mutex> lock(m_lock);
    m_set.insert(val);
    return m_set.insert(val).second;
}

template<typename T>
bool ThreadSafeSet<T>::insert(const T &val)
{
    std::lock_guard<std::mutex> lock(m_lock);
    return m_set.insert(val).second;
}

template<typename T>
bool ThreadSafeSet<T>::erase(const T &value)
{
    std::lock_guard<std::mutex> lock(m_lock);
    return m_set.erase(value) > 0;
}

template<typename T>
bool ThreadSafeSet<T>::contains(const T &value)
{
    std::lock_guard<std::mutex> lock(m_lock);
    return m_set.find(value) != m_set.end();
}

template<typename T>
size_t ThreadSafeSet<T>::size()
{
    std::lock_guard<std::mutex> lock(m_lock);
    return m_set.size();
}

template<typename T>
void ThreadSafeSet<T>::clear()
{
    std::lock_guard<std::mutex> lock(m_lock);
    m_set.clear();
}

template<typename T>
bool ThreadSafeSet<T>::empty()
{
    std::lock_guard<std::mutex> lock(m_lock);
    return m_set.empty();
}


#endif // THREADSAFESET_H
