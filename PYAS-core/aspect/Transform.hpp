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

    spa_data spa;

    void prep();

public:
    Transform();

    Pair getSunAzEl(); //azimuth/elevation of the Sun
    Pair getTargetAzEl(); //azimuth/elevation of the solar target
    Pair getPointingAzEl(const Pair& sunPixel); //azimuth/elevation of the current pointing

    Pair getAngularShift(const Pair& sunPixel);

    //returns new azimuth/elevation
    Pair addAngularShiftToAzEl(const Pair& angularShift, const Pair& azel);

    void report();
    Pair calculateOffset(const Pair& sunPixel);

    void set_conversion(const Pair& intercept, const Pair& slope);
    void set_calibrated_center(const Pair& arg);
    void set_clocking(float arg);

    void set_solar_target(const Pair& arg);
};

#endif
