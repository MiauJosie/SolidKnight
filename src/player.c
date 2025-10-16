#include "player.h"
#include "level.h"
#include <stdlib.h>
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

    player->velocity = (Vector2){0, 0};
    player->speed = 2.0f;

    return player;
}

void player_update(Player *player, ALLEGRO_KEYBOARD_STATE *keys)
{
    Actor *actor = player->actor;

    player->velocity.x = 0;
    player->velocity.y = 0;

    if (al_key_down(keys, ALLEGRO_KEY_LEFT) || al_key_down(keys, ALLEGRO_KEY_A))
    {
        player->velocity.x = -player->speed;
        actor_set_facing(actor, false);
    }
    if (al_key_down(keys, ALLEGRO_KEY_RIGHT) || al_key_down(keys, ALLEGRO_KEY_D))
    {
        player->velocity.x = player->speed;
        actor_set_facing(actor, true);
    }

    if (al_key_down(keys, ALLEGRO_KEY_UP) || al_key_down(keys, ALLEGRO_KEY_W))
    {
        player->velocity.y = -player->speed;
    }
    if (al_key_down(keys, ALLEGRO_KEY_DOWN) || al_key_down(keys, ALLEGRO_KEY_S))
    {
        player->velocity.y = player->speed;
    }

    actor_move_x(actor, player->velocity.x);
    actor_move_y(actor, player->velocity.y);
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