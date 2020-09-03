#include"EventLoopThread.h"
#include<functional>
#include"EventLoop.h"
#include<assert.h>

EventLoopThread::EventLoopThread():
    _loop(NULL),
    _exiting(false),
    _thread(std::bind(&EventLoopThread::threadFunc,this)),
    _mutex(),
    _cond(_mutex)
{
}
EventLoopThread::~EventLoopThread()
{
    _exiting=true;
    if(_loop!=NULL)
    {
        _loop->quit();
        _thread.join();
    }
}
EventLoop* EventLoopThread::startLoop(){
    {
        MutexLockGuard lock(_mutex);
        while(_loop==NULL)_cond.wait();
    }
    return _loop;
}
void EventLoopThread::threadFunc()
{
    EventLoop loop;
    {
        MutexLockGuard lock(_mutex);
        _loop=&loop;
        _cond.notify();
    }
    loop.loop();
    _loop=NULL;
}