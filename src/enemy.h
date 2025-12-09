#pragma once
#include "actor.h"
#include "level.h"
#include <allegro5/allegro5.h>

typedef enum
{
    ENEMY_TYPE_PATROL,
    ENEMY_TYPE_FLYING,
    ENEMY_TYPE_STATIONARY
} EnemyType;

typedef enum
{
    ENEMY_STATE_IDLE,
    ENEMY_STATE_PATROL,
    ENEMY_STATE_DEAD
} EnemyState;

typedef struct
{
    float patrol_distance;
    float move_speed;
    int patrol_direction; // -1 left, 1 right
    int initial_patrol_direction;
} PatrolData;

typedef struct
{
    float hover_height;
    float hover_frequency;
    float hover_time;
    float fly_distance;
    float fly_speed;
    int fly_direction;  // -1 left, 1 right
} FlyingData;

typedef union
{
    PatrolData patrol;
    FlyingData flying;
} EnemySpecificData;

typedef struct
{
    Actor* actor;

    EnemyType type;
    EnemyState state;

    Vector2 velocity;
    Vector2 start_position;

    EnemySpecificData data;

    // Health
    int health;
    int max_health;
    bool is_dead;

    // Animations
    Animation* idle_anim;
    Animation* walk_anim;
    Animation* death_anim;

    // Timer for death animation
    float death_timer;
    float death_duration;
} Enemy;

Enemy* enemy_create(Level* level, Vector2 position, EnemyType type, int initial_direction, ALLEGRO_BITMAP* sprite);
void enemy_destroy(Enemy* enemy);
void enemy_update(Enemy* enemy, float delta_time);
void enemy_take_damage(Enemy* enemy, int damage);
bool enemy_check_player_collision(Enemy* enemy, Actor* player);
bool enemy_should_be_removed(Enemy* enemy);
void enemy_patrol_behavior(Enemy* enemy);
void enemy_flying_behavior(Enemy* enemy, float delta_time);
void enemy_handle_death_state(Enemy* enemy, float delta_time);
void enemy_handle_animation(Enemy* enemy);