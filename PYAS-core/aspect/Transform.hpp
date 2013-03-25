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

    //Rotation of screen coordinates to optical bench coordinates (degrees)
    float clocking;

    //Solar target in helioprojective coordinates (arcseconds)
    Pair solar_target;

    spa_data spa;

    void prep();

  public:
    Transform();

    //Retrieves azimuth/elevation of the Sun for the current time
    Pair getAzEl(); 
    void report();
    Pair calculateOffset(const Pair& sunPixel);

    void set_conversion(const Pair& intercept, const Pair& slope);
    void set_calibrated_center(const Pair& arg);
    void set_clocking(float arg);

    void set_solar_target(const Pair& arg);
};

#endif
