#include "manager.h"
#include "player.h"
#include "enemy.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

LevelManager* level_manager_create(void)
{
    LevelManager* manager = malloc(sizeof(LevelManager));
    if (manager == NULL)
    {
        return NULL;
    }

    manager->head = NULL;
    manager->tail = NULL;
    manager->current_node = NULL;
    manager->player = NULL;
    manager->level_count = 0;
    manager->enemy_count = 0;

    return manager;
}

void level_manager_destroy(LevelManager* manager)
{
    if (manager == NULL)
    {
        return;
    }

    if (manager->player != NULL)
    {
        player_destroy(manager->player);
    }

    for (int i = 0; i < manager->enemy_count; i++)
    {
        if (manager->enemies[i] != NULL)
        {
            enemy_destroy(manager->enemies[i]);
        }
    }

    LevelNode* current = manager->head;
    while (current != NULL)
    {
        LevelNode* next = current->next;
        level_destroy(current->level);
        free(current);
        current = next;
    }

    free(manager);
}

void level_manager_add_level(LevelManager* manager, Level* level, const char* name)
{
    LevelNode* node = malloc(sizeof(LevelNode));
    if (node == NULL)
    {
        return;
    }

    node->level = level;
    node->next = NULL;
    node->prev = NULL;

    if (name != NULL)
    {
        strncpy(node->name, name, 63);
        node->name[63] = '\0';
    }
    else
    {
        strcpy(node->name, "Unnamed Level");
    }

    if (manager->head == NULL)
    {
        manager->head = node;
        manager->tail = node;
    }
    else
    {
        manager->tail->next = node;
        node->prev = manager->tail;
        manager->tail = node;
    }

    manager->level_count++;
}

static LevelNode* level_node_at_index(LevelManager* manager, int index)
{
    if (index < 0 || index >= manager->level_count)
    {
        return NULL;
    }

    LevelNode* current = manager->head;
    for (int i = 0; i < index; i++)
    {
        current = current->next;
    }

    return current;
}

static void switch_to_node(LevelManager* manager, LevelNode* new_node, Vector2 spawn_position)
{
    if (new_node == NULL)
    {
        printf("Cannot switch to NULL node\n");
        return;
    }

    Level* new_level = new_node->level;

    if (manager->player == NULL)
    {
        manager->player = player_create(new_level, spawn_position);
        if (manager->player == NULL)
        {
            printf("Failed to create player!\n");
            return;
        }
        level_add_actor(new_level, manager->player->actor);
    }
    else
    {
        Level* old_level = manager->current_node->level;
        for (int i = 0; i < old_level->actor_count; i++)
        {
            if (old_level->actors[i] == manager->player->actor)
            {
                for (int j = i; j < old_level->actor_count - 1; j++)
                {
                    old_level->actors[j] = old_level->actors[j + 1];
                }
                old_level->actor_count--;
                break;
            }
        }

        manager->player->actor->level = new_level;
        manager->player->actor->position = spawn_position;

        manager->player->velocity = (Vector2){ 0, 0 };
        manager->player->is_on_ground = false;

        level_add_actor(new_level, manager->player->actor);
    }

    manager->current_node = new_node;

    level_update_current_room(new_level, spawn_position);
    if (new_level->current_room != NULL)
    {
        new_level->camera_position.x = new_level->current_room->x;
        new_level->camera_position.y = new_level->current_room->y;
        new_level->camera_target = new_level->camera_position;
    }

    printf("Switched to level: %s\n", new_node->name);
}

void level_manager_switch_to_level_by_index(LevelManager* manager, int level_index)
{
    LevelNode* node = level_node_at_index(manager, level_index);
    if (node == NULL)
    {
        printf("Invalid level index: %d\n", level_index);
        return;
    }

    switch_to_node(manager, node, node->level->spawn_position);
}

void level_manager_switch_to_next_level(LevelManager* manager)
{
    if (manager->current_node == NULL)
    {
        printf("No current level\n");
        return;
    }

    if (manager->current_node->next == NULL)
    {
        printf("No next level\n");
        return;
    }

    switch_to_node(manager, manager->current_node->next, manager->current_node->next->level->spawn_position);
}

void level_manager_switch_to_previous_level(LevelManager* manager, Vector2 spawn_position)
{
    if (manager->current_node == NULL)
    {
        printf("No current level\n");
        return;
    }

    if (manager->current_node->prev == NULL)
    {
        printf("No previous level\n");
        return;
    }

    switch_to_node(manager, manager->current_node->prev, spawn_position);
}

Level* level_manager_get_current_level(LevelManager* manager)
{
    if (manager->current_node == NULL)
    {
        return NULL;
    }
    return manager->current_node->level;
}

Player* level_manager_get_player(LevelManager* manager)
{
    return manager->player;
}

int level_manager_get_level_count(LevelManager* manager)
{
    return manager->level_count;
}

void level_manager_add_enemy(LevelManager* manager, Enemy* enemy)
{
    if (manager->enemy_count < 100)
    {
        manager->enemies[manager->enemy_count++] = enemy;

        Level* target_level = enemy->actor->level;
        if (target_level != NULL)
        {
            level_add_actor(target_level, enemy->actor);
        }
    }
}

void level_manager_update(LevelManager* manager, ALLEGRO_KEYBOARD_STATE* keys, float delta_time)
{
    Level* current_level = level_manager_get_current_level(manager);
    if (current_level == NULL || manager->player == NULL)
    {
        return;
    }

    for (int i = 0; i < manager->enemy_count; i++)
    {
        enemy_update(manager->enemies[i], delta_time);

        if (manager->player->enemy_ptr == manager->enemies[i]->actor && !manager->enemies[i]->is_dead)
        {
            enemy_take_damage(manager->enemies[i], 999);
            manager->player->enemy_ptr = NULL;
        }

        if (enemy_should_be_removed(manager->enemies[i]))
        {
            level_remove_actor(current_level, manager->enemies[i]->actor);

            enemy_destroy(manager->enemies[i]);

            for (int j = i; j < manager->enemy_count - 1; j++)
            {
                manager->enemies[j] = manager->enemies[j + 1];
            }
            manager->enemies[manager->enemy_count - 1] = NULL;
            manager->enemy_count--;
            i--;
        }
    }

    player_update(manager->player, keys, delta_time);
    level_update(current_level, manager->player->actor->position);
}

void level_manager_draw(LevelManager* manager)
{
    Level* current_level = level_manager_get_current_level(manager);
    if (current_level == NULL)
    {
        return;
    }

    level_draw(current_level);
    if (manager->player != NULL)
    {
        player_draw(manager->player);
    }
}

void level_manager_respawn_player(LevelManager* manager)
{
    Level* current_level = level_manager_get_current_level(manager);
    if (current_level == NULL || manager->player == NULL)
    {
        return;
    }

    manager->player->actor->position = current_level->spawn_position;
    player_respawn(manager->player);
    manager->player->velocity = (Vector2){ 0, 0 };
}