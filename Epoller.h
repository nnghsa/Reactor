#ifndef _MHG_EPOLLER_H_
#define _MHG_EPOLLER_H_
#include<sys/epoll.h>
#include<vector>
#include<map>
#include<memory>
class EventLoop;
class EventHandler;
class Epoller
{
    public:
    typedef std::vector<EventHandler*> EhList;
    typedef std::shared_ptr<EventHandler>sp_EventHandler;
    Epoller(EventLoop*loop);
    ~Epoller();
    
    void epoll_add(sp_EventHandler request,int timeout);
    void epoll_mod(sp_EventHandler request,int timeout);
    void epoll_del(sp_EventHandler request);

    std::vector<std::shared_ptr<EventHandler>>Epoll();
    std::vector<std::shared_ptr<EventHandler>>getEventRequest(int events_num);
    
    int getEpollFd(){return Epfd;}
    void assertInLoopThread();

private:
    void update(int opreation,sp_EventHandler eh);
    /* data */
    std::vector<epoll_event>__events;
    int ev_index;
    int Epfd;
    EventLoop*_loop;
    typedef std::map<int ,sp_EventHandler>  Map_EhList;
    std::vector<struct epoll_event> _events;
    Map_EhList _ehlist;
};
#endif
