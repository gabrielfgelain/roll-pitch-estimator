#pragma once
#include <cstdint>

// One raw accelerometer reading:
//   <time_stamp_ms; accel_x_axis; accel_y_axis; accel_z_axis>
// Source: MMA8451 accelerometer, full scale +/-8g, values already converted to mg.
// Axes: x forward, y right, z down. A level, stationary sensor reads about -1000 mg on z.
struct AccelSample
{
    std::uint32_t time_stamp_ms{};
    int accel_x_mg{};
    int accel_y_mg{};
    int accel_z_mg{};
};
