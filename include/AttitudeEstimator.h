#pragma once
#include "Sample.h"

struct Attitude
{
    double roll_deg{};
    double pitch_deg{};
};

Attitude estimateAttitude(const AccelSample& sample);