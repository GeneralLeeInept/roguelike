#pragma once
#include <algorithm>
#include <chrono>
#include <map>
#include <random>
#include <string>

class Random
{
public:
    Random();

    // Generate a random number on the interval [min, max] with linear distribution.
    int operator()(int min, int max);

private:
    std::uniform_int_distribution<> _distribution;
    std::mt19937 _generator;
};

class Roller
{
public:
    Roller();

    // Roll some dice. Don't use this to roll one die, use Random for that.
    int operator()(int num_dice, int num_sides);

private:
    std::normal_distribution<> _distribution;
    std::mt19937 _generator;
};
