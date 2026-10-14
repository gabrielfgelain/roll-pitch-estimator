#include "ResultWriter.h"

#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace
{
    // Avoids printing "-0.00": IEEE 754 negative zero (e.g. atan2(-0.0, x) when y = 0)
    double normalizeZero(double value)
    {
        return (std::abs(value) < 0.005) ? 0.0 : value;
    }
}

void writeResults(const std::string &path, const std::vector<TimedAttitude> &results)
{
    std::ofstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: '" + path + "'");
    }

    file << std::fixed << std::setprecision(2);
    file << "time_stamp_ms;roll_deg;pitch_deg\n";

    for (const TimedAttitude &result : results)
    {
        file << result.time_stamp_ms << ';'
             << normalizeZero(result.attitude.roll_deg) << ';'
             << normalizeZero(result.attitude.pitch_deg) << '\n';
    }

    if (!file)
    {
        throw std::runtime_error("Failed to write to file: '" + path + "'");
    }
}