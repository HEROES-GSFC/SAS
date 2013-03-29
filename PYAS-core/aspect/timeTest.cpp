// time_point::time_since_epoch
#include <iostream>
#include <chrono>

#include <ctime>       /* time_t, struct tm, time, gmtime */

main ()
{
    
    using namespace std::chrono;

    timespec clocktime, latertime, difftime;
    int useconds, mseconds, seconds, minutes, hours, days;
    timespec waittime;
    waittime.tv_sec = 1;
    waittime.tv_nsec = 0L;
    while(true)
    {	
    nanosleep(&waittime, NULL);
    system_clock::time_point tp = system_clock::now();
    system_clock::duration dtn = tp.time_since_epoch();
    clock_gettime(CLOCK_REALTIME, &clocktime);
    useconds = dtn.count() ;
    mseconds = dtn.count()/1000L;
    seconds = dtn.count()/1000000L;
    minutes = seconds/60;
    hours = minutes/60;
    days = hours/24;
    std::cout << days << " " 
	      << hours - 24*days << ":"
	      << minutes - 60*hours << ":"
	      << seconds - 60*minutes << "."
	      << mseconds -1000*seconds << "."
	      << useconds - 1000*mseconds << "\n";
	      

    minutes = clocktime.tv_sec/60;
    hours = minutes/60;
    days = hours/24;
    std::cout << days << " " 
	      << hours - 24*days << ":"
	      << minutes - 60*hours << ":"
	      << clocktime.tv_sec - 60*minutes << "."
	      << clocktime.tv_nsec/1000000 << "."
	      << (clocktime.tv_nsec/1000)%1000 << "."
	      << (clocktime.tv_nsec/1000000) << "\n";
    clock_gettime(CLOCK_REALTIME, &latertime);
    difftime.tv_sec = latertime.tv_sec-clocktime.tv_sec;
    difftime.tv_nsec = latertime.tv_nsec-clocktime.tv_nsec;
    std::cout << latertime.tv_nsec/1000000 << "."
	      << (latertime.tv_nsec/1000)%1000 << "."
	      << (latertime.tv_nsec/1000000) << "\n";
    std::cout << ctime(&clocktime.tv_sec);
        std::cout << "\x1b[A\x1b[A\x1b[A\x1b[A\r";

    std::cout.flush();

    
    }
    return 0;
}
	
