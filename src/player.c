#include "player.h"
#include "level.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <allegro5/allegro5.h>

Player *player_create(Level *level, Vector2 position)
{
    Player *player = malloc(sizeof(Player));
    if (!player)
    {
        return NULL;
    }

    player->actor = actor_create(level, position, 16, 24);
    if (!player->actor)
    {
        free(player);
        return NULL;
    }

    // Syntatic Sugar pra criar uma variável sem dar nome
    player->velocity = (Vector2){0, 0};
    player->speed = 2.5f;
    player->max_speed = 10.0f;
    player->jump_force = -6.0f;
    player->gravity_force = 0.5f;
    player->is_on_ground = false;
    player->is_jump_held = false;

    return player;
}

void player_update(Player *player, ALLEGRO_KEYBOARD_STATE *keys)
{
    handle_movement(player, keys);
    handle_jump(player, keys);
    handle_gravity(player);

    actor_move_x(player->actor, player->velocity.x);
    bool hit_ground = actor_move_y(player->actor, player->velocity.y);

    if (hit_ground && player->velocity.y > 0)
    {
        player->is_on_ground = true;
        player->velocity.y = 0;
    }
    else
    {
        Vector2 check_below = {player->actor->position.x, player->actor->position.y + 1};
        player->is_on_ground = actor_collide_at(player->actor, check_below);
    }
}

void handle_movement(Player *player, ALLEGRO_KEYBOARD_STATE *keys)
{
    float target_velocity_x = 0;

    if (al_key_down(keys, ALLEGRO_KEY_LEFT) || al_key_down(keys, ALLEGRO_KEY_A))
    {
        target_velocity_x = -player->speed;
        actor_set_facing(player->actor, false);
    }

    if (al_key_down(keys, ALLEGRO_KEY_RIGHT) || al_key_down(keys, ALLEGRO_KEY_D))
    {
        target_velocity_x = player->speed;
        actor_set_facing(player->actor, true);
    }

    float acceleration = player->is_on_ground ? 0.3f : 0.2f;

    if (target_velocity_x == 0 && player->is_on_ground)
    {
        float friction = 0.3f;
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
    player->is_jump_held = al_key_down(keys, ALLEGRO_KEY_W);
    if (player->is_jump_held && player->is_on_ground)
    {
        player->velocity.y = player->jump_force;
    }
}

// A ascenção e a queda...
void handle_gravity(Player *player)
{
    if (!player->is_on_ground)
    {
        float gravity_multiplier = 1.0f;

        if (player->velocity.y < 0)
        {
            gravity_multiplier = player->is_jump_held ? 0.5f : 0.8f;
        }
        else
        {
            if (player->velocity.y < 2.0f)
            {
                gravity_multiplier = 0.9f;
            }
            else if (player->velocity.y > 2.0f && player->velocity.y < 6.0)
            {
                gravity_multiplier = 1.0f;
            }
            else if (player->velocity.y > 6.0f)
            {
                gravity_multiplier = 1.5f;
            }
        }

        float gravity = player->gravity_force * gravity_multiplier;
        player->velocity.y = get_clamped(player->velocity.y + gravity, -player->max_speed, player->max_speed);
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

// if (!player->is_on_ground)
//{
//     float gravity_multiplier = 1.0f;
//     float gravity = player->gravity_force * gravity_multiplier;
//     player->velocity.y = get_clamped(player->velocity.y + gravity, -4.0f, 1.0f);
// }