#pragma once
#include "actor.h"
#include <allegro5/allegro5.h>

// OMGGG COMPOSITION MENTIONED!
typedef struct
{
    Actor *actor;

    Vector2 velocity;
    float speed;
} Player;

Player *player_create(Level *level, Vector2 position);
void player_destroy(Player *player);
void player_update(Player *player, ALLEGRO_KEYBOARD_STATE *keys);
void player_draw(Player *player);