#include"Epoller.h"
#include<iostream>
#include"EventHandler.h"
#include<assert.h>
#include<string.h>
#include"EventLoop.h"
const int outMs = 10000;



Epoller::Epoller(EventLoop* loop) :_loop(loop), _events(4096)
{
    Epfd = epoll_create1(EPOLL_CLOEXEC);
    if (Epfd < 0)
    {
        std::cout << "Epoller error" << std::endl;
        abort();
    }
}

Epoller::~Epoller()
{
    //::close(Epfd);
}
void Epoller::epoll_add(sp_EventHandler request, int timeout)
{
    int fd = request->fd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();
    
    _ehlist[fd] = request;
    if (epoll_ctl(Epfd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        perror("epoll_add error\n");
        _ehlist.erase(fd);
    }

}
void Epoller::epoll_mod(sp_EventHandler request, int timeout)
{
    int fd = request->fd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();

    _ehlist[fd] = request;
    if (epoll_ctl(Epfd, EPOLL_CTL_MOD, fd, &event) < 0)
    {
         perror("epoll_mod error\n");
        _ehlist.erase(fd);
    }
}
void Epoller::epoll_del(sp_EventHandler request)
{
    int fd = request->fd();
    struct epoll_event event;
    event.data.fd = fd;
    event.events = request->getEvents();

    _ehlist[fd] = request;
    if (epoll_ctl(Epfd, EPOLL_CTL_MOD, fd, &event) < 0)
    {
        perror("epoll_del error\n");
    }
    _ehlist.erase(fd);
}
     
std::vector<std::shared_ptr<EventHandler>> Epoller::Epoll()
{
    while (true) {
        int numReady = epoll_wait(Epfd,
            &*_events.data(), _events.size(),outMs);
        if (numReady < 0)
        {
            perror("epoll error");
        }
        std::vector<sp_EventHandler>request_data = getEventRequest(numReady);
        if (request_data.size() > 0)return request_data;
    }
}
std::vector<std::shared_ptr<EventHandler>>Epoller::getEventRequest(int events_num)
{
    std::vector<sp_EventHandler>request_data;
    for (int i = 0;i < events_num;++i)
    {
        int fd = _events[i].data.fd;

        sp_EventHandler current_request = _ehlist[fd];

        if (current_request)
        {
            current_request->set_revents(_events[i].events);
            current_request->setEvents(0);
            request_data.push_back(current_request);
        }
        else
        {
            std::cout << " sp current_request is invalid" << std::endl;
            exit(0);
        }
    }
    return request_data;
}

// void Epoller::updateEventHandler(EventHandler* _eh)
// {

//     assertInLoopThread();
//     std::shared_ptr<EventHandler>eh(_eh);
//     const int index = eh->index();
//     int fd = eh->fd();
//     std::cout << "fd = " << eh->fd() << " events = " << eh->events();
//     if (eh->index() < 0)
//     {
//         assert(_ehlist.find(eh->fd()) == _ehlist.end());
//         _ehlist[fd] = eh;
//         eh->set_index(index + 1);
//         update(EPOLL_CTL_ADD, eh);
//     }
//     else
//     {
//         assert(_ehlist.find(eh->fd()) == _ehlist.end());
//         assert(_ehlist[eh->fd()] == eh);

//         update(EPOLL_CTL_MOD, eh);

//     }
// }
