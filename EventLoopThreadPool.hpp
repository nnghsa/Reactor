#pragma once
#include"EventLoop.h"
#include<iostream>
#include"EventLoopThread.h"
#include<memory>
#include<assert.h>

class EventLoopThreadPool
{

public:
    EventLoopThreadPool(EventLoop* loop, int numThreads);
    ~EventLoopThreadPool();

    void start();
    EventLoop* getNextLoop();
private:
    EventLoop* _baseLoop;
    bool _started;
    int numThreads;
    int _next;
    std::vector<std::shared_ptr<EventLoopThread>>_threads;
    std::vector<EventLoop*>_loops;
};

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop, int numThreads) :
    _baseLoop(loop),
    _started(false),
    numThreads(numThreads),
    _next(0)
{
    if (numThreads <= 0)
    {
        std::cout << "numTreads = " << 0 << std::endl;
        abort();
    }
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    std::cout << "~EventLoopThreadPool()" << std::endl;
}
void EventLoopThreadPool::start()
{
    _baseLoop->assertInLoopThread();
    _started = true;
    for (int i = 0;i < numThreads;++i)
    {
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        _threads.push_back(t);
        _loops.push_back(t->startLoop());
    }
}
EventLoop* EventLoopThreadPool::getNextLoop()
{
    _baseLoop->assertInLoopThread();
    assert(_started);
    EventLoop* loop = _baseLoop;
    if (!_loops.empty())
    {
        loop = _loops[_next];
        _next = (_next + 1) % numThreads;
    }
    return loop;

}