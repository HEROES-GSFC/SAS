#include <mutex>
#include <time.h>
#include <utilities.hpp>
Semaphore::Semaphore()
{
    count = 0;
}

Semaphore::~Semaphore(){};

void Semaphore::increment()
{
    mutex.lock();
    count++;
    mutex.unlock();
}
void Semaphore::decrement()
{
    mutex.lock();
    if (count == 0)
    {
	mutex.unlock();
	throw "Counter empty";
    }
    count--;
    mutex.unlock();
}
    
Flag::Flag()
{
    value = 0;
}

Flag::~Flag(){};

void Flag::raise()
{
    mutex.lock();
    value = 0;
    mutex.unlock();
}
void Flag::lower()
{
    mutex.lock();
    value = 1;
    mutex.unlock();
}

bool Flag::check()
{
    bool temp;
    mutex.lock();
    temp = value;
    mutex.unlock();
    return temp;
}

void fine_wait(int sec, int msec, int usec, int nsec)
{
    timespec waittime;
    waittime.tv_sec = sec;
    waittime.tv_nsec = (long) 1000*(1000*msec + usec) + nsec;
    nanosleep(&waittime, NULL);
}
