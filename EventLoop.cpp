#include<iostream>

#include"EventLoop.h"
#include"CurrentThreadId.h"
#include<assert.h>

#include<sys/eventfd.h>
#include<memory>
#include<vector>
__thread EventLoop* _loopInThisThread = 0;

int createEventFd()
{
    int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        std::cout << "eventfd() error" << std::endl;
        abort();
    }
    return evtfd;
}

EventLoop::EventLoop()
    :p_epoller(new Epoller(this)),
    _looping(false),
    _quit(false),
    _threadId(CurrentThreadId::tid()),
    _eventHandling(false),
    _callingPendingFunctors(false),
    _wakeupFd(createEventFd()),
    sp_wakeupEventHandler(new EventHandler(this,_wakeupFd))
{
    if (_loopInThisThread)
    {
        std::cout << "Anthor EventLoop is exists" << std::endl;
       abort();
    }
    std::cout<<_threadId<<std::endl;
    
    _loopInThisThread = this;
    sp_wakeupEventHandler->setEvents(EPOLLIN|EPOLLET);
    sp_wakeupEventHandler->setReadCallBack(std::bind(&EventLoop::handleRead,this));
    sp_wakeupEventHandler->setConnHandler(std::bind(&EventLoop::handleConn,this));
    p_epoller->epoll_add(sp_wakeupEventHandler,0);
}

EventLoop::~EventLoop()
{
    assert(!_looping);
    _loopInThisThread = NULL;
}
void EventLoop::loop()
{
    assert(!_looping);
    assertInLoopThread();
    _looping = true;
    _quit = false;
    std::vector<std::shared_ptr<EventHandler>>request;
    printf("EventLoop %s start looping\n", this);

    while (!_quit)
    {
        request.clear();
        request = p_epoller->Epoll();
        _eventHandling = true;
        for (auto& it : request)it->handleEvent();
        _eventHandling=false;
        doPendingFunctors();
        //p_epoller->handleExpired();
    }
    printf("EventLoop stop");
    _looping = false;


}

void EventLoop::quit()
{
    _quit = true;
    if (!isInLoopThread())
    {
        wakeup();
    }
}
void EventLoop::doPendingFunctors()
{
    std::vector<Functor>functors;
    _callingPendingFunctors = true;
    {
        MutexLockGuard lock(_mutex);
        functors.swap(_pendingFunctors);
    }
    for (size_t i = 0;i < functors.size();++i) functors[i]();
    _callingPendingFunctors = false;
}
void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = write(_wakeupFd, (char*)&(one), sizeof one);
    printf("wakeup\n");
    // if(n!=sizeof one)
    // {

    // }
}
void EventLoop::queueInLoop(Functor&& cb)
{
    {
        MutexLockGuard lock(_mutex);
        _pendingFunctors.emplace_back(std::move(cb));
    }
    if (!isInLoopThread() || _callingPendingFunctors)wakeup();

}
void EventLoop::handleRead()
{
    uint64_t one =1;
    ssize_t n=read(_wakeupFd,&one,sizeof one);
    printf("handleRead()\n");
    if(n!=sizeof one)
    {
        printf("EventLoop::handleRead() reads %d bytes instead of 8",n);
    }
}
void EventLoop::handleConn()
{
    updateEpoller(sp_wakeupEventHandler,0);
}