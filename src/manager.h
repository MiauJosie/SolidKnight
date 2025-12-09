#pragma once
#include "level.h"
#include "player.h"
#include "enemy.h"

typedef struct LevelNode
{
    Level* level;
    struct LevelNode* next;
    struct LevelNode* prev;
    char name[64];
} LevelNode;

typedef struct LevelManager
{
    LevelNode* head;
    LevelNode* tail;
    LevelNode* current_node;

    Player* player;
    int level_count;

    Enemy* enemies[100];
    int enemy_count;
} LevelManager;

LevelManager* level_manager_create();
void level_manager_destroy(LevelManager* manager);
void level_manager_add_level(LevelManager* manager, Level* level, const char* name);
void level_manager_switch_to_level_by_index(LevelManager* manager, int level_index);
void level_manager_switch_to_next_level(LevelManager* manager);
void level_manager_switch_to_previous_level(LevelManager* manager, Vector2 spawn_position);
Level* level_manager_get_current_level(LevelManager* manager);
Player* level_manager_get_player(LevelManager* manager);
int level_manager_get_level_count(LevelManager* manager);
void level_manager_add_enemy(LevelManager* manager, Enemy* enemy);
void level_manager_update(LevelManager* manager, ALLEGRO_KEYBOARD_STATE* keys, float delta_time);
void level_manager_draw(LevelManager* manager);
void level_manager_respawn_player(LevelManager* manager);