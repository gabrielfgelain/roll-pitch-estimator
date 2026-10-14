#pragma once
#include "Sample.h"

// Vehicle attitude in degrees (aerospace rotation sequence).
// Roll range: [-180, 180]; pitch range: [-90, 90].
struct Attitude
{
    double roll_deg{};
    double pitch_deg{};
};

// Attitude estimate paired with the timestamp of the sample it came from.
struct TimedAttitude
{
    std::uint32_t time_stamp_ms{};
    Attitude attitude{};
};

// Estimates roll and pitch from a single accelerometer sample (NXP AN3461, eq. 25/26).
// Assumes no linear acceleration.
Attitude estimateAttitude(const AccelSample &sample);