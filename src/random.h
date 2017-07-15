#pragma once
#include <algorithm>
#include <chrono>
#include <map>
#include <random>
#include <string>

class Random
{
public:
    Random(int range);
    Random(int min, int max);

    // Generate a random number on the interval [min, max]
    int operator()();

private:
    std::uniform_int_distribution<> _distribution;
    std::mt19937 _generator;
};

class Roller
{
public:
    Roller(int sides);

    // Generate a random number on the interval [1, sides]
    int operator()();

private:
    std::normal_distribution<> _distribution;
    std::mt19937 _generator;
    int _sides;
};

template <typename T_Random>
void test_random(int range)
{
    T_Random random(range);

    terminal_clear();

    std::map<int, int> histogram;
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    start = std::chrono::high_resolution_clock::now();

    for (int n = 0; n < 10000; ++n)
    {
        ++histogram[random()];
    }

    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    terminal_printf(1, 1, "Rolling 10,000 dice took: %fs", elapsed);

    int y = 3;
    int max_lines = terminal_state(TK_HEIGHT) - y - 1;
    int stride = (range / max_lines) + 1;
    for (int n = 1; n < range + 1; n += stride)
    {
        terminal_printf(1, y++, "%2d: %5d %s", n, histogram[n], std::string(histogram[n] / (1000 / range), '*').c_str());
    }

    terminal_refresh();

    while (TK_ESCAPE != terminal_read())
        ;
}
