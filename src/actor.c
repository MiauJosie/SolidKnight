#include "actor.h"
#include "level.h"

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

    bounds.left = actor->position.x;                   // top_left_x
    bounds.top = actor->position.y;                    // top_left_y
    bounds.right = actor->position.x + actor->width;   // width
    bounds.bottom = actor->position.y + actor->height; // height

    return bounds;
}

// Ele fala: "Colidiu aqui?!?!"
bool actor_collide_at(Actor *actor, Vector2 position)
{
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
void actor_move_x(Actor *actor, float amount)
{
    Vector2 test_pos = {actor->position.x + amount, actor->position.y};

    if (!actor_collide_at(actor, test_pos))
    {
        actor->position.x += amount;
    }
}

void actor_move_y(Actor *actor, float amount)
{
    Vector2 test_pos = {actor->position.x, actor->position.y + amount};

    if (!actor_collide_at(actor, test_pos))
    {
        actor->position.y += amount;
    }
}

void actor_draw(Actor *actor)
{
    if (actor->sprite == NULL)
    {
        return;
    }

    int flags = actor->is_facing_right ? 0 : ALLEGRO_FLIP_HORIZONTAL;
    al_draw_bitmap(actor->sprite, actor->position.x, actor->position.y, flags);
}