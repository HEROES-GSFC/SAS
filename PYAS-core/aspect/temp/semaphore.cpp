#include <mutex>
#include <semaphore.hpp>
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
    
