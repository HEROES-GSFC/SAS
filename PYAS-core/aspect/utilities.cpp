#include <time.h>
#include "utilities.hpp"

Semaphore::Semaphore()
{
    pthread_mutex_init(&mutex, NULL);
    count = 0;
}

Semaphore::~Semaphore()
{
    pthread_mutex_destroy(&mutex);
}

void Semaphore::increment()
{
    pthread_mutex_lock(&mutex);
    count++;
    pthread_mutex_unlock(&mutex);
}
void Semaphore::decrement()
{
    pthread_mutex_lock(&mutex);;
    if (count == 0)
    {
	pthread_mutex_unlock(&mutex);
	throw "Counter empty";
    }
    count--;
    pthread_mutex_unlock(&mutex);
}
    
Flag::Flag()
{
    pthread_mutex_init(&mutex, NULL);
    value = 0;
}

Flag::~Flag()
{
    pthread_mutex_destroy(&mutex);
}

void Flag::raise()
{
    pthread_mutex_lock(&mutex);
    value = 1;
    pthread_mutex_unlock(&mutex);
}
void Flag::lower()
{
    pthread_mutex_lock(&mutex);
    value = 0;
    pthread_mutex_unlock(&mutex);
}

bool Flag::check()
{
    bool temp;
    pthread_mutex_lock(&mutex);
    temp = value;
    pthread_mutex_unlock(&mutex);
    return temp;
}

void fine_wait(int sec, int msec, int usec, int nsec)
{
    timespec waittime;
    waittime.tv_sec = sec;
    waittime.tv_nsec = (long) 1000*(1000*msec + usec) + nsec;
    nanosleep(&waittime, NULL);
}

double GetSystemTime()
{
    using namespace std::chrono;

    system_clock::time_point tp = system_clock::now();
    system_clock::duration dtn = tp.time_since_epoch();
    return (double) dtn.count() * system_clock::period::num / system_clock::period::den;
}

