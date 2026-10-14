#include "AttitudeEstimator.h"
#include "LogParser.h"
#include "ResultWriter.h"

#include <exception>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input.log> [output.csv]\n";
        return 1;
    }

    const std::string input_path = argv[1];
    const std::string output_path = (argc >= 3) ? argv[2] : "attitude_output.csv";

    try
    {
        const std::vector<AccelSample> samples = parseLogFile(input_path);

        std::vector<TimedAttitude> results;
        results.reserve(samples.size());
        for (const AccelSample &sample : samples)
        {
            results.push_back({sample.time_stamp_ms, estimateAttitude(sample)});
        }

        writeResults(output_path, results);
        std::cout << "Input:   " << input_path << '\n'
                  << "Samples: " << samples.size() << '\n'
                  << "Output:  " << output_path << '\n'
                  << "Columns: time_stamp_ms (ms); roll_deg, pitch_deg (degrees, "
                     "aerospace sequence, roll [-180, 180], pitch [-90, 90])\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}