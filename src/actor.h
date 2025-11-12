#pragma once
#include "types.h"
#include "animator.h"
#include <allegro5/allegro5.h>

typedef struct Level Level;

typedef struct
{
    Vector2 position;
    Vector2 remainder;
    int width;
    int height;
    Level *level;
    Vector2 sprite_offset;
    ALLEGRO_BITMAP *sprite; // Não usamos mais

    Animator *animator; // Animator is the new sprite!

    bool is_facing_right;
    bool is_collidable;
} Actor;

Actor *actor_create(Level *level, Vector2 position, int with, int height);
void actor_destroy(Actor *actor);
void actor_move_x(Actor *actor, float amount);
void actor_move_y(Actor *actor, float amount);
void actor_update(Actor *actor, float delta_time);
void actor_draw(Actor *actor);
Rectangle actor_get_bounds(Actor *actor);
bool actor_collide_at(Actor *actor, Vector2 position);
void actor_set_facing(Actor *actor, bool value);