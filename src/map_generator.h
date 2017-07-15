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
    void configure(int max_rooms, int room_min_size, int room_max_size);

    void generate_map(int width, int height, MapDef &map_def) override;

private:
    int _max_rooms;
    int _room_min_size;
    int _room_max_size;
};