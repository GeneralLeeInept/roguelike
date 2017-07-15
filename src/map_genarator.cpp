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

void BasicMapGenerator::configure(int num_rooms, int room_min_size, int room_max_size)
{
    _num_rooms = num_rooms;
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

    while (rooms.size() < _num_rooms)
    {
        // Add a room
        int fails = 0;
        bool abort = false;
        bool keep = false;

        while (!keep && !abort)
        {
            keep = true;

            if (++fails > 100)
            {
                abort = true;
                break;
            }

            int w = room_size();
            int h = room_size();
            int x = room_origin_x();
            int y = room_origin_y();

            Rect room = { { x - w / 2, y - h / 2 }, { x + w, y + h } };

            if (room.tlc.x < 1 || room.tlc.y < 1 || room.brc.x >= width - 1 || room.brc.y >= height - 1)
            {
                keep = false;
                continue;
            }
            else
            {
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

        if (abort)
        {
            // Failed to create a room too many times, start over
            rooms.clear();
        }
    }

    // Dig out rooms
    for (auto& r : rooms)
    {
        dig_room(r, map_def);
    }

    // Connect rooms
    for (int n = 1; n < _num_rooms; ++n)
    {
        Point start = { (rooms[n - 1].tlc.x + rooms[n - 1].brc.x) / 2, (rooms[n - 1].tlc.y + rooms[n - 1].brc.y) / 2 };
        Point end = { (rooms[n].tlc.x + rooms[n].brc.x) / 2, (rooms[n].tlc.y + rooms[n].brc.y) / 2 };
        dig_tunnel(start, end, map_def);
    }

    // Set player start position in first room
    map_def.spawn_x = (rooms[0].tlc.x + rooms[0].brc.x) / 2;
    map_def.spawn_y = (rooms[0].tlc.y + rooms[0].brc.y) / 2;
}
