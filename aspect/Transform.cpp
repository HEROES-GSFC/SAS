#include <math.h>
#include <time.h>
#include <string.h>

#include "Transform.hpp"
//Include the spa code directly to be able to use private functions
#include "spa/spa.c"

Transform::Transform(Location location, Environment environment)
{
    conversion_intercept = Pair(-3000, 3000);
    conversion_slope = Pair(6, -6);

    calibrated_center = Pair(0, 0);

    distance = 3000.;

    clocking = 97.;

    solar_target = Pair(0, 0);

    spa.delta_t       = 32.184+35-0.03975; //as of 2013 Aug 26

    switch(location) {
        case GREENBELT:
            spa.longitude     = -76.8758;
            spa.latitude      = 39.0044;
            break;
        case HUNTSVILLE:
            spa.longitude     = -86.5861;
            spa.latitude      = 34.7303;
            break;
        case FORT_SUMNER:
        default:
            //New hangar, values from Google Earth
            spa.longitude     = -104.221800;
            spa.latitude      = 34.490000;
    }

    switch(environment) {
        case GROUND:
            spa.elevation       = 1280; //meters
            spa.pressure        = 1013; //millibars
            spa.temperature     = 20; //degrees Celsius
            break;
        case FLIGHT:
        default:
            spa.elevation     = 38405; //meters
            spa.pressure      = 3; //millibars
            spa.temperature   = -50; //degrees Celsius
    }

    spa.slope         = 0;
    spa.azm_rotation  = 0;
    spa.atmos_refract = 0.5667;
    spa.function      = SPA_ALL;
}

void Transform::calculate(const struct timespec &seconds)
{
    struct tm *input_time;
    input_time = gmtime(&seconds.tv_sec);

    spa.timezone      = -0.0;
    spa.year          = input_time->tm_year+1900;
    spa.month         = input_time->tm_mon+1;
    spa.day           = input_time->tm_mday;
    spa.hour          = input_time->tm_hour;
    spa.minute        = input_time->tm_min;
    spa.second        = input_time->tm_sec;

    spa_calculate2(&spa, &spa2, seconds.tv_nsec);

    elevation = 90.-spa.zenith;
    elevation2 = 90.-spa2.zenith;

    double u_x = cos(deg2rad(elevation));
    //double u_y = 0;
    double u_z = sin(deg2rad(elevation));

    double v_x = cos(deg2rad(elevation2))*cos(deg2rad(spa2.azimuth-spa.azimuth));
    double v_y = cos(deg2rad(elevation2))*sin(deg2rad(spa2.azimuth-spa.azimuth));
    double v_z = sin(deg2rad(elevation2));

    double mag = sqrt(pow(v_x-u_x,2)+pow(v_y,2)+pow(v_z-u_z,2));

    orientation = rad2deg(asin(v_y/mag));
    if (elevation2 < elevation) orientation = 180.-orientation;
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

void Transform::set_solar_target(const Pair& arg)
{
    solar_target = arg;
}

void Transform::set_lat_lon(const Pair& lat_lon)
{
    spa.latitude = lat_lon.x();
    spa.longitude = lat_lon.y();
}

Pair Transform::getSunAzEl()
{
    return Pair(spa.azimuth, 90.-spa.zenith);
}

Pair Transform::getTargetAzEl()
{
    //double delta_az = (solar_target.x()*cos(deg2rad(orientation))+solar_target.y()*sin(deg2rad(orientation)))/3600.;
    //double delta_el = (solar_target.y()*cos(deg2rad(orientation))-solar_target.x()*sin(deg2rad(orientation)))/3600.;
    //std::cout << delta_az << ", " << delta_el << std::endl;

    double mag = sqrt(pow(solar_target.x(),2)+pow(solar_target.y(),2))/3600.;
    double angle = rad2deg(atan2(solar_target.x(),solar_target.y()))+orientation;

    Pair result = translateAzEl(Pair(mag,angle), getSunAzEl());
    //std::cout << result << std::endl;

    return result;
}

Pair Transform::getPointingAzEl(const Pair& sunPixel)
{
    Pair angularShift = getAngularShift(sunPixel);

    Pair newAzEl = translateAzEl(angularShift, getSunAzEl());

    return newAzEl;
}

Pair Transform::getAngularShift(const Pair& sunPixel)
{
    Pair sunScreen = conversion_intercept+conversion_slope*sunPixel;

    //The difference we want here is pointing vector minus Sun-center vector
    //There is an extra inversion through the optics, hence the direction of subtraction
    Pair shiftScreen = sunScreen-calibrated_center;

    double magnitudeScreen = sqrt(pow(shiftScreen.x(),2)+pow(shiftScreen.y(),2));
    //In mils, so convert to angle (degrees)
    double magnitudeAngle = rad2deg(atan2(magnitudeScreen/1000, distance/25.4));

    //Direction is clockwise from +Y in screen coordinates
    double direction = rad2deg(atan2(shiftScreen.x(), shiftScreen.y()));

    //Direction+clocking gives rotation CW from "up" in optical bench
    //  coordinates, when looking from the front towards the screen
    //Multiply by -1 to give rotation CW from "up" when looking in the
    //  pointing direction
    return Pair(magnitudeAngle, -(direction+clocking));
}

double Transform::getOrientation() const
{
    return orientation;
}

double Transform::calculateOrientation(const struct timespec &seconds)
{
    calculate(seconds);
    return orientation;
}

float Transform::get_clocking() const { return clocking; }
Pair Transform::get_lat_lon() const { return Pair(spa.latitude, spa.longitude); }
Pair Transform::get_solar_target() const { return solar_target; }

Pair Transform::translateAzEl(const Pair& amount, const Pair& azel)
{
    //amount is [magnitude, direction]

    //The approach here is to choose a convenient space to do these calculations
    //I use a spherical coordinate system
    //First, the pole is the pointing direction
    //Second, the pole is rotated "down" towards -X to the correct elevation
    //Third, the pole is rotated to the correct azimuth
    //One should be careful of "gimbal lock" near zenith!!!

    //Convert everthing to radians
    double magnitudeAngle = deg2rad(amount.x());
    double direction = deg2rad(amount.y());
    double azimuth = deg2rad(azel.x());
    double elevation = deg2rad(azel.y());

    //Angular shift from "pole" in Cartesian
    //Thus, "up" initially corresponds to +X
    double x0 = sin(magnitudeAngle)*cos(direction);
    double y0 = sin(magnitudeAngle)*sin(direction);
    double z0 = cos(magnitudeAngle);

    //Rotate "pole" down to proper elevation
    double x1 = x0*sin(elevation)-z0*cos(elevation); 
    double y1 = y0;
    double z1 = x0*cos(elevation)+z0*sin(elevation);

    double outElevation = rad2deg(asin(z1));
    double outAzimuth = rad2deg((azimuth + atan2(y1, -x1)));

    return Pair(outAzimuth, outElevation);
}

void Transform::report()
{
    timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    report(now);
}

void Transform::report(const struct timespec &seconds)
{
    calculate(seconds);

    std::cout << "*** Sun center ***\n";
    std::cout << "Azimuth: " << spa.azimuth << std::endl;
    std::cout << "Elevation: " << elevation << std::endl;

    std::cout << "Sun transit altitude: " << spa.sta << std::endl;

    std::cout << "Sunrise: " << spa.sunrise << std::endl;
    std::cout << "Sun transit: " << spa.suntransit << std::endl;
    std::cout << "Sunset: " << spa.sunset << std::endl;

    std::cout << "*** Solar north pole ***\n";
    std::cout << "Azimuth: " << spa2.azimuth << std::endl;
    std::cout << "Elevation: " << elevation2 << std::endl;

    std::cout << "*** Orientation ***\n";
    std::cout << "Angle: " << orientation << std::endl;
}

Pair Transform::calculateOffset(const Pair& sunPixel, const struct timespec &seconds)
{
    calculate(seconds);

    //If we get (0,0), assume that it's not a valid Sun center, and return a "no-move" offset
    if ((sunPixel.x() == 0) && (sunPixel.y() == 0)) return Pair(0,0);

/*
//For small angles, this calculation should be sufficient for a solar target at disc center
Pair angularShift = getAngularShift(sunPixel);
Pair result(angularShift.x()*sin(angularShift.y()*PI/180), angularShift.x()*cos(angularShift.y()*PI/180));
*/

    Pair sunAzEl = getTargetAzEl();
    Pair pointingAzEl = getPointingAzEl(sunPixel);

    Pair result = pointingAzEl-sunAzEl; //straight subtraction

    //std::cout << result.x()*3600. << ", " << result.y()*3600. << std::endl;

    return result;
}

int spa_calculate2(spa_data *spa, spa_data *spa2, long nanoseconds)
{
    //Code adapated from spa_calculate
    int result;

    result = validate_inputs(spa);

    if (result == 0)
    {
        spa->jd = julian_day (spa->year, spa->month,  spa->day,
                              spa->hour, spa->minute, spa->second, spa->timezone);
        spa->jd += nanoseconds/1.e9/86400.;

        //Begin code adapted from calculate_geocentric_sun_right_ascension_and_declination
        double x[TERM_X_COUNT];

        spa->jc = julian_century(spa->jd);

        spa->jde = julian_ephemeris_day(spa->jd, spa->delta_t);
        spa->jce = julian_ephemeris_century(spa->jde);
        spa->jme = julian_ephemeris_millennium(spa->jce);

        spa->l = earth_heliocentric_longitude(spa->jme);
        spa->b = earth_heliocentric_latitude(spa->jme);
        spa->r = earth_radius_vector(spa->jme);

        spa->theta = geocentric_longitude(spa->l);
        spa->beta  = geocentric_latitude(spa->b);

        x[TERM_X0] = spa->x0 = mean_elongation_moon_sun(spa->jce);
        x[TERM_X1] = spa->x1 = mean_anomaly_sun(spa->jce);
        x[TERM_X2] = spa->x2 = mean_anomaly_moon(spa->jce);
        x[TERM_X3] = spa->x3 = argument_latitude_moon(spa->jce);
        x[TERM_X4] = spa->x4 = ascending_longitude_moon(spa->jce);

        nutation_longitude_and_obliquity(spa->jce, x, &(spa->del_psi), &(spa->del_epsilon));

        spa->epsilon0 = ecliptic_mean_obliquity(spa->jme);
        spa->epsilon  = ecliptic_true_obliquity(spa->del_epsilon, spa->epsilon0);

        spa->del_tau   = aberration_correction(spa->r);
        spa->lamda     = apparent_sun_longitude(spa->theta, spa->del_psi, spa->del_tau);
        spa->nu0       = greenwich_mean_sidereal_time (spa->jd, spa->jc);
        spa->nu        = greenwich_sidereal_time (spa->nu0, spa->del_psi, spa->epsilon);

        //Increase geocentric latitude by a quarter degree for the ~north pole of the Sun
        memcpy(spa2, spa, sizeof(spa_data));
        spa2->beta  = spa->beta + 0.25;
        spa2->alpha = geocentric_sun_right_ascension(spa2->lamda, spa2->epsilon, spa2->beta);
        spa2->delta = geocentric_sun_declination(spa2->beta, spa2->epsilon, spa2->lamda);

        spa->alpha = geocentric_sun_right_ascension(spa->lamda, spa->epsilon, spa->beta);
        spa->delta = geocentric_sun_declination(spa->beta, spa->epsilon, spa->lamda);
        //End code from calculate_geocentric_sun_right_ascension_and_declination

        //First, calculate for ~north pole of the Sun
        spa2->h  = observer_hour_angle(spa2->nu, spa2->longitude, spa2->alpha);
        spa2->xi = sun_equatorial_horizontal_parallax(spa2->r);

        sun_right_ascension_parallax_and_topocentric_dec(spa2->latitude, spa2->elevation, spa2->xi,
                                    spa2->h, spa2->delta, &(spa2->del_alpha), &(spa2->delta_prime));

        spa2->alpha_prime = topocentric_sun_right_ascension(spa2->alpha, spa2->del_alpha);
        spa2->h_prime     = topocentric_local_hour_angle(spa2->h, spa2->del_alpha);

        spa2->e0      = topocentric_elevation_angle(spa2->latitude, spa2->delta_prime, spa2->h_prime);
        spa2->del_e   = atmospheric_refraction_correction(spa2->pressure, spa2->temperature,
                                                         spa2->atmos_refract, spa2->e0);
        spa2->e       = topocentric_elevation_angle_corrected(spa2->e0, spa2->del_e);

        spa2->zenith     = topocentric_zenith_angle(spa2->e);
        spa2->azimuth180 = topocentric_azimuth_angle_neg180_180(spa2->h_prime, spa2->latitude,
                                                                             spa2->delta_prime);
        spa2->azimuth    = topocentric_azimuth_angle_zero_360(spa2->azimuth180);

        if ((spa2->function == SPA_ZA_INC) || (spa2->function == SPA_ALL))
            spa2->incidence  = surface_incidence_angle(spa2->zenith, spa2->azimuth180,
                                                      spa2->azm_rotation, spa2->slope);

        //Second, do calculation for Sun center as normal
        spa->h  = observer_hour_angle(spa->nu, spa->longitude, spa->alpha);
        spa->xi = sun_equatorial_horizontal_parallax(spa->r);

        sun_right_ascension_parallax_and_topocentric_dec(spa->latitude, spa->elevation, spa->xi,
                                    spa->h, spa->delta, &(spa->del_alpha), &(spa->delta_prime));

        spa->alpha_prime = topocentric_sun_right_ascension(spa->alpha, spa->del_alpha);
        spa->h_prime     = topocentric_local_hour_angle(spa->h, spa->del_alpha);

        spa->e0      = topocentric_elevation_angle(spa->latitude, spa->delta_prime, spa->h_prime);
        spa->del_e   = atmospheric_refraction_correction(spa->pressure, spa->temperature,
                                                         spa->atmos_refract, spa->e0);
        spa->e       = topocentric_elevation_angle_corrected(spa->e0, spa->del_e);

        spa->zenith     = topocentric_zenith_angle(spa->e);
        spa->azimuth180 = topocentric_azimuth_angle_neg180_180(spa->h_prime, spa->latitude,
                                                                             spa->delta_prime);
        spa->azimuth    = topocentric_azimuth_angle_zero_360(spa->azimuth180);

        if ((spa->function == SPA_ZA_INC) || (spa->function == SPA_ALL))
            spa->incidence  = surface_incidence_angle(spa->zenith, spa->azimuth180,
                                                      spa->azm_rotation, spa->slope);

        //Don't bother with this calculation for the solar north pole
        if ((spa->function == SPA_ZA_RTS) || (spa->function == SPA_ALL))
            calculate_eot_and_sun_rise_transit_set(spa);
    }

    return result;
}
