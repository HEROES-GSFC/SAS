/*

Transform

Azimuth is defined as eastward from North
Elevation is defined as poleward from horizontal


*/

#ifndef _TRANSFORM_HPP_
#define _TRANSFORM_HPP_

#include <iostream>

#include "types.hpp"
#include "spa/spa.h"

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

    void prep();

  public:
    Transform();

    Pair getSunAzEl(); //azimuth/elevation of the Sun
    Pair getTargetAzEl(); //azimuth/elevation of the solar target
    Pair getPointingAzEl(const Pair& sunPixel); //azimuth/elevation of the current pointing

    Pair getAngularShift(const Pair& sunPixel);

    double getOrientation();

    //returns new azimuth/elevation
    Pair translateAzEl(const Pair& amount, const Pair& azel);

    void report();
    Pair calculateOffset(const Pair& sunPixel);

    void set_conversion(const Pair& intercept, const Pair& slope);
    void set_calibrated_center(const Pair& arg);
    void set_clocking(float arg);

    void set_solar_target(const Pair& arg);
};

//Follows the scheme in spa.c but extends the calculation
int spa_calculate2(spa_data *spa, spa_data *spa2);


#endif
