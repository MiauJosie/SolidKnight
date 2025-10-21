#include "player.h"
#include "level.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <allegro5/allegro5.h>

#define MAX_SPEED 10.0f
#define SPEED 2.5f
#define JUMP_FORCE -4.0f
#define JUMP_X_BOOST 3.0f
#define GRAVITY_FORCE 0.4f
#define AIR_ACCEL 0.2f
#define GROUND_ACCEL 0.3f
#define GROUND_FRICTION 0.4f
#define JUMP_HELD_GRAV_MULT 0.6f
#define DEFAULT_GRAV_MULT 1.0f
#define PEAK_GRAV_MULT 2.0f
#define INIT_GRAV_MULT 0.8f
#define FAST_FALL_GRAV_MULT 1.3f

Player *player_create(Level *level, Vector2 position)
{
    Player *player = malloc(sizeof(Player));
    if (!player)
    {
        return NULL;
    }

    player->actor = actor_create(level, position, 8, 14);
    if (!player->actor)
    {
        free(player);
        return NULL;
    }

    player->actor->sprite_width = 48;
    player->actor->sprite_height = 48;

    player->actor->sprite_offset.x = -20;
    player->actor->sprite_offset.y = -18;

    player->velocity = (Vector2){0, 0};
    player->is_on_ground = false;
    player->is_jump_held = false;
    player->pallas_time = 6.0f;
    player->pallas_counter = 0;
    player->jump_buffer_time = 6.0f;
    player->jump_buffer_counter = 0;

    return player;
}

void player_update(Player *player, ALLEGRO_KEYBOARD_STATE *keys)
{
    bool was_on_ground = player->is_on_ground;

    handle_movement(player, keys);
    handle_jump(player, keys);
    handle_gravity(player);

    actor_move_x(player->actor, player->velocity.x);
    bool hit_something = actor_move_y(player->actor, player->velocity.y);

    if (hit_something && player->velocity.y > 0)
    {
        player->is_on_ground = true;
        player->velocity.y = 0;
    }
    else if (hit_something && player->velocity.y < 0)
    {
        player->velocity.y = 0;
        player->is_on_ground = false;
    }
    else
    {
        player->is_on_ground = false;
    }

    if (was_on_ground && !player->is_on_ground)
    {
        player->pallas_counter = player->pallas_time;
    }
    else if (!player->is_on_ground)
    {
        player->pallas_counter -= 1.0f;
    }
}

void handle_movement(Player *player, ALLEGRO_KEYBOARD_STATE *keys)
{
    float target_velocity_x = 0;

    if (al_key_down(keys, ALLEGRO_KEY_LEFT) || al_key_down(keys, ALLEGRO_KEY_A))
    {
        target_velocity_x = -SPEED;
        actor_set_facing(player->actor, false);
    }

    if (al_key_down(keys, ALLEGRO_KEY_RIGHT) || al_key_down(keys, ALLEGRO_KEY_D))
    {
        target_velocity_x = SPEED;
        actor_set_facing(player->actor, true);
    }

    float acceleration = player->is_on_ground ? GROUND_ACCEL : AIR_ACCEL;

    if (target_velocity_x == 0 && player->is_on_ground)
    {
        float friction = GROUND_FRICTION;
        if (fabs(player->velocity.x) > friction)
        {
            player->velocity.x -= sign_here_please(player->velocity.x) * friction;
        }
        else
        {
            player->velocity.x = 0;
        }
    }
    else
    {
        float max_speed_change = acceleration;
        float velocity_diff = target_velocity_x - player->velocity.x;
        float actual_change = get_clamped(velocity_diff, -max_speed_change, max_speed_change);

        player->velocity.x += actual_change;
    }
}

void handle_jump(Player *player, ALLEGRO_KEYBOARD_STATE *keys)
{
    bool jump_pressed = al_key_down(keys, ALLEGRO_KEY_W);

    if (jump_pressed && !player->is_jump_held)
    {
        player->jump_buffer_counter = player->jump_buffer_time;
    }
    else
    {
        player->jump_buffer_counter -= 1.0f;
    }

    if (player->jump_buffer_counter > 0 && (player->is_on_ground || player->pallas_counter > 0))
    {
        player->velocity.y = JUMP_FORCE;

        player->jump_buffer_counter = 0;
        player->pallas_counter = 0;
        player->is_on_ground = false;
    }

    if ((player->jump_buffer_counter > 0 && (player->is_on_ground || player->pallas_counter > 0)) && (al_key_down(keys, ALLEGRO_KEY_D) || al_key_down(keys, ALLEGRO_KEY_A)))
    {
        player->velocity.x = sign_here_please(player->velocity.x) * JUMP_X_BOOST;
    }

    if (!jump_pressed && player->velocity.y < 0)
    {
        player->velocity.y = fmax(player->velocity.y, -3.0f);
    }

    player->is_jump_held = jump_pressed;
}

// A ascenção e a queda...
void handle_gravity(Player *player)
{
    if (!player->is_on_ground)
    {
        float gravity_multiplier = 1.0f;

        if (player->velocity.y < 0)
        {
            gravity_multiplier = player->is_jump_held ? JUMP_HELD_GRAV_MULT : PEAK_GRAV_MULT;
        }
        else
        {
            if (player->velocity.y < 1.5f)
            {
                gravity_multiplier = INIT_GRAV_MULT;
            }
            else if (player->velocity.y > 1.5f && player->velocity.y < 6.0)
            {
                gravity_multiplier = DEFAULT_GRAV_MULT;
            }
            else if (player->velocity.y > 6.0f)
            {
                gravity_multiplier = FAST_FALL_GRAV_MULT;
            }
        }

        float gravity = GRAVITY_FORCE * gravity_multiplier;
        player->velocity.y = get_clamped(player->velocity.y + gravity, -MAX_SPEED, MAX_SPEED);
    }
    else
    {
        if (player->velocity.y > 0)
        {
            player->velocity.y = 0;
        }
    }
}

void player_draw(Player *player)
{
    actor_draw(player->actor);
}

void player_destroy(Player *player)
{
    if (player)
    {
        actor_destroy(player->actor);
        free(player);
    }
}