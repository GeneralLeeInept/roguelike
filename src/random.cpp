#include <BearLibTerminal.h>

#include "random.h"

Random::Random()
    : _distribution()
    , _generator(std::random_device{}())
{
}

int Random::operator()(int min, int max)
{
    std::uniform_int_distribution<>::param_type param(min, max);
    _distribution.param(param);
    return _distribution(_generator);
}

static std::normal_distribution<>::param_type calculate_mean_sigma(int num_dice, int num_sides)
{
    double mean = num_sides * (num_sides + 1.0) * 0.5;
    double variance = (num_sides * num_sides - 1.0) / 12.0;
    double sigma = std::sqrt(variance);
    return std::normal_distribution<>::param_type(mean, sigma);
}

Roller::Roller()
    : _distribution()
    , _generator(std::random_device{}())
{
}

int Roller::operator()(int num_dice, int num_sides)
{
    auto param = calculate_mean_sigma(num_dice, num_sides);
    _distribution.param(param);
    int val = 0;
    
    while (val < num_dice || val > num_dice * num_sides)
    {
        val = static_cast<int>(std::floor(_distribution(_generator)));
    } 

    return val;
}
