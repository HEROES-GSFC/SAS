/*

  Transform

  Azimuth is defined as eastward from North
  Elevation is defined as poleward from horizontal

  To specify a particular time, some methods accept a time_t value
    Under most implementations, should be seconds since 1970 Jan 1 UT
    Use time() or clock_gettime() to retrieve
    If you have a tm struct, use mktime() to convert to time_t
      Beware of timezones!  mktime() takes input in local time
      That is, mktime() inverts localtime(), not gmtime()
*/

#ifndef _TRANSFORM_HPP_
#define _TRANSFORM_HPP_

#include <time.h>
#include <iostream>

#include "types.hpp"
#include "spa/spa.h"

enum Location
{
    GREENBELT = 0,
    HUNTSVILLE,
    FORT_SUMNER
};

enum Environment
{
    GROUND = 0,
    FLIGHT
};

class Transform {
private:
    //Conversion from pixel to screen coordinates
    Pair conversion_intercept;
    Pair conversion_slope;

    //Calibrated center in screen coordinates (mils)
    Pair calibrated_center;

    //Distance between lens and screen (mm)
    float distance;

    //Rotation of screen coordinates to optical bench coordinates (degrees)
    //When looking at the screen, it is the angle from +Y to "up", CCW positive
    float clocking;

    //Solar target in helioprojective coordinates (arcseconds)
    Pair solar_target;

    spa_data spa; //Calculations for Sun center
    spa_data spa2; //Calculations for ~north pole of the Sun
    double elevation, elevation2;
    double orientation; //Calculated apparent orientation of the Sun's north pole, measured eastward from zenith

    //Converts detector pixel coordinates to an angular shift
    //Output format is that used by translateAzEl()
    Pair getAngularShift(const Pair& sunPixel);

    //Returns a new azimuth/elevation
    //Shifts azel by angle amount.x() in direction amount.y()
    //See code for more explanation
    Pair translateAzEl(const Pair& amount, const Pair& azel);

public:
    Transform(Location location = FORT_SUMNER,
              Environment environment = GROUND);

    //This function must be called sometime before any get* methods
    void calculate(timespec *seconds);

    //These functions return the azimuth/elevation of points of interest
    Pair getSunAzEl(); // Sun center
    Pair getTargetAzEl(); // solar target
    Pair getPointingAzEl(const Pair& sunPixel); // spot on screen in screen physical coordinates (mils)

    double getOrientation() const;

    //These three methods *do* call calculate() internally
    void report(timespec *seconds = NULL);
    Pair calculateOffset(const Pair& sunPixel, timespec *seconds = NULL);
    double calculateOrientation(timespec *seconds = NULL);

    void set_conversion(const Pair& intercept, const Pair& slope);
    void set_calibrated_center(const Pair& arg);
    void set_clocking(float arg);

    void set_solar_target(const Pair& arg);

    float get_clocking() const;
    Pair get_solar_target() const;
};

//Follows the scheme in spa.c but extends the calculation
int spa_calculate2(spa_data *spa, spa_data *spa2, long nanoseconds = 0);


#endif
