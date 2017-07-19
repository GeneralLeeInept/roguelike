#include "map_generator.h"

#include <algorithm>

#include "map_def.h"
#include "random.h"

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

void BasicMapGenerator::generate_map(const MapGeneratorParameters& parameters, MapDef& map_def)
{
    map_def.size = parameters.map_size;
    size_t num_tiles = parameters.map_size.x * parameters.map_size.y;
    map_def.tiles.resize(num_tiles, { TileType::Wall });

    // Generate rooms
    Random random;
    std::vector<Rectangle> rooms;

    for (int n = 0; n < parameters.room_max; ++n)
    {
        // Add a room
        int w = random(parameters.room_size_range.x, parameters.room_size_range.y);
        int h = random(parameters.room_size_range.x, parameters.room_size_range.y);
        int x = random(parameters.room_size_range.x / 2 + 1, parameters.map_size.x - parameters.room_size_range.x / 2 - 1);
        int y = random(parameters.room_size_range.x / 2 + 1, parameters.map_size.y - parameters.room_size_range.x / 2 - 1);
        Rectangle room = Rectangle::intersection(Rectangle(Point(1, 1), parameters.map_size - Point(1, 1)), Rectangle(Point(x, y), w, h));
        bool keep = true;

        for (auto& r : rooms)
        {
            if (!Rectangle::separated(r, room, parameters.room_min_spacing))
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

    // Add actors in remaining rooms
    for (size_t n = 1; n < rooms.size(); ++n)
    {
        for (int m = 0; m < parameters.monsters_max_per_room; ++m)
        {
            MapDef::Actor monster = {};
            monster.spawn_pos = random(rooms[n].mins + Point(1, 1), rooms[n].maxs - Point(1,1));
            int monster_type = random(1, 100);

            if (monster_type < 51)
            {
                // 50% of the time no monster
                continue;
            }
            else if (monster_type < 91)
            {
                // 40% of the time a weak monster
                monster.type = ActorType::Weak;
            }
            else
            {
                // 10% of the time a strong monster
                monster.type = ActorType::Strong;
            }

            map_def.actors.push_back(monster);
        }
    }
}
