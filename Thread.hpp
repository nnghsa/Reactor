#ifndef _MHG_THREAD_HPP_
#define _MHG_THREAD_HPP_

#include<functional>
#include<thread>
#include<memory>
#include<assert.h>
#include"CurrentThreadId.h"
class Thread
{
public:
    typedef std::function<void()>ThreadFunc;

    explicit Thread(const ThreadFunc&threadRoutine);
    ~Thread();

    void start();
    void join();
    void detach();

    bool isStarted() {
        return _isStarted;
    }
    bool isJoined() {
        return _isJoined;
    }

    int getThreadId()const
    {
        assert(_isStarted);
        return CurrentThreadId::tid();
    }
private:
    Thread(const Thread&);
    const Thread&operator=(const Thread&);

    bool _isStarted;
    bool _isJoined;
    ThreadFunc _threadRoutine;
    std::unique_ptr<std::thread> _thread;
};

#endif