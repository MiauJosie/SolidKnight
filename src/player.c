#include "player.h"
#include "level.h"
#include "enemy.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <allegro5/allegro5.h>

#define MAX_SPEED 80.0f
#define ACCELERATION 900.0f
#define GROUND_DECELERATION 900.0f
#define AIR_DECELERATION 300.0f

#define GRAVITY 850.0f
#define MAX_FALL_SPEED 180.0f
#define GROUNDING_FORCE 2.0f

#define JUMP_POWER 220.0f
#define JUMP_END_EARLY_GRAVITY_MODIFIER 3.0f

#define COYOTE_TIME 0.1f
#define JUMP_BUFFER_TIME 0.15f
#define INVALID_TIME -9999999.0f

Player* player_create(Level* level, Vector2 position)
{
    Player* player = malloc(sizeof(Player));
    if (player == NULL)
    {
        return NULL;
    }

    player->actor = actor_create(level, position, 8, 14);
    if (player->actor == NULL)
    {
        free(player);
        return NULL;
    }

    player->actor->sprite_offset.x = -20;
    player->actor->sprite_offset.y = -18;

    player->velocity = (Vector2){ 0, 0 };
    player->is_on_ground = false;
    player->time_left_ground = INVALID_TIME;
    player->jump_to_consume = false;
    player->ended_jump_early = false;
    player->time_jump_pressed = INVALID_TIME;
    player->time = 0.0f;

    player->idle_anim = NULL;
    player->run_anim = NULL;
    player->jump_anim = NULL;

    player->trigger_hit = 0;

    // Add damage system
    player->can_take_damage = true;
    player->damage_cooldown = 1.0f;
    player->damage_cooldown_timer = 0.0f;
    player->enemy_ptr = NULL;  // To store enemy reference for collision check

    player->health = 3;
    player->max_health = 3;
    player->is_dead = false;

    return player;
}

void player_respawn(Player* player)
{
    player->health = player->max_health;
    player->is_dead = false;
    player->can_take_damage = true;
}

// Update player_update() to include enemy collision check
void player_update(Player* player, ALLEGRO_KEYBOARD_STATE* keys, float delta_time)
{
    player->time += delta_time;

    // Update damage cooldown
    if (!player->can_take_damage)
    {
        player->damage_cooldown_timer += delta_time;
        if (player->damage_cooldown_timer >= player->damage_cooldown)
        {
            player->can_take_damage = true;
        }
    }

    check_triggers(player);
    check_enemy_collisions(player);

    handle_input(player, keys);
    check_collision(player);
    handle_jump(player, keys);
    handle_movement(player, keys, delta_time);
    handle_gravity(player, delta_time);
    apply_movement(player, delta_time);

    if (!player->is_on_ground)
    {
        animator_play(player->actor->animator, player->jump_anim);
    }
    else if (fabs(player->velocity.x) > 0.1f)
    {
        animator_play(player->actor->animator, player->run_anim);
    }
    else
    {
        animator_play(player->actor->animator, player->idle_anim);
    }

    actor_update(player->actor, delta_time);

    Level* current_level = player->actor->level;
    if (current_level != NULL && current_level->current_room != NULL)
    {
        Room* current_room = current_level->current_room;
        Rectangle player_bounds = actor_get_bounds(player->actor);

        // Check left boundary
        if (player_bounds.left < current_room->x && current_room->kill_on_exit_left)
        {
            player->is_dead = true;
            printf("Player died by falling out of bounds!\n");
        }
        // Check right boundary
        if (player_bounds.right > current_room->x + current_level->room_width && current_room->kill_on_exit_right)
        {
            player->is_dead = true;
            printf("Player died by falling out of bounds!\n");
        }
        // Check top boundary
        if (player_bounds.top < current_room->y && current_room->kill_on_exit_top)
        {
            player->is_dead = true;
            printf("Player died by falling out of bounds!\n");
        }
        // Check bottom boundary
        if (player_bounds.bottom > current_room->y + current_level->room_height && current_room->kill_on_exit_bottom)
        {
            player->is_dead = true;
            printf("Player died by falling out of bounds!\n");
        }
    }
}

void handle_input(Player* player, ALLEGRO_KEYBOARD_STATE* keys)
{
    static bool was_jump_pressed = false;
    bool is_jump_pressed = al_key_down(keys, ALLEGRO_KEY_W);

    if (is_jump_pressed && !was_jump_pressed)
    {
        player->jump_to_consume = true;
        player->time_jump_pressed = player->time;
    }

    was_jump_pressed = is_jump_pressed;
}

void check_collision(Player* player)
{
    Vector2 ground_check = { player->actor->position.x, player->actor->position.y + 1 };
    bool ground_hit = actor_collide_at(player->actor, ground_check);

    Vector2 ceiling_check = { player->actor->position.x, player->actor->position.y - 1 };
    bool ceiling_hit = actor_collide_at(player->actor, ceiling_check);

    if (ceiling_hit && player->velocity.y < 0)
    {
        int MAX_NUDGE_DISTANCE = 3;
        int nudge_left_distance = 0;
        int nudge_right_distance = 0;

        for (int i = 1; i <= MAX_NUDGE_DISTANCE; i++)
        {
            Vector2 nudge_check = { player->actor->position.x - i, player->actor->position.y - 1 };
            if (!actor_collide_at(player->actor, nudge_check))
            {
                nudge_left_distance = i;
                break;
            }
        }

        for (int i = 1; i <= MAX_NUDGE_DISTANCE; i++)
        {
            Vector2 nudge_check = { player->actor->position.x + i, player->actor->position.y - 1 };
            if (!actor_collide_at(player->actor, nudge_check))
            {
                nudge_right_distance = i;
                break;
            }
        }

        if (player->velocity.x < 0 && nudge_left_distance > 0)
        {
            player->actor->position.x -= nudge_left_distance;
            // printf("nudged left %d pixels\n", nudge_left_distance);
        }
        else if (player->velocity.x > 0 && nudge_right_distance > 0)
        {
            player->actor->position.x += nudge_right_distance;
            // printf("nudged right %d pixels\n", nudge_right_distance);
        }
        else if (nudge_left_distance > 0 && (nudge_right_distance == 0 || nudge_left_distance < nudge_right_distance))
        {
            player->actor->position.x -= nudge_left_distance;
            // printf("nudged left %d pixels\n", nudge_left_distance);
        }
        else if (nudge_right_distance > 0)
        {
            player->actor->position.x += nudge_right_distance;
            // printf("nudged right %d pixels\n", nudge_right_distance);
        }
        else
        {
            player->velocity.y = 0;
        }
    }

    if (!player->is_on_ground && ground_hit)
    {
        player->is_on_ground = true;
        player->ended_jump_early = false;
    }
    else if (player->is_on_ground && !ground_hit)
    {
        player->is_on_ground = false;
        player->time_left_ground = player->time;
    }
}

void handle_jump(Player* player, ALLEGRO_KEYBOARD_STATE* keys)
{
    bool is_jump_held = al_key_down(keys, ALLEGRO_KEY_W);
    if (!player->ended_jump_early && !player->is_on_ground && !is_jump_held && player->velocity.y < 0)
    {
        player->ended_jump_early = true;
    }

    bool has_buffered_jump;
    if (player->time_jump_pressed == INVALID_TIME)
    {
        has_buffered_jump = false;
    }
    else
    {
        has_buffered_jump = player->time < player->time_jump_pressed + JUMP_BUFFER_TIME;
    }

    bool can_use_coyote;
    if (player->time_left_ground == INVALID_TIME)
    {
        can_use_coyote = false;
    }
    else
    {
        can_use_coyote = !player->is_on_ground && player->time < player->time_left_ground + COYOTE_TIME;
    }

    if (!player->jump_to_consume && !has_buffered_jump)
    {
        return;
    }

    if (player->is_on_ground || can_use_coyote)
    {
        player->ended_jump_early = false;
        player->time_jump_pressed = INVALID_TIME;
        player->velocity.y = -JUMP_POWER;
    }

    player->jump_to_consume = false;
}

void handle_movement(Player* player, ALLEGRO_KEYBOARD_STATE* keys, float delta_time)
{

    float move_x = 0.0f;
    if (al_key_down(keys, ALLEGRO_KEY_A))
    {
        move_x = -1.0f;
    }
    if (al_key_down(keys, ALLEGRO_KEY_D))
    {
        move_x = 1.0f;
    }

    if (move_x != 0)
    {
        actor_set_facing(player->actor, move_x > 0);
    }

    if (move_x == 0)
    {
        float deceleration = player->is_on_ground ? GROUND_DECELERATION : AIR_DECELERATION;
        player->velocity.x = move_towards(player->velocity.x, 0, deceleration * delta_time);
    }
    else
    {
        player->velocity.x = move_towards(player->velocity.x, move_x * MAX_SPEED, ACCELERATION * delta_time);
    }
}

void handle_gravity(Player* player, float delta_time)
{
    if (player->is_on_ground && player->velocity.y >= 0)
    {
        player->velocity.y = GROUNDING_FORCE;
        // printf("grounded\n");
        return;
    }
    else
    {
        float gravity = GRAVITY;

        if (player->ended_jump_early && player->velocity.y < 0)
        {
            gravity *= JUMP_END_EARLY_GRAVITY_MODIFIER;
            // printf("high gravity\n");
        }
        else
        {
            // printf("normal gravity\n");
        }

        player->velocity.y = move_towards(player->velocity.y, MAX_FALL_SPEED, gravity * delta_time);
    }
}

void apply_movement(Player* player, float delta_time)
{
    if (player->velocity.x != 0)
    {
        actor_move_x(player->actor, player->velocity.x * delta_time);
    }

    if (player->velocity.y != 0)
    {
        actor_move_y(player->actor, player->velocity.y * delta_time);
    }
}

void check_enemy_collisions(Player* player)
{
    Level* level = player->actor->level;
    if (level == NULL)
    {
        return;
    }

    // Iterate through all actors and check if they're enemies
    for (int i = 0; i < level->actor_count; i++)
    {
        Actor* other = level->actors[i];

        // Skip if it's the player itself
        if (other == player->actor)
        {
            continue;
        }

        // We need to check if this actor belongs to an enemy
        // The enemy system stores a pointer, so we'll check collision directly
        Rectangle player_bounds = actor_get_bounds(player->actor);
        Rectangle other_bounds = actor_get_bounds(other);

        if (rectangles_intersect(player_bounds, other_bounds) && other->is_collidable)
        {
            // Check if player is stomping
            bool is_stomping = player->velocity.y > 0 &&
                player->actor->position.y + 8 < other->position.y;

            if (is_stomping)
            {
                // Signal damage to enemy (enemy will handle this in its update)
                // For now, just bounce the player
                player->velocity.y = -150.0f;
                printf("Stomped something!\n");

                // Mark the enemy actor for the enemy update to detect
                player->enemy_ptr = other;
            }
            else if (player->can_take_damage)
            {
                // Player takes damage
                printf("Player hit!\n");
                player->can_take_damage = false;
                player->damage_cooldown_timer = 0.0f;
                player->health--;
                printf("Player has %d lives left.\n", player->health);

                if (player->health <= 0)
                {
                    player->is_dead = true;
                }

                // Knockback
                float knockback_direction = (player->actor->position.x < other->position.x) ? -1.0f : 1.0f;
                player->velocity.x = knockback_direction * 100.0f;
                player->velocity.y = -100.0f;
            }
        }
    }
}

void check_triggers(Player* player)
{
    Rectangle player_bounds = actor_get_bounds(player->actor);

    Solid** solids = level_get_solids(player->actor->level);
    int solid_count = level_get_solid_count(player->actor->level);

    for (int i = 0; i < solid_count; i++)
    {
        if (solids[i]->is_trigger)
        {
            Rectangle trigger_bounds = solid_get_bounds(solids[i]);

            if (rectangles_intersect(player_bounds, trigger_bounds))
            {
                // Signal that a trigger was hit
                player->trigger_hit = solids[i]->trigger_type;
            }
        }
    }
}

void player_draw(Player* player)
{
    if (player->is_dead)
    {
        return;
    }
    actor_draw(player->actor);
}

void player_destroy(Player* player)
{
    if (player)
    {
        actor_destroy(player->actor);
        free(player);
    }
}