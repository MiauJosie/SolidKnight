#include "enemy.h"
#include "level.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define PATROL_SPEED 30.0f
#define FLYING_SPEED 20.0f
#define GRAVITY 400.0f
#define MAX_FALL_SPEED 200.0f

Enemy* enemy_create(Level* level, Vector2 position, EnemyType type, int initial_direction, ALLEGRO_BITMAP* sprite)
{
    Enemy* enemy = malloc(sizeof(Enemy));
    if (enemy == NULL)
    {
        return NULL;
    }

    enemy->actor = actor_create(level, position, 8, 8);
    if (enemy->actor == NULL)
    {
        free(enemy);
        return NULL;
    }
    enemy->actor->sprite = sprite;
    enemy->actor->faces_left_by_default = true;

    enemy->type = type;
    enemy->state = (type == ENEMY_TYPE_STATIONARY) ? ENEMY_STATE_IDLE : ENEMY_STATE_PATROL;
    enemy->velocity = (Vector2){ 0, 0 };
    enemy->start_position = position;

    switch (type)
    {
        case ENEMY_TYPE_PATROL:
            enemy->data.patrol.patrol_distance = 5.0f;
            enemy->data.patrol.move_speed = PATROL_SPEED;
            enemy->data.patrol.initial_patrol_direction = (initial_direction == 0) ? 1 : initial_direction;
            enemy->data.patrol.patrol_direction = enemy->data.patrol.initial_patrol_direction;
            actor_set_facing(enemy->actor, enemy->data.patrol.patrol_direction == 1);
            enemy->actor->sprite_offset.x = -18;
            enemy->actor->sprite_offset.y = -24;
            enemy->actor->width = 12;
            enemy->actor->height = 7;
            break;
        case ENEMY_TYPE_FLYING:
            enemy->data.flying.hover_height = 16.0f;
            enemy->data.flying.hover_frequency = 2.0f;
            enemy->data.flying.hover_time = 0.0f;
            enemy->data.flying.fly_distance = 20.0f;
            enemy->data.flying.fly_speed = FLYING_SPEED;
            enemy->data.flying.fly_direction = (initial_direction == 0) ? 1 : initial_direction;
            actor_set_facing(enemy->actor, enemy->data.flying.fly_direction == 1);
            enemy->actor->sprite_offset.x = -16;
            enemy->actor->sprite_offset.y = -20;
            enemy->actor->width = 16;
            enemy->actor->height = 8;
            break;
        case ENEMY_TYPE_STATIONARY:
            break;
    }

    enemy->health = 3;
    enemy->max_health = 3;
    enemy->is_dead = false;

    enemy->idle_anim = NULL;
    enemy->walk_anim = NULL;
    enemy->death_anim = NULL;

    enemy->death_timer = 0.0f;
    enemy->death_duration = 0.5f;

    return enemy;
}

void enemy_destroy(Enemy* enemy)
{
    if (enemy == NULL)
    {
        return;
    }

    if (enemy->actor != NULL)
    {
        actor_destroy(enemy->actor);
    }

    free(enemy);
}

void enemy_patrol_behavior(Enemy* enemy)
{
    float current_pos_x = enemy->actor->position.x;
    float start_pos_x = enemy->start_position.x;
    float patrol_dist = enemy->data.patrol.patrol_distance;

    float left_bound;
    float right_bound;

    if (enemy->data.patrol.initial_patrol_direction == 1)
    {
        left_bound = start_pos_x;
        right_bound = start_pos_x + patrol_dist;
    }
    else
    {
        left_bound = start_pos_x - patrol_dist;
        right_bound = start_pos_x;
    }

    if (enemy->data.patrol.patrol_direction == 1 && current_pos_x >= right_bound)
    {
        enemy->data.patrol.patrol_direction = -1;
        actor_set_facing(enemy->actor, false);
    }
    else if (enemy->data.patrol.patrol_direction == -1 && current_pos_x <= left_bound)
    {
        enemy->data.patrol.patrol_direction = 1;
        actor_set_facing(enemy->actor, true);
    }

    enemy->velocity.x = enemy->data.patrol.patrol_direction * enemy->data.patrol.move_speed;
}

void enemy_flying_behavior(Enemy* enemy, float delta_time)
{
    enemy->data.flying.hover_time += delta_time;

    float distance_from_start = enemy->actor->position.x - enemy->start_position.x;

    if (enemy->data.flying.fly_direction == 1 && distance_from_start >= enemy->data.flying.fly_distance)
    {
        enemy->data.flying.fly_direction = -1;
        actor_set_facing(enemy->actor, false);
    }
    else if (enemy->data.flying.fly_direction == -1 && distance_from_start <= 0)
    {
        enemy->data.flying.fly_direction = 1;
        actor_set_facing(enemy->actor, true);
    }

    enemy->velocity.x = enemy->data.flying.fly_direction * enemy->data.flying.fly_speed;

    float target_y = enemy->start_position.y + sin(enemy->data.flying.hover_time * enemy->data.flying.hover_frequency) * enemy->data.flying.hover_height;

    float y_diff = target_y - enemy->actor->position.y;
    enemy->velocity.y = y_diff * 5.0f;
}

void enemy_update(Enemy* enemy, float delta_time)
{
    if (enemy->state == ENEMY_STATE_DEAD)
    {
        enemy_handle_death_state(enemy, delta_time);
        return;
    }

    enemy->velocity = (Vector2){ 0, 0 };

    switch (enemy->type)
    {
        case ENEMY_TYPE_PATROL:
            enemy_patrol_behavior(enemy);
            break;

        case ENEMY_TYPE_FLYING:
            enemy_flying_behavior(enemy, delta_time);
            break;

        case ENEMY_TYPE_STATIONARY:
            break;
    }

    if (enemy->velocity.x != 0)
    {
        actor_move_x(enemy->actor, enemy->velocity.x * delta_time);
    }

    if (enemy->velocity.y != 0)
    {
        actor_move_y(enemy->actor, enemy->velocity.y * delta_time);
    }

    enemy_handle_animation(enemy);

    actor_update(enemy->actor, delta_time);
}

void enemy_handle_death_state(Enemy* enemy, float delta_time)
{
    enemy->death_timer += delta_time;
    actor_update(enemy->actor, delta_time);
}

void enemy_take_damage(Enemy* enemy, int damage)
{
    if (enemy->is_dead)
    {
        return;
    }

    enemy->health -= damage;

    if (enemy->health <= 0)
    {
        enemy->health = 0;
        enemy->is_dead = true;
        enemy->state = ENEMY_STATE_DEAD;
        enemy->actor->is_collidable = false;
        printf("Enemy died!\n");
    }
}

bool enemy_check_player_collision(Enemy* enemy, Actor* player)
{
    if (enemy->is_dead)
    {
        return false;
    }

    Rectangle enemy_bounds = actor_get_bounds(enemy->actor);
    Rectangle player_bounds = actor_get_bounds(player);

    return rectangles_intersect(enemy_bounds, player_bounds);
}

bool enemy_should_be_removed(Enemy* enemy)
{
    return enemy->state == ENEMY_STATE_DEAD && enemy->death_timer >= enemy->death_duration;
}

void enemy_handle_animation(Enemy* enemy)
{
    if (enemy->actor->animator != NULL)
    {
        if (fabs(enemy->velocity.x) > 0.1f && enemy->walk_anim != NULL)
        {
            animator_play(enemy->actor->animator, enemy->walk_anim);
        }
        else if (enemy->idle_anim != NULL)
        {
            animator_play(enemy->actor->animator, enemy->idle_anim);
        }
    }
}