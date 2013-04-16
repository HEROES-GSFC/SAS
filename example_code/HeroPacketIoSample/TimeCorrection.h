/// \file TimeCorrection.h
///   Interface for the TimeCorrection class and dependent classes.

#if !defined(AFX_TIMECORRECTION_H__DC8D856C_6DA2_46DA_9636_E4C679DCD897__INCLUDED_)
#define AFX_TIMECORRECTION_H__DC8D856C_6DA2_46DA_9636_E4C679DCD897__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <time.h>

/**
 *  Virtual base class used to establish interface for time correction classes.
 */
class TimeCorrection  
{
public:
  TimeCorrection() {};
  virtual ~TimeCorrection() {};

  /**
   *  Returns current converted time as a double precision value.
   *  \param[out]   tout      Current converted time represeted as double.
   */
  virtual int getTime( double &td ) {return -1;};

  /**
   *  Returns current converted time as a timespec structure.
   *  \param[out]   tout      Current converted time represeted as timespec.
   */
  virtual int getTime( struct timespec &tm ) {return -1;};

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as double.
   *  \param[out]   tout      Current converted time represented as double.
   */
  virtual void correctTime( double &tin, double &tout ) {};

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as double.
   *  \param[out]     tout      Current converted time represented as timespec.
   */
  virtual void correctTime( double &tin, struct timespec &tout ) {};

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as timespec.
   *  \param[out]   tout      Current converted time represented as double.
   */
  virtual void correctTime( struct timespec &tin, double &tout ) {};

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as timespec.
   *  \param[out]   tout      Current converted time represented as timespec.
   */
  virtual void correctTime( struct timespec &tin, struct timespec &tout )
  {};

  /**
   *  Converts input timespec time into the same time in double precision.
   *  \param[in]    tin       Time represented as timespec.
   *  \param[out]   tout      Time converted to double precision.
   */
  static void convertTime( struct timespec &tin, double &tout )
  {
    tout = (double)tin.tv_nsec * 0.000000001 + (double)tin.tv_sec;
  }

  /**
   *  Convert input double precision time into the same time as timespec.
   *  \param[in]    tin       Time represented as double precision.
   *  \param[out]   tout      Time converted to timespec structure.
   */
  static void convertTime( double &tin, struct timespec &tout )
  {
    tout.tv_sec = (long)tin;
    tout.tv_nsec = (long)((tin - (double)tout.tv_sec) * 1000000000.0);
  }
};

/**
 *  Applies simple offset time correction to raw clock time.
 */
class OffsetTimeCorrection : public TimeCorrection
{
public:
  OffsetTimeCorrection() {this->offset = 0.;};

  virtual ~OffsetTimeCorrection() {};

  /**
   *  Returns current converted time as a double precision value.
   *  \param[out]   tout      Current converted time represeted as double.
   */
  virtual int getTime( double &td )
  {
    struct timespec tin;
    int rval = clock_gettime( CLOCK_REALTIME, &tin );
    correctTime( tin, td );
    return rval;
  }

  /**
   *  Returns current converted time as a timespec structure.
   *  \param[out]   tout      Current converted time represeted as timespec.
   */
  virtual int getTime( struct timespec &tm )
  {
    struct timespec tin;
    int rval = clock_gettime( CLOCK_REALTIME, &tin );
    correctTime( tin, tm );
    return rval;
  }

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as double.
   *  \param[out]   tout      Current converted time represented as double.
   */
  virtual void correctTime( double &tin, double &tout )
  {
    tout = tin + this->offset;
  }

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as double.
   *  \param[out]   tout      Current converted time represented as timespec.
   */
  virtual void correctTime( double &tin, struct timespec &tout )
  {
    double td;
    correctTime( tin, td );
    convertTime( td, tout );
  }

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as timespec.
   *  \param[out]   tout      Current converted time represented as double.
   */
  virtual void correctTime( struct timespec &tin, double &tout )
  {
    double td;
    convertTime( tin, td );
    correctTime( td, tout );
  }

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as timespec.
   *  \param[out]   tout      Current converted time represented as timespec.
   */
  virtual void correctTime( struct timespec &tin, struct timespec &tout )
  {
    double td;
    convertTime( tin, td );
    correctTime( td, tout );
  }

  /**
   *  Sets the clock offset (seconds).
   */
  void setOffset( double offset )
  {
    this->offset = offset;
  };

protected:
  double      offset;     ///< Offset of corrected time from raw time.
};

/**
 *  Applies linear fit time correction to raw clock time.
 */
class LinearTimeCorrection : public OffsetTimeCorrection
{
public:
  LinearTimeCorrection()
  {
    this->slope = 0.;
  };

  virtual ~LinearTimeCorrection() {};

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as double.
   *  \param[out]   tout      Current converted time represented as double.
   */
  virtual void correctTime( double &tin, double &tout )
  {
    tout = tin + (tin * this->slope + this->offset);
  }

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as double.
   *  \param[out]   tout      Current converted time represented as timespec.
   */
  virtual void correctTime( double &tin, struct timespec &tout )
  {
    double td;
    correctTime( tin, td );
    convertTime( td, tout );
  }

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as timespec.
   *  \param[out]   tout      Current converted time represented as double.
   */
  virtual void correctTime( struct timespec &tin, double &tout )
  {
    double td;
    convertTime( tin, td );
    correctTime( td, tout );
  }

  /**
   *  Converts input time to corrected time.
   *  \param[in]    tin       Current raw clock time represented as timespec.
   *  \param[out]   tout      Current converted time represented as timespec.
   */
  virtual void correctTime( struct timespec &tin, struct timespec &tout )
  {
    double td;
    convertTime( tin, td );
    correctTime( td, tout );
  }

  /**
   *  Sets the clock offset (seconds).
   */
  void setParameters( double slope, double offset )
  {
    this->slope = slope;
    this->offset = offset;
  };

protected:
  double      slope;      ///< Slope of corrected time from raw time.
};

#endif // !defined(AFX_TIMECORRECTION_H__DC8D856C_6DA2_46DA_9636_E4C679DCD897__INCLUDED_)
