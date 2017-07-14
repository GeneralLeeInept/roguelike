#include "random.h"

#include <BearLibTerminal.h>

#include <algorithm>
#include <chrono>
#include <map>
#include <string>

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

int Roller::roll()
{
    int val = 0;
    
    while (val < 1 || val > _sides)
    {
        val = static_cast<int>(std::floor(_distribution(_generator)));
    } 

    return val;
}

void Roller::test()
{
    terminal_clear();

    std::map<int, int> histogram;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();

    for (int n = 0; n < 10000; ++n)
    {
        ++histogram[roll()];
    }

    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    terminal_printf(1, 1, "Rolling 10,000 dice took: %fs", elapsed);

    int y = 3;
    int max_lines = terminal_state(TK_HEIGHT) - y - 1;
    int stride = (_sides / max_lines) + 1;
    for (int n = 1; n < _sides + 1; n += stride)
    {
        terminal_printf(1, y++, "%2d: %5d %s", n, histogram[n], std::string(histogram[n] / (1000 / _sides), '*').c_str());
    }

    terminal_refresh();

    while (TK_ESCAPE != terminal_read());
}
