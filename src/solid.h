#pragma once
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include "types.h"

typedef struct Level Level;

typedef struct
{
    Vector2 position;
    int width;
    int height;
    Level *level;

    ALLEGRO_BITMAP *sprite;
    bool is_collidable;
} Solid;

Solid *solid_create(Level *level, Vector2 position, int width, int height, bool is_collidable);
void solid_destroy(Solid *solid);

Rectangle solid_get_bounds(Solid *solid);
void solid_draw(Solid *solid);