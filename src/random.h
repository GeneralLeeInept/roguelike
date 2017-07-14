#pragma once
#include <random>

class Roller
{
public:
    Roller(int sides);

    int roll();

    void test();

private:
    std::normal_distribution<> _distribution;
    std::mt19937 _generator;
    int _sides;
};
