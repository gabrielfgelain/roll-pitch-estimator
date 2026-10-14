#include <iostream>
#include "AttitudeEstimator.h"



int main()
{
    AccelSample sample {54741, 434, 12, -898};
    const Attitude attitude = estimateAttitude(sample);

    std::cout << "roll = " << attitude.roll_deg << " deg\n";
    std::cout << "pitch = " << attitude.pitch_deg << " deg\n";

    return 0;
}

