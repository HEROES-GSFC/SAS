#include <mutex>

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
