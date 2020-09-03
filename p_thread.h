#pragma once

#include<pthread.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<functional>
#include<memory>
#include<string>
#include"CountDownLatch.h"
#include"noncopyable.h"


class Thread :noncopyable
{

public:
    typedef std::function<void()> ThreadFunc;
    explicit Thread(const ThreadFunc& threadFunc, const std::string& name = std::string());
    ~Thread();
    void start();
    int join();
    bool started()const { return _started; }
    pid_t tid()const { return _tid; }
    const std::string& name()const { return _name; }


private:
    void setDefaultName();
    bool _started;
    bool _joined;
    pthread_t _pthreadId;
    pid_t _tid;
    ThreadFunc _threadFunc;
    std::string _name;
    CountDownLatch _latch;



};