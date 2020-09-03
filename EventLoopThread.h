#pragma once
#include<thread>
#include"MutexLock.h"
#include"Condition.h"
class EventLoop;
class EventLoopThread
{
    public:
    EventLoopThread();
    ~EventLoopThread();
    EventLoop*startLoop();
private:
    void threadFunc();
    EventLoop*_loop;
    bool _exiting;
    std::thread _thread;
    MutexLock _mutex;
    Condition _cond;
    
};