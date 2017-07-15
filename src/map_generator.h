#pragma once

#include <utility>
#include <vector>

#include "random.h"

struct MapDef;

class MapGenerator
{
public:
    virtual ~MapGenerator() {}

    virtual void generate_map(int width, int height, MapDef &map_def) = 0;
};

class BasicMapGenerator : MapGenerator
{
public:
    void generate_map(int width, int height, MapDef &map_def) override;
};