#pragma once

#include <utility>
#include <vector>

#include "geometry.h"

struct MapDef;

struct MapGeneratorParameters
{
    Point map_size;
    int room_max;
    Point room_size_range;
    int room_min_spacing;
    int monsters_max_per_room;
};

class MapGenerator
{
public:
    virtual ~MapGenerator() {}

    virtual void generate_map(const MapGeneratorParameters& parameters, MapDef &map_def) = 0;
};

class BasicMapGenerator : MapGenerator
{
public:
    void generate_map(const MapGeneratorParameters& parameters, MapDef &map_def) override;
};

class TestMapGenerator : MapGenerator
{
public:
    void generate_map(const MapGeneratorParameters& parameters, MapDef &map_def) override;
};