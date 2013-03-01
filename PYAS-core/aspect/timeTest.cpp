// time_point::time_since_epoch
#include <iostream>
#include <chrono>

#include <time.h>       /* time_t, struct tm, time, gmtime */

main ()
{
    using namespace std::chrono;

      time_t rawtime;
      struct tm * ptm;

    system_clock::time_point tp = system_clock::now();
    time ( &rawtime );
    system_clock::duration dtn = tp.time_since_epoch();

    std::cout << "numerator: " << system_clock::period::num << "\n";
    std::cout << "denominator: " << system_clock::period::den <<"\n";
    ptm = gmtime ( &rawtime );
    int seconds = dtn.count() * system_clock::period::num / system_clock::period::den;
    int minutes = seconds/60;
    int hours = minutes/60;
    int days = hours/24;
    std::cout << "UTC Epoch (ntp): " << seconds << "\n";
    std::cout << days << " " 
	      << hours - 24*days << ":"
	      << minutes - 60*hours << ":"
	      << seconds - 60*minutes << "\n";

    std::cout << (ptm->tm_hour)%24 << ":" 
	      << ptm->tm_min << ":"
	      << ptm->tm_sec << "\n";

    return 0;
}
