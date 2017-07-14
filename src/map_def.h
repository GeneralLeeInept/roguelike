#pragma once

struct TileDef
{
    int type;
};

struct MapDef
{
    int width;
    int height;
    TileDef* tiles;
};
