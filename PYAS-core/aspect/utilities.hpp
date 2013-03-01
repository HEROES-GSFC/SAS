#include <mutex>
#include <chrono>

class Semaphore 
{
public:
    Semaphore();
    ~Semaphore();
    void increment();
    void decrement();
private:
    int count;	
    std::mutex mutex;
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
    std::mutex mutex;
};

void fine_wait(int sec, int msec, int usec, int nsec);

double GetSystemTime();
