#pragma once
#include "AttitudeEstimator.h"

#include <string>
#include <vector>

// Writes one line per result: time_stamp_ms;roll_deg;pitch_deg (with a header line).
// Throws std::runtime_error if the file cannot be opened or written.
void writeResults(const std::string &path, const std::vector<TimedAttitude> &results);