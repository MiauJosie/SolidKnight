#pragma once
#include "actor.h"
#include <allegro5/allegro5.h>

// OMGGG COMPOSITION MENTIONED!
typedef struct
{
    Actor *actor;

    Vector2 velocity;

    bool is_on_ground;
    float time_left_ground;

    bool jump_to_consume;
    bool ended_jump_early;
    float time_jump_pressed;

    float time;
} Player;

Player *player_create(Level *level, Vector2 position);
void player_destroy(Player *player);
void player_update(Player *player, ALLEGRO_KEYBOARD_STATE *keys, float delta_time);
void player_draw(Player *player);

void handle_input(Player *player, ALLEGRO_KEYBOARD_STATE *keys);
void handle_gravity(Player *player, float delta_time);
void handle_movement(Player *player, ALLEGRO_KEYBOARD_STATE *keys, float delta_time);
void handle_jump(Player *player, ALLEGRO_KEYBOARD_STATE *keys);
void check_collision(Player *player);
void apply_movement(Player *player, float delta_time);