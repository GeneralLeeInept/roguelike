#include "random.h"

#include <BearLibTerminal.h>

Random::Random(int range)
    : Random(1, range)
{
}

Random::Random(int min, int max)
    : _distribution(min, max)
    , _generator(std::random_device{}())
{
}

int Random::operator()()
{
    return _distribution(_generator);
}

static std::normal_distribution<>::param_type calculate_mean_sigma(int sides)
{
    double dsides = static_cast<double>(sides);
    double mean = 1.0 + dsides * 0.5;
    double variance = 0.0;

    for (int i = 0; i < sides; i++)
    {
        double distance_to_mean = i + 1.0 - mean;
        variance += (distance_to_mean * distance_to_mean);
    }

    variance = variance / dsides;

    double sigma = std::sqrt(variance);

    return std::normal_distribution<>::param_type(mean, sigma);
}

Roller::Roller(int sides)
    : _distribution(calculate_mean_sigma(sides))
    , _generator(std::random_device{}())
    , _sides(sides)
{
}

int Roller::operator()()
{
    int val = 0;
    
    while (val < 1 || val > _sides)
    {
        val = static_cast<int>(std::floor(_distribution(_generator)));
    } 

    return val;
}
