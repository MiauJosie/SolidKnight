#pragma once
#include "actor.h"
#include <allegro5/allegro5.h>

// OMGGG COMPOSITION MENTIONED!
typedef struct
{
    Actor *actor;

    Vector2 velocity;
    float pallas_time;
    float pallas_counter;
    float jump_buffer_time;
    float jump_buffer_counter;
    bool is_on_ground;
    bool is_jump_held;
} Player;

Player *player_create(Level *level, Vector2 position);
void player_destroy(Player *player);
void player_update(Player *player, ALLEGRO_KEYBOARD_STATE *keys);
void player_draw(Player *player);
void handle_gravity(Player *player);
void handle_movement(Player *player, ALLEGRO_KEYBOARD_STATE *keys);
void handle_jump(Player *player, ALLEGRO_KEYBOARD_STATE *keys);