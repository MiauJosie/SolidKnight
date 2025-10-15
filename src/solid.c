#include "solid.h"
#include <allegro5/allegro5.h>

Solid *solid_create(Level *level, Vector2 position, int width, int height, bool is_collidable)
{
    Solid *solid = malloc(sizeof(Solid));
    if (!solid)
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
    if (solid)
    {
        free(solid);
    }
}

Rectangle solid_get_bounds(Solid *solid)
{
    Rectangle bounds;

    bounds.left = solid->position.x;                   // top_left_x
    bounds.top = solid->position.y;                    // top_left_y
    bounds.right = solid->position.x + solid->width;   // width
    bounds.bottom = solid->position.y + solid->height; // height

    return bounds;
}

void solid_draw(Solid *solid)
{
    if (solid->sprite == NULL)
    {
        return;
    }

    al_draw_bitmap(solid->sprite, solid->position.x, solid->position.y, 0);
}