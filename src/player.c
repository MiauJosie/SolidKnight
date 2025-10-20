#include "player.h"
#include "level.h"
#include <stdlib.h>
#include <stdio.h>
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
    player->speed = 2.0f;
    player->max_speed = 10.0f;
    player->jump_force = -6.0f;
    player->gravity_force = 0.5f;
    player->is_on_ground = false;
    player->is_jump_held = false;

    return player;
}

void player_update(Player *player, ALLEGRO_KEYBOARD_STATE *keys)
{
    player->velocity.x = 0;

    handle_movement(player, keys);
    handle_jump(player, keys);
    handle_gravity(player);

    printf("VEL Y: %f\n", player->velocity.y);
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
        player->velocity.x = -player->speed;
        actor_set_facing(player->actor, false);
    }

    if (al_key_down(keys, ALLEGRO_KEY_RIGHT) || al_key_down(keys, ALLEGRO_KEY_D))
    {
        player->velocity.x = player->speed;
        actor_set_facing(player->actor, true);
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
    printf("on ground: %d | ", player->is_on_ground);
    if (!player->is_on_ground)
    {
        float gravity_multiplier = 1.0f;

        if (player->velocity.y < 0)
        {
            gravity_multiplier = player->is_jump_held ? 0.5f : 0.8f;
            printf("gm: %f | ", gravity_multiplier);
        }
        else
        {
            if (player->velocity.y < 2.0f)
            {
                gravity_multiplier = 0.9f;
                printf("gm: %f | ", gravity_multiplier);
            }
            else if (player->velocity.y > 2.0f && player->velocity.y < 6.0)
            {
                gravity_multiplier = 1.0f;
                printf("gm: %f | ", gravity_multiplier);
            }
            else if (player->velocity.y > 6.0f)
            {
                printf("FASTFALL ON ");
                gravity_multiplier = 1.5f;
                printf("gm: %f | ", gravity_multiplier);
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