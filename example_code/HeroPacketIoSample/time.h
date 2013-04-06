/// \file time.h
///   Win32 file to provide support of Linux time structures and functions.

#ifndef __TIME_H_DECLARED__
#define __TIME_H_DECLARED__

typedef int clockid_t;

#define CLOCK_REALTIME 1      ///< ID of periodically offset-adjusted clock.
#define CLOCK_MONOTONIC 2     ///< ID of non-adjusted clock (monotonic).
#define CLOCK_LINEAR 3        ///< ID of linear-fit adjust clock.

struct timespec
{
  long  tv_sec;        /* seconds */
  long  tv_nsec;       /* nanoseconds */
};

#ifndef _TM_DEFINED
struct tm
{
  int   tm_sec;       ///< Seconds (0-59).
  int   tm_min;       ///< Minutes (0-59).
  int   tm_hour;      ///< Hour of the day (0-23).
  int   tm_mday;      ///< Day of the month (1-31).
  int   tm_mon;       ///< Months since January (0-11).
  int   tm_year;      ///< Years since 1900 (0 - 2**31).
  int   tm_wday;      ///< Day of the week (days since Sunday, 0-6).
  int   tm_yday;      ///< Day of the year (days since Jan. 1, 0-365).
  int   tm_isdst;     ///< Daylight savings flag (>0 if DST, 0 otherwise).
};
#define _TM_DEFINED
#endif

int nanosleep( const struct timespec *req, struct timespec *rem );

int clock_gettime( clockid_t clk_id, struct timespec *tp );

int clock_settime( clockid_t clk_id, const struct timespec *tp );

struct tm* gmtime( const time_t *timer );

time_t mktime( struct tm* timeptr );

double dblTime( const struct timespec &stime );

#endif // #ifndef __TIME_H_DECLARED__