#ifndef _MHG_EVENTHANDLER_
#define _MHG_EVENTHANDLER_
#include<functional>
#include<poll.h>
#include<sys/types.h>
#include<unistd.h>
#include<string>
#include"CurrentThreadId.h"
class EventLoop;
class EventHandler
{

public:
    EventHandler(EventLoop*,int fd);
    EventHandler(EventLoop*loop);
    ~EventHandler();
    typedef std::function<void()>Functor;

    void setReadCallBack(const Functor &cb) {
        _readCallBack=cb;
    };
    void setWriteCallBack(const Functor &cb)
    {
        _writeCallBack=cb;
    }
    void setErrorCallBack(const Functor &cb)
    {
        _errorCallBack=cb;
    }
    void setConnHandler(const Functor&cb)
    {
        _connCallBack=cb;
    }
 
    void handleEvent();

    int index(){return _index;}
    void set_index(int idx){_index=idx;}
    void echofunc()
    {
        char buf[BUFSIZ];
        ssize_t numRead;
        while((numRead=read(_fd,buf,BUFSIZ))>0)
        {
            printf("%s\n",buf);
            printf("numread = %d\n",numRead);
            write(_fd,buf,sizeof numRead);
        }
        std::string a=std::to_string(CurrentThreadId::tid());
        write(_fd,a.c_str(),sizeof(int));
    }
    
    int fd(){return _fd;}

    void setFd(int fd){_fd=fd;}
    int getEvents()const{return _events;}
    void set_revents(int revt){_revents=revt;}
    void setEvents(int ev){_events=ev;}
    //bool isNoneEvent()const{return _events==kNoneEvent;}

    //EventLoop*ownerloop(){return _loop;}



private:


    int _fd;
    int _index;
    int  _events;
    int _revents;

    EventLoop*_loop;

    Functor _readCallBack;
    Functor _writeCallBack;
    Functor _errorCallBack;
    Functor _connCallBack;


};
#endif