#include <time.h>

#include "Transform.hpp"

Transform::Transform()
{
  conversion_intercept = Pair(-3000, 3000);
  conversion_slope = Pair(6, -6);

  calibrated_center = Pair(0, 0);

  clocking = 0;

  solar_target = Pair(0, 0);

  spa.delta_t       = 67.1116;

  //Greenbelt
  spa.longitude     = -76.8758;
  spa.latitude      = 39.0044;

  //Huntsville
  //spa.longitude     = 86.5861;
  //spa.latitude      = 34.7303;

  //Fort Sumner
  //spa.longitude     = -104.2450;
  //spa.latitude      = 34.4717;

  //Ground
  spa.elevation       = 0; //meters
  spa.pressure        = 1013; //millibars
  spa.temperature     = 20; //degrees Celsius

  //Flight
  //spa.elevation     = 40000; //meters
  //spa.pressure      = 3; //millibars
  //spa.temperature   = -50; //degrees Celsius

  spa.slope         = 0;
  spa.azm_rotation  = 0;
  spa.atmos_refract = 0.5667;
  spa.function      = SPA_ALL;
}

void Transform::prep()
{
  time_t t;
  time(&t);
  struct tm *now;
  now = gmtime(&t);

  spa.timezone      = -0.0;
  spa.year          = now->tm_year+1900;
  spa.month         = now->tm_mon+1;
  spa.day           = now->tm_mday;
  spa.hour          = now->tm_hour;
  spa.minute        = now->tm_min;
  spa.second        = now->tm_sec;

  spa_calculate(&spa);
}

void Transform::set_conversion(const Pair& intercept, const Pair& slope)
{
  conversion_intercept = intercept;
  conversion_slope = slope;
}

void Transform::set_calibrated_center(const Pair& arg)
{
  calibrated_center = arg;
}

void Transform::set_clocking(float arg)
{
  clocking = arg;
}

Pair Transform::getAzEl()
{
  prep();

  return Pair(spa.azimuth, 90.-spa.zenith);
}

void Transform::report()
{
  prep();

  std::cout << "Azimuth: " << spa.azimuth << std::endl;
  std::cout << "Elevation: " << 90.-spa.zenith << std::endl;

  std::cout << "Sun transit altitude: " << spa.sta << std::endl;

  std::cout << "Sunrise: " << spa.sunrise << std::endl;
  std::cout << "Sun transit: " << spa.suntransit << std::endl;
  std::cout << "Sunset: " << spa.sunset << std::endl;
}

Pair Transform::calculateOffset(const Pair& sunPixel)
{
  Pair sunScreen(conversion_intercept.x()+conversion_slope.x()*sunPixel.x(),
                 conversion_intercept.y()+conversion_slope.y()*sunPixel.y());

  //calibrated center

  //clocking

  //solar target (plus apparent rotation)

  //For now, a crude approximation with target at Sun center with no rotations
  Pair result(sunScreen.x()/6.*10.6/3600., sunScreen.y()/6.*10.6/3600);

  return result;
}

