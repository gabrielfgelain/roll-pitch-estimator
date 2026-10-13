#pragma once
#include <cstdint>

struct AccelSample
{
    std::uint32_t time_stamp_ms{};
    int accel_x_mg{};
    int accel_y_mg{};
    int accel_z_mg{};
};
