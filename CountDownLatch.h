#pragma once
#include"Condition.h"
#include"MutexLock.h"


class CountDownLatch
{
    public:
    explicit CountDownLatch(int count):
    _mutex(),_condition(_mutex),_count(count)
    {}
    void wait()
    {
        MutexLockGuard lock(_mutex);
        while(_count>0)_condition.wait();
    }
    void countDown()
    {
        MutexLockGuard lock(_mutex);
        --_count;
        if(_count==0)_condition.notifyAll();
    }
    private:
    mutable MutexLock _mutex;
    Condition _condition;
    int _count;
};
