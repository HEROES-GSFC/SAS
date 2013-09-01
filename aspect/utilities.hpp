#pragma once 
#include <pthread.h>
#include <string>
#include <ctime>
#include <iostream>

#include "Packet.hpp" //for clock_gettime on OS X

class Semaphore 
{
public:
    Semaphore(int maximum = -1);
    ~Semaphore();
    void increment();
    void decrement();
private:
    int count, max;
    pthread_mutex_t mutex;
};

class SemaphoreException : public std::exception
{
    virtual const char* what() const throw()
        {
            return "Exception in Semaphore usage";
        }
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
const std::string MonoTimeSince(timespec &start);
void writeCurrentUT(char *buffer);
