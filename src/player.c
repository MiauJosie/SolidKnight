#include "player.h"
#include "level.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <allegro5/allegro5.h>

#define MAX_SPEED 90.0f
#define AIR_FRICTION 0.95f

#define GRAVITY 900.0f
#define MAX_FALL_SPEED 180.0f

#define JUMP_SPEED -240.0f
#define JUMP_X_BOOST 40.0f

#define COYOTE_TIME 0.1f
#define JUMP_BUFFER_TIME 0.1f

#define PEAK_GRAVITY_MULT 0.5f // Half gravity near peak of jump
#define PEAK_THRESHOLD 30.0f   // When we're "near peak"

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

    player->actor->sprite_offset.x = -20;
    player->actor->sprite_offset.y = -18;

    player->velocity = (Vector2){0, 0};
    player->is_on_ground = false;

    return player;
}

void player_update(Player *player, ALLEGRO_KEYBOARD_STATE *keys)
{
    handle_collision(player);

    handle_jump(player, keys);
    handle_movement(player, keys);
    handle_gravity(player);

    // aplica o movimento:
}

void handle_movement(Player *player, ALLEGRO_KEYBOARD_STATE *keys)
{
}

void handle_jump(Player *player, ALLEGRO_KEYBOARD_STATE *keys)
{
}

void handle_gravity(Player *player)
{
}

void handle_collision(Player *player)
{
    if (player->velocity.y < 0)
    {
        Vector2 ceiling = {player->actor->position.x, player->actor->position.y - 1};
        bool ceilingHit = actor_collide_at(player, ceiling);
    }
    else
    {
        Vector2 ground = {player->actor->position.x, player->actor->position.y + 1};
        bool groundHit = actor_collide_at(player, ground);
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