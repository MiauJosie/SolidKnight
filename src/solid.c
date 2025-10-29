#include "solid.h"
#include "level.h"
#include <allegro5/allegro5.h>

Solid *solid_create(Level *level, Vector2 position, int width, int height, bool is_collidable)
{
    Solid *solid = malloc(sizeof(Solid));
    if (solid == NULL)
    {
        return NULL;
    }

    solid->level = level;
    solid->position = position;
    solid->width = width;
    solid->height = height;
    solid->is_collidable = is_collidable;
    solid->sprite = NULL;

    return solid;
}

void solid_destroy(Solid *solid)
{
    if (solid == NULL)
    {
        return;
    }

    free(solid);
}

Rectangle solid_get_bounds(Solid *solid)
{
    Rectangle bounds;

    bounds.left = (int)solid->position.x;
    bounds.top = (int)solid->position.y;
    bounds.right = (int)solid->position.x + solid->width;
    bounds.bottom = (int)solid->position.y + solid->height;

    return bounds;
}

void solid_draw(Solid *solid)
{
    if (solid->sprite == NULL)
    {
        return;
    }

    // Use float positions to prevent flickering with camera movement
    al_draw_bitmap_region(solid->sprite, solid->source_x, solid->source_y, TILE_SIZE, TILE_SIZE, solid->position.x, solid->position.y, 0);
}