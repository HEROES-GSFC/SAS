/// \file time.cpp
///   C++ source file defining functions to port Linux calls to ETS/Win32.

#include "StdAfx.h"
#include "time.h"
#include <stdint.h>
#include <windows.h>
#include <stdio.h>

static tm linux_time;
static double time_offset=0.0;
static double time_loffset=0.0;
static double time_lslope=0.0;

/**
 *  Mimics the linux nanosleep command by calling sleep.
 *  \param[in]    req     Requested time to sleep (secs + nanosec).
 *  \param[out]   rem     Time remaining to sleep, if interrupted.
 */
int nanosleep( const struct timespec *req, struct timespec *rem )
{
  // Return an erro if the requested time structure is NULL.
  if ( req == NULL )
    return -1;

  // Calculate the sleep time in milliseconds.
  uint32_t mSec = req->tv_sec * 1000 + req->tv_nsec / 1000000;

  // Sleep the milliseconds away with Win32 call.
  Sleep( mSec );

  // Zero the remaining time.
  if ( rem != NULL )
  {
    rem->tv_nsec = 0;
    rem->tv_sec = 0;
  }

  // Return without error.
  return 0;
}

void cnvTimespecToFiletime( const timespec* tspec, FILETIME *filetime )
{
  LONGLONG *ftime = (LONGLONG*)filetime;

  // Convert Linux time to Win32 system time (as FILETIME).
  *ftime = (LONGLONG)(tspec->tv_sec) * (LONGLONG)10000000 + (LONGLONG)(tspec->tv_nsec / 100);
  *ftime += (LONGLONG)116444736000000000;
}

void cnvFiletimeToTimespec( const FILETIME* filetime, timespec *tspec )
{
  LONGLONG *ftime = (LONGLONG*)filetime;
  *ftime -= 116444736000000000;

  tspec->tv_sec = (long)(*ftime / 10000000);
  tspec->tv_nsec = (long)((*ftime * 100) % 1000000000);
}

/**
 *  Mimics the linux clock_gettime command by calling Win32 time function.
 *  \param[in]    clk_id      ID of clock to get time from (ignored).
 *  \param[out]   tp          Time structure of returned time.
 */
int clock_gettime( clockid_t clk_id, struct timespec *tp )
{
  // Get the Win32 system time.
  LONGLONG fTime;
  GetSystemTimeAsFileTime( (FILETIME*)&fTime );

  // Convert it to linux time.
  cnvFiletimeToTimespec( (FILETIME*)&fTime, tp );

  return 0;
}

int clock_settime( clockid_t clk_id, const struct timespec *tp )
{
  FILETIME fTime;
  SYSTEMTIME sysTime;
  cnvTimespecToFiletime( tp, &fTime );
  FileTimeToSystemTime( &fTime, &sysTime );
  SetSystemTime( &sysTime );

  return 0;
}

/**
 *  Mimics the linux gmtime function by calling Win32 time function.
 *  \param[in]    timer     Linux format time value.
 *  \return   Returns a pointer to a statically allocated tm structure.
 */
struct tm* gmtime( const time_t *timer )
{
  timespec tspec;
  tspec.tv_nsec = 0;
  tspec.tv_sec = *timer;

  // Convert Linux time to Win32 system time (as FILETIME).
  LONGLONG ftime;
  cnvTimespecToFiletime( &tspec, (FILETIME*)&ftime );

  // Now convert to system time.
  SYSTEMTIME stime;
  FileTimeToSystemTime( (FILETIME *)&ftime, &stime );

  // Copy values into the tm structure.
  linux_time.tm_sec = stime.wSecond;
  linux_time.tm_min = stime.wMinute;
  linux_time.tm_hour = stime.wHour;
  linux_time.tm_mday = stime.wDay;
  linux_time.tm_mon = stime.wMonth - 1;
  linux_time.tm_year = stime.wYear - 1900;
  linux_time.tm_wday = stime.wDayOfWeek;

  // Zero the rest of the data.
  linux_time.tm_yday = 0;
  linux_time.tm_isdst = 0;

  // Return a pointer to the data.
  return &linux_time;
}

/**
 *  Mimics the linux mktime function by calling Win32 time functions.
 *  \param[in]    timeptr     Pointer to tm structure.
 *  \return   Returns the Linux format time in seconds since Jan 1, 1970.
 */
time_t mktime( struct tm* timeptr )
{
  SYSTEMTIME stime;

  // Copy values into the SYSTEMTIME structure.
  stime.wSecond = timeptr->tm_sec;
  stime.wMinute = timeptr->tm_min;
  stime.wHour = timeptr->tm_hour;
  stime.wDay = timeptr->tm_mday;
  stime.wMonth = timeptr->tm_mon + 1;
  stime.wYear = timeptr->tm_year + 1900;
  stime.wDayOfWeek = timeptr->tm_wday;
  stime.wMilliseconds = 0;

  // Convert the system time to file time.
  LONGLONG ftime;
  SystemTimeToFileTime( &stime, (FILETIME*)&ftime );

  // Convert from file time to timespec.
  timespec tspec;
  cnvFiletimeToTimespec( (FILETIME*)&ftime, &tspec );

  // Return the timespec seconds.
  return tspec.tv_sec;
}

double dblTime( const struct timespec &stime )
{
  double rval = (double)stime.tv_sec;
  rval += (double)stime.tv_nsec * 0.000000001;
  return rval;
}
