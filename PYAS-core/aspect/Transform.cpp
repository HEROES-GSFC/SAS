#include <math.h>
#include <time.h>

#include "Transform.hpp"

#define PI 3.141592653589793

Transform::Transform()
{
  conversion_intercept = Pair(-3000, 3000);
  conversion_slope = Pair(6, -6);

  calibrated_center = Pair(0, 0);

  distance = 3000.;

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

Pair Transform::getSunAzEl()
{
  prep();

  return Pair(spa.azimuth, 90.-spa.zenith);
}

Pair Transform::getTargetAzEl()
{
  Pair sunAzEl = getSunAzEl();

  //TODO: calculate apparent rotation

  Pair result(sunAzEl.x(), sunAzEl.y());
  return result;
}

Pair Transform::getPointingAzEl(const Pair& sunPixel)
{
  Pair angularShift = getAngularShift(sunPixel);

  Pair newAzEl = addAngularShiftToAzEl(angularShift, getSunAzEl());

  return newAzEl;
}

Pair Transform::getAngularShift(const Pair& sunPixel)
{
  Pair sunScreen = conversion_intercept+conversion_slope*sunPixel;

  Pair shiftScreen = calibrated_center-sunScreen;

  double magnitudeScreen = sqrt(pow(shiftScreen.x(),2)+pow(shiftScreen.y(),2));
  //In mils, so convert to angle (degrees)
  double magnitudeAngle = atan2(magnitudeScreen, distance) * 180/PI;

  //Direction is clockwise from +Y in screen coordinates
  double direction = atan2(shiftScreen.x(), shiftScreen.y()) * 180/PI;

  //return direction as CW from "up" in optical bench coordinates
  return Pair(magnitudeAngle, direction+clocking);
}

Pair addAngularShiftToAzEl(const Pair& angularShift, const Pair& azel)
{
  //The approach here is to choose a convenient space to do these calculations
  //I use a spherical coordinate system
  //First, the pole is the pointing direction
  //Second, the pole is rotated "down" towards -X to the correct elevation
  //Third, the pole is rotated to the correct azimuth
  //One should be careful of "gimbal lock" near zenith!!!

  //Convert everthing to radians
  double magnitudeAngle = angularShift.x() * PI/180;
  double direction = angularShift.y() * PI/180;
  double azimuth = azel.x() * PI/180;
  double elevation = azel.y() * PI/180;

  //Angular shift from "pole" in Cartesian
  //Thus, "up" initially corresponds to +X
  double x0 = sin(magnitudeAngle)*cos(direction);
  double y0 = sin(magnitudeAngle)*sin(direction);
  double z0 = cos(magnitudeAngle);

  //Rotate "pole" down to proper elevation
  double x1 = -x0*sin(elevation)+z0*cos(elevation); 
  double y1 = y0;
  double z1 = x0*cos(elevation)+z0*sin(elevation);

  double outElevation = asin(z1) * 180/PI;
  double outAzimuth = (azimuth + atan2(y1, -x1)) * 180/PI;

  return Pair(outAzimuth, outElevation);
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
  Pair sunAzEl = getSunAzEl();
  Pair pointingAzEl = getPointingAzEl(sunPixel);

  Pair angularShift = getAngularShift(sunPixel);
  std::cout << "Angular shift says: " << angularShift << std::endl;

  Pair result = pointingAzEl-sunAzEl;
  std::cout << "Az/El offset says: " << result << std::endl;

  return result;

}

