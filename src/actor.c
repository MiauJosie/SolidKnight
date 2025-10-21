#include "actor.h"
#include "level.h"
#include <stdio.h>

Actor *actor_create(Level *level, Vector2 position, int width, int height)
{
    Actor *actor = malloc(sizeof(Actor));
    if (!actor)
    {
        return NULL;
    }

    actor->level = level;
    actor->position = position;
    actor->width = width;
    actor->height = height;
    actor->is_collidable = true;
    actor->sprite = NULL;
    actor->is_facing_right = true;

    return actor;
}

void actor_destroy(Actor *actor)
{
    if (actor)
    {
        free(actor);
    }
}

Rectangle actor_get_bounds(Actor *actor)
{
    Rectangle bounds;

    bounds.left = (int)actor->position.x;
    bounds.top = (int)actor->position.y;
    bounds.right = (int)actor->position.x + actor->width;
    bounds.bottom = (int)actor->position.y + actor->height;

    return bounds;
}

// Ele fala: "Colidiu aqui?!?!"
bool actor_collide_at(Actor *actor, Vector2 position)
{

    if (actor->level == NULL)
    {
        return false;
    }

    Vector2 original_pos = actor->position;
    actor->position = position;
    Rectangle beiradinha_do_ator = actor_get_bounds(actor);
    actor->position = original_pos;

    Solid **solids = level_get_solids(actor->level);
    int solid_count = level_get_solid_count(actor->level);

    for (int i = 0; i < solid_count; i++)
    {
        Rectangle solid_bounds = solid_get_bounds(solids[i]);

        if (solids[i]->is_collidable && rectangles_intersect(beiradinha_do_ator, solid_bounds))
        {
            return true;
        }
    }

    // You shall not move!
    return false;
}

// Remou, puxou, passou... ?
bool actor_move_x(Actor *actor, float amount)
{
    int pixels = (int)amount;
    int sign = (int)sign_here_please(pixels);

    bool hit_something = false;

    for (int i = 0; i < abs(pixels); i++)
    {
        Vector2 test_pos = {actor->position.x + sign, actor->position.y};

        if (!actor_collide_at(actor, test_pos))
        {
            actor->position.x += sign;
        }
        else
        {
            hit_something = true;
            break;
        }
    }

    return hit_something = true;
}

bool actor_move_y(Actor *actor, float amount)
{
    int pixels = (int)amount;
    int sign = (int)sign_here_please(pixels);
    bool hit_something = false;

    for (int i = 0; i < abs(pixels); i++)
    {
        Vector2 test_pos = {actor->position.x, actor->position.y + sign};

        if (!actor_collide_at(actor, test_pos))
        {
            actor->position.y += sign;
        }
        else
        {
            hit_something = true;
            break;
        }
    }

    return hit_something;
}

void actor_set_facing(Actor *actor, bool value)
{
    actor->is_facing_right = value;
}

void actor_draw(Actor *actor)
{
    if (actor->sprite == NULL)
    {
        return;
    }

    int flags = actor->is_facing_right ? 0 : ALLEGRO_FLIP_HORIZONTAL;
    al_draw_bitmap(actor->sprite, (int)actor->position.x, (int)actor->position.y, flags);
}