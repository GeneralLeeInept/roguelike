#include "map_generator.h"

#include <algorithm>

#include "map_def.h"

struct Point
{
    int x, y;
};

struct Rect
{
    Point tlc, brc;
};

static bool overlap(const Rect& r1, const Rect& r2)
{
    if (r1.tlc.x > r2.brc.x || r1.brc.x < r2.tlc.x)
    {
        // No overlap in x
        return false;
    }

    if (r1.tlc.y > r2.brc.y || r1.brc.y < r2.tlc.y)
    {
        // No overlap in y
        return false;
    }

    return true;
}

static void dig_room(const Rect& room, MapDef& map_def)
{
    for (int y = room.tlc.y; y <= room.brc.y; ++y)
    {
        for (int x = room.tlc.x; x <= room.brc.x; ++x)
        {
            map_def.tiles[x + y * map_def.width].type = TileType::Floor;
        }
    }
}

static void dig_tunnel(const Point& start, const Point& end, MapDef& map_def)
{
    if (start.x == end.x)
    {
        int ymin = std::min(start.y, end.y);
        int ymax = std::max(start.y, end.y);

        for (int y = ymin; y <= ymax; ++y)
        {
            map_def.tiles[start.x + y * map_def.width].type = TileType::Floor;
        }
    }
    else if (start.y == end.y)
    {
        int xmin = std::min(start.x, end.x);
        int xmax = std::max(start.x, end.x);

        for (int x = xmin; x <= xmax; ++x)
        {
            map_def.tiles[x + start.y * map_def.width].type = TileType::Floor;
        }
    }
    else
    {
        int tx = start.x;
        int ty = start.y;
        int dx = start.x < end.x ? 1 : -1;
        int dy = start.y < end.y ? 1 : -1;

        // Dig horizontal to middle x point
        int mx = (start.x + end.x) / 2;

        for (; tx != mx; tx += dx)
        {
            map_def.tiles[tx + ty * map_def.width].type = TileType::Floor;
        }

        // Dig vertical from start y to end y
        for (; ty != end.y; ty += dy)
        {
            map_def.tiles[tx + ty * map_def.width].type = TileType::Floor;
        }

        // Dig horizontal to end x
        for (; tx != end.x; tx += dx)
        {
            map_def.tiles[tx + ty * map_def.width].type = TileType::Floor;
        }
    }
}

void BasicMapGenerator::generate_map(int width, int height, MapDef& map_def)
{
    Roller random(100);
    std::vector<Rect> _rooms;
    map_def.width = width;
    map_def.height = height;
    map_def.tiles.resize(width * height);

    // Fill map with walls
    for (auto& tile : map_def.tiles)
    {
        tile.type = TileType::Wall;
    }

    // Dig out some rooms
    dig_room({ { 20, 5 }, { 30, 20 } }, map_def);
    dig_room({ { 50, 5 }, { 60, 20 } }, map_def);
    dig_tunnel({ 25, 12 }, { 55, 12 }, map_def);

    // Set player start position in first room
    map_def.spawn_x = 25;
    map_def.spawn_y = 12;
}
