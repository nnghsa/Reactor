

#include<iostream>
#include"EventHandler.h"
#include"EventLoop.h"

EventHandler::EventHandler
(EventLoop*loop,int fd):_fd(fd),_events(0),_loop(loop)
{

}
EventHandler::EventHandler(EventLoop*loop):
_loop(loop),_events(0),_fd(0)
{

}

EventHandler::~EventHandler()
{

}

void EventHandler::handleEvent()
{
    if(_revents&POLLNVAL)
    {
        std::cout<<"handleEvent() POLLNVAL"<<std::endl;
    }
    if(_revents&(POLLERR|POLLNVAL))
    {
        if(_errorCallBack)_errorCallBack();return;
    }
    if(_revents&(POLLIN|POLLPRI|POLLRDHUP))
    {
        if(_readCallBack)_readCallBack();return;
    }
    if(_revents&POLLOUT)
    {
        if(_writeCallBack)_writeCallBack();
    }
}
