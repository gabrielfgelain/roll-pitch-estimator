#include "AttitudeEstimator.h"
#include <cmath>

namespace
{
    constexpr double pi = 3.14159265358979323846;

    double radToDeg(double angle_rad)
    {
        return angle_rad * 180.0 / pi;
    }
}

Attitude estimateAttitude(const AccelSample& sample)
{
    const double gx = static_cast<double>(sample.accel_x_mg);
    const double gy = static_cast<double>(sample.accel_y_mg);
    const double gz = static_cast<double>(sample.accel_z_mg);

    // AN3461 eq. 25/26, evaluated with atan2 to get the correct quadrant (AN3461 p. 12),
    // using its sign convention Gp = -raw.
    const double roll_rad = std::atan2(-gy, -gz);
    const double pitch_rad = std::atan2(gx, std::sqrt(gy * gy + gz * gz));

    // create an Attitude struct in degrees
    Attitude attitude{};
    attitude.roll_deg = radToDeg(roll_rad);
    attitude.pitch_deg = radToDeg(pitch_rad);

    return attitude;
}
