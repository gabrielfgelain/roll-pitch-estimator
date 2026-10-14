#include "LogParser.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace
{
    // Warnings printed before switching to a summary, to avoid flooding the console.
    constexpr int max_warnings = 10;

    // Blank lines (e.g. an empty last line, or a lone '\r' from CRLF files) are not errors.
    bool isBlank(const std::string &line)
    {
        return line.find_first_not_of(" \t\r") == std::string::npos;
    }
}

std::optional<AccelSample> parseLine(const std::string &line)
{
    std::istringstream iss(line);

    // Read the timestamp as signed 64-bit so negative values can be detected.
    std::int64_t time_stamp_ms{};
    int x{}, y{}, z{};
    char semicolon1{}, semicolon2{}, semicolon3{};
    iss >> time_stamp_ms >> semicolon1 >> x >> semicolon2 >> y >> semicolon3 >> z;

    if (!iss || (semicolon1 != ';' || semicolon2 != ';' || semicolon3 != ';'))
    {
        return std::nullopt;
    }

    // operator >> skips whitespace (including '\r'), so reading anything else
    // means the line has extra fields or garbage.
    char extra{};
    if (iss >> extra)
    {
        return std::nullopt;
    }

    if (time_stamp_ms < 0 || time_stamp_ms > std::numeric_limits<std::uint32_t>::max())
    {
        return std::nullopt;
    }

    return AccelSample{static_cast<std::uint32_t>(time_stamp_ms), x, y, z};
}

std::vector<AccelSample> parseLogFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: '" + path + "'");
    }

    std::vector<AccelSample> samples;
    std::string line;
    int line_number = 0;
    int warnings = 0;

    while (std::getline(file, line))
    {
        ++line_number;

        if (isBlank(line))
        {
            continue;
        }

        const std::optional<AccelSample> sample = parseLine(line);
        if (!sample)
        {
            ++warnings;
            if (warnings <= max_warnings)
            {
                std::cerr << "Warning: line " << line_number << " skipped (invalid format)\n";
            }
            continue;
        }

        samples.push_back(*sample);
    }

    if (warnings > max_warnings)
    {
        std::cerr << "Warning: " << (warnings - max_warnings) << " more lines skipped (invalid format)\n";
    }

    if (samples.empty())
    {
        throw std::runtime_error("no valid samples found in '" + path + "'");
    }

    return samples;
}
