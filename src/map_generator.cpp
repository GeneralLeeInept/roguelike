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
    if ((r1.tlc.x - r2.brc.x) > 1 || (r2.tlc.x - r1.brc.x) > 1)
    {
        // No overlap in x
        return false;
    }

    if ((r1.tlc.y - r2.brc.y) > 1 || (r2.tlc.y - r1.brc.y) > 1)
    {
        // No overlap in y
        return false;
    }

    return true;
}

static Point get_centre_point(const Rect& r)
{
    Point centre;
    centre.x = (r.tlc.x + r.brc.x) / 2;
    centre.y = (r.tlc.y + r.brc.y) / 2;
    return centre;
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

void BasicMapGenerator::configure(int max_rooms, int room_min_size, int room_max_size)
{
    _max_rooms = max_rooms;
    _room_min_size = room_min_size;
    _room_max_size = room_max_size;
}

void BasicMapGenerator::generate_map(int width, int height, MapDef& map_def)
{
    map_def.width = width;
    map_def.height = height;
    map_def.tiles.resize(width * height);

    // Fill map with walls
    for (auto& tile : map_def.tiles)
    {
        tile.type = TileType::Wall;
    }

    // Generate rooms
    std::vector<Rect> rooms;
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
        Rect room = { { x - w / 2, y - h / 2 }, { x + w, y + h } };

        if (room.tlc.x < 1 || room.tlc.y < 1 || room.brc.x >= width - 1 || room.brc.y >= height - 1)
        {
            continue;
        }
        else
        {
            bool keep = true;

            for (auto& r : rooms)
            {
                if (overlap(r, room))
                {
                    keep = false;
                    break;
                }
            }

            if (!keep)
            {
                continue;
            }

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
        Point start = get_centre_point(rooms[n - 1]);
        Point end = get_centre_point(rooms[n]);
        dig_tunnel(start, end, map_def);
    }

    // Set player start position in first room
    Point spawn = get_centre_point(rooms[0]);
    map_def.spawn_x = spawn.x;
    map_def.spawn_y = spawn.y;
}

void FovTestMapGenerator::generate_map(int width, int height, MapDef& map_def)
{
    map_def.width = width;
    map_def.height = height;
    map_def.tiles.resize(width * height);

    for (auto& t : map_def.tiles)
    {
        t.type = TileType::Wall;
    }

    map_def.spawn_x = width / 2;
    map_def.spawn_y = height / 2;

    map_def.tiles[map_def.spawn_x + map_def.spawn_y * width].type = TileType::Floor;
}
