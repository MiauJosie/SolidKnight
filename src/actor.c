#include "actor.h"
#include "level.h"
#include <stdio.h>
#include <allegro5/allegro_primitives.h>

Actor *actor_create(Level *level, Vector2 position, int width, int height)
{
    Actor *actor = malloc(sizeof(Actor));
    if (actor == NULL)
    {
        return NULL;
    }

    actor->level = level;
    actor->position = position;
    actor->remainder = (Vector2){0, 0};
    actor->width = width;
    actor->height = height;
    actor->is_collidable = true;
    actor->sprite = NULL;
    actor->animator = animator_create();
    actor->is_facing_right = true;

    return actor;
}

void actor_destroy(Actor *actor)
{
    if (actor == NULL)
    {
        return;
    }

    animator_destroy(actor->animator);
    free(actor);
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
            // You shall collide!
            return true;
        }
    }

    // You shall not collide!
    return false;
}

// Remou, passou...
void actor_move_x(Actor *actor, float amount)
{
    actor->remainder.x += amount;
    int pixels = (int)actor->remainder.x;
    actor->remainder.x -= pixels;

    int sign = (int)sign_here_please(pixels);

    for (int i = 0; i < abs(pixels); i++)
    {
        Vector2 test_pos = {actor->position.x + sign, actor->position.y};

        if (!actor_collide_at(actor, test_pos))
        {
            actor->position.x += sign;
        }
        else
        {
            break;
        }
    }
}

void actor_move_y(Actor *actor, float amount)
{
    actor->remainder.y += amount;
    int pixels = (int)actor->remainder.y;
    actor->remainder.y -= pixels;

    int sign = (int)sign_here_please(pixels);

    for (int i = 0; i < abs(pixels); i++)
    {
        Vector2 test_pos = {actor->position.x, actor->position.y + sign};

        if (!actor_collide_at(actor, test_pos))
        {
            actor->position.y += sign;
        }
        else
        {
            break;
        }
    }
}

void actor_set_facing(Actor *actor, bool value)
{
    actor->is_facing_right = value;
}

void actor_update(Actor *actor, float delta_time)
{
    if (actor->animator == NULL)
    {
        return;
    }

    animator_update(actor->animator, delta_time);
}

void actor_draw(Actor *actor)
{
    if (actor->sprite == NULL)
    {
        return;
    }

    float sprite_x = actor->position.x + actor->sprite_offset.x;
    float sprite_y = actor->position.y + actor->sprite_offset.y;

    if (actor->animator != NULL && actor->animator->current_animation != NULL)
    {
        animator_draw(actor->animator, sprite_x, sprite_y, !actor->is_facing_right);
    }
    else
    {
        int flags = actor->is_facing_right ? 0 : ALLEGRO_FLIP_HORIZONTAL;
        al_draw_bitmap(actor->sprite, sprite_x, sprite_y, flags);
    }

    // al_draw_rectangle(actor->position.x, actor->position.y, actor->position.x + actor->width, actor->position.y + actor->height, al_map_rgb(255, 0, 0), 1.0f);
}