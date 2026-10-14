#include "LogParser.h"

#include <exception>
#include <iostream>
#include <vector>

int main()
{
    try
    {
        const std::vector<AccelSample> samples = parseLogFile("attitude_exam.log");
        std::cout << samples.size() << " samples read\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}