#include"Thread.hpp"

Thread::Thread(const ThreadFunc&threadRouthine)
    :_isStarted(false),
    _isJoined(false),
    _threadRoutine(threadRouthine)
    {

    }
Thread::~Thread()
{
    if(_isStarted&&!_isJoined)
    {
        _thread->detach();
    }
}
void Thread::join()
{
    assert(_isStarted);
    assert(!_isJoined);
    _thread->join();
}
void Thread::detach()
{
    assert(_isStarted);
    assert(!_isJoined);
    _thread->detach();

}
void Thread::start()
{
    assert(!_isStarted);
    assert(!_thread);
    _thread.reset(new std::thread(_threadRoutine));
}