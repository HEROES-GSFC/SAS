#pragma once 
#include <pthread.h>
#include <chrono>
#include <string>
#include <ctime>

class Semaphore 
{
public:
    Semaphore();
    ~Semaphore();
    void increment();
    void decrement();
private:
    int count;	
    pthread_mutex_t mutex;
};

class Flag 
{
public:
    Flag();
    ~Flag();
    void raise();
    void lower();
    bool check();
private:
    bool value;	
    pthread_mutex_t mutex;
};

timespec TimespecDiff(timespec start, timespec end);
const std::string nanoString(long tv_nsec);
