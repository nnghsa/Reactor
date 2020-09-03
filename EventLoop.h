#ifndef _MHG_EVENTLOOP_H_
#define _MHG_EVENTLOOP_H_

#include"MutexLock.h"
#include"Epoller.h"
#include"EventHandler.h"
#include"CurrentThreadId.h"
#include<memory>
#include<vector>
#include<functional>
#include<mutex>
#include<assert.h>
class EventLoop
{
public:
    
    EventLoop();
    ~EventLoop();
    typedef std::function<void()>Functor;
    typedef std::vector<EventHandler*>EventHandlerList;
    void assertInLoopThread()
    {
        assert(isInLoopThread());
    }
    bool isInLoopThread()const { return _threadId == CurrentThreadId::tid(); }
    void loop();
    void quit();
    void runInLoop(Functor&& cb);
    void queueInLoop(Functor&& cb);
    void shutDown(std::shared_ptr<EventHandler>eh) { ::close(eh->fd()); }

    void removeFromEpoller(std::shared_ptr<EventHandler>eh) { p_epoller->epoll_del(eh); }
    void updateEpoller(std::shared_ptr<EventHandler>eh, int timeout = 0) { p_epoller->epoll_mod(eh, timeout); }
    void addToPoller(std::shared_ptr<EventHandler>eh, int timeout = 0) { p_epoller->epoll_add(eh, timeout); }

    //void updateEventHanlder(EventHandler*);
private:
    bool _looping;
    bool _quit;
    const pid_t _threadId;
    int _wakeupFd;
    bool _eventHandling;
    mutable MutexLock _mutex;
    bool _callingPendingFunctors;

    std::unique_ptr<Epoller> p_epoller;
    EventHandlerList _activeEventList;
    std::vector<Functor>_pendingFunctors;
    std::shared_ptr<EventHandler> sp_wakeupEventHandler;

    void wakeup();
    void handleRead();
    void doPendingFunctors();
    void handleConn();
};
#endif