#include "map_generator.h"

#include <algorithm>

#include "geometry.h"
#include "map_def.h"

static void dig_room(const Rectangle& room, MapDef& map_def)
{
    for (int y = room.mins.y; y < room.maxs.y; ++y)
    {
        for (int x = room.mins.x; x < room.maxs.x; ++x)
        {
            map_def.tiles[x + y * map_def.size.x].type = TileType::Floor;
        }
    }
}

static void dig_tunnel(const Point& start, const Point& end, MapDef& map_def)
{
    int tx = start.x;
    int ty = start.y;
    int dx = start.x < end.x ? 1 : -1;
    int dy = start.y < end.y ? 1 : -1;

    // Dig horizontal to middle x point
    int mx = (start.x + end.x) / 2;

    for (; tx != mx; tx += dx)
    {
        map_def.tiles[tx + ty * map_def.size.x].type = TileType::Floor;
    }

    // Dig vertical from start y to end y
    for (; ty != end.y; ty += dy)
    {
        map_def.tiles[tx + ty * map_def.size.x].type = TileType::Floor;
    }

    // Dig horizontal to end x
    for (; tx != end.x; tx += dx)
    {
        map_def.tiles[tx + ty * map_def.size.x].type = TileType::Floor;
    }
}

void BasicMapGenerator::configure(int max_rooms, int room_min_size, int room_max_size)
{
    _max_rooms = max_rooms;
    _room_min_size = room_min_size;
    _room_max_size = room_max_size;
}

void BasicMapGenerator::generate_map(int width, int height, MapDef& map_def)
{
    map_def.size = Point(width, height);
    map_def.tiles.resize(width * height);

    // Fill map with walls
    for (auto& tile : map_def.tiles)
    {
        tile.type = TileType::Wall;
    }

    // Generate rooms
    std::vector<Rectangle> rooms;
    Random room_size(_room_min_size, _room_max_size);
    Random room_origin_x(_room_min_size / 2 + 1, width - _room_min_size / 2 + 1);
    Random room_origin_y(_room_min_size / 2 + 1, height - _room_min_size / 2 + 1);

    for (int n = 0; n < _max_rooms; ++n)
    {
        // Add a room
        int w = room_size();
        int h = room_size();
        int x = room_origin_x();
        int y = room_origin_y();
        Rectangle room = Rectangle::intersection(Rectangle(Point(1, 1), map_def.size - Point(1, 1)), Rectangle(Point(x, y), w, h));
        bool keep = true;

        for (auto& r : rooms)
        {
            if (!Rectangle::separated(r, room, 5))
            {
                keep = false;
                break;
            }
        }

        if (keep)
        {
            rooms.push_back(room);
        }
    }

    // Dig out rooms
    for (auto& r : rooms)
    {
        dig_room(r, map_def);
    }

    // Connect rooms
    for (size_t n = 1; n < rooms.size(); ++n)
    {
        Point start = rooms[n - 1].centre();
        Point end = rooms[n].centre();
        dig_tunnel(start, end, map_def);
    }

    // Set player start position in first room
    map_def.spawn_position = rooms[0].centre();
}
