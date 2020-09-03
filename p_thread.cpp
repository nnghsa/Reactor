#include "p_thread.h"
#include <assert.h>
#include <errno.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include "CurrentThreadId.h"

#include<iostream>
using namespace std;
__thread const char*  t_threadName="default";
struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc _threadFunc;
    string _name;
    pid_t*_tid;
    CountDownLatch *_latch;

    ThreadData(const ThreadFunc&threadFunc,const std::string&name,pid_t *tid,
    CountDownLatch*latch):
    _threadFunc(threadFunc),_name(name),_tid(tid),_latch(latch){}

    void runInThread()
    {
        *_tid=CurrentThreadId::tid();
        _tid=nullptr;
        _latch->countDown();
        _latch=nullptr;
        t_threadName=_name.empty()?"Thread":_name.c_str();

        prctl(PR_SET_NAME,t_threadName);
        _threadFunc();
        t_threadName="finished";
    }
    
};
void *startThread(void *obj)
{
    ThreadData *data=static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}
Thread::Thread(const ThreadFunc&func,const string&name):
_started(false),
_joined(false),
_pthreadId(0),
_tid(0),
_threadFunc(func),
_name(name),
_latch(1)
{
    setDefaultName();
}
Thread::~Thread()
{
    if(_started&&!_joined)pthread_detach(_pthreadId);
}
void Thread::setDefaultName()
{
    if(_name.empty())
    {
        _name="Thread";
    }
}
void Thread::start()
{
    assert(!_started);
    _started=true;
    ThreadData*data=new ThreadData(_threadFunc,_name,&_tid,&_latch);
    if(pthread_create(&_pthreadId,NULL,&startThread,data))
    {
        _started=false;
        delete data;
    }
    else
    {
        _latch.wait();
        assert(_tid>0);
    }
}
int Thread::join()
{
    assert(_started);
    assert(!_joined);
    _joined=true;
    return pthread_join(_pthreadId,NULL);
}