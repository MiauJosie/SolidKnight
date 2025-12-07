#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "level.h"

Level* level_create(int room_width, int room_height, Vector2 spawn_position)
{
    Level* level = malloc(sizeof(Level));
    if (level == NULL)
    {
        return NULL;
    }

    level->actor_count = 0;
    level->room_count = 0;
    level->current_room = NULL;
    level->camera_position = (Vector2){ 0, 0 };
    level->camera_target = (Vector2){ 0, 0 };
    level->camera_lerp_speed = 0.1f;
    level->room_width = room_width;
    level->room_height = room_height;
    level->spawn_position = spawn_position;

    return level;
}

void level_destroy(Level* level)
{
    if (level == NULL)
    {
        return;
    }

    for (int i = 0; i < level->room_count; i++)
    {
        room_destroy(level->rooms[i]);
    }

    free(level);
}

void level_add_actor(Level* level, Actor* actor)
{
    if (level->actor_count < MAX_ACTORS)
    {
        level->actors[level->actor_count++] = actor;
    }
}

Room* room_create(int x, int y)
{
    Room* room = malloc(sizeof(Room));
    if (room == NULL)
    {
        return NULL;
    }

    room->x = x;
    room->y = y;
    room->solid_count = 0;
    strcpy(room->name, "Room Room");
    room->kill_on_exit_left = false;
    room->kill_on_exit_right = false;
    room->kill_on_exit_top = false;
    room->kill_on_exit_bottom = false;

    return room;
}

void room_destroy(Room* room)
{
    if (room == NULL)
    {
        return;
    }

    for (int i = 0; i < room->solid_count; i++)
    {
        solid_destroy(room->solids[i]);
    }

    free(room);
}

void room_add_solid(Room* room, Solid* solid)
{
    if (room->solid_count < MAX_SOLIDS)
    {
        room->solids[room->solid_count++] = solid;
    }
}

void level_add_room(Level* level, Room* room)
{
    if (level->room_count < MAX_ROOMS)
    {
        level->rooms[level->room_count++] = room;
    }
}

Room* level_get_room_at(Level* level, Vector2 position)
{
    for (int i = 0; i < level->room_count; i++)
    {
        Room* room = level->rooms[i];

        if (position.x >= room->x && position.x < room->x + level->room_width && position.y >= room->y && position.y < room->y + level->room_height)
        {
            return room;
        }
    }

    return NULL;
}

void level_update_current_room(Level* level, Vector2 player_position)
{
    Room* new_room = level_get_room_at(level, player_position);

    if (new_room == NULL || new_room == level->current_room)
    {
        return;
    }

    level->current_room = new_room;
    printf("Entered room: %s\n", new_room->name);

    level->camera_target.x = new_room->x;
    level->camera_target.y = new_room->y;
}

void level_load_room_from_csv(Level* level, Room* room, const char* csv_path, ALLEGRO_BITMAP* tileset)
{
    FILE* file = fopen(csv_path, "r");
    if (file == NULL)
    {
        printf("Map file not found at %s\n", csv_path);
        return;
    }

    // guarda quantos tiles por linha podem ter no tileset (os nossos estão espaçados mas isso não é um problema
    // porquê pegaremos apenas os IDs que tem tile de fato)
    int tileset_width = al_get_bitmap_width(tileset);
    int tiles_per_row = tileset_width / TILE_SIZE;

    int y = 0; // index da linha
    char line[512];

    // lê cada linha do CSV
    while (fgets(line, sizeof(line), file))
    {
        int x = 0; // index da coluna
        char* id = strtok(line, ",");

        while (id)
        {
            int tile_id = atoi(id);
            if (tile_id == 176 || tile_id == 177 || tile_id == 178 || tile_id == 179 || tile_id == 190 || tile_id == 191 || tile_id == 192 || tile_id == 193 || tile_id == 194 || tile_id == 195 || tile_id == 196)
            {
                Vector2 position = { .x = room->x + (x * TILE_SIZE), .y = room->y + (y * TILE_SIZE) };

                Solid* solid = solid_create(level, position, TILE_SIZE, TILE_SIZE, false);
                solid->sprite = tileset;
                solid->is_trigger = true;
                solid->trigger_type = 1;

                int tile_x = (tile_id % tiles_per_row) * TILE_SIZE;
                int tile_y = (tile_id / tiles_per_row) * TILE_SIZE;

                solid->source_x = tile_x;
                solid->source_y = tile_y;

                room_add_solid(room, solid);
            }
            else if (tile_id == 197 || tile_id == 198 || tile_id == 180 || tile_id == 181)
            {
                Vector2 position = { .x = room->x + (x * TILE_SIZE), .y = room->y + (y * TILE_SIZE) };

                Solid* solid = solid_create(level, position, TILE_SIZE, TILE_SIZE, false);
                solid->sprite = tileset;
                solid->is_trigger = true;
                solid->trigger_type = 2;

                int tile_x = (tile_id % tiles_per_row) * TILE_SIZE;
                int tile_y = (tile_id / tiles_per_row) * TILE_SIZE;

                solid->source_x = tile_x;
                solid->source_y = tile_y;

                room_add_solid(room, solid);
            }
            else if (tile_id != -1)
            {
                Vector2 position = { .x = room->x + (x * TILE_SIZE), .y = room->y + (y * TILE_SIZE) };

                Solid* solid = solid_create(level, position, TILE_SIZE, TILE_SIZE, true);
                solid->sprite = tileset;

                int tile_x = (tile_id % tiles_per_row) * TILE_SIZE;
                int tile_y = (tile_id / tiles_per_row) * TILE_SIZE;

                solid->source_x = tile_x;
                solid->source_y = tile_y;

                room_add_solid(room, solid);
            }

            id = strtok(NULL, ","); // pega o próximo tile começando de onde parou
            x++;
        }

        y++;
    }

    fclose(file);
}

void level_update_camera(Level* level, float delta_time)
{
    (void)delta_time; // Não usado ainda

    if (level->current_room == NULL)
    {
        return;
    }

    level->camera_position.x = move_towards(level->camera_position.x, level->camera_target.x, fabs(level->camera_target.x - level->camera_position.x) * level->camera_lerp_speed);
    level->camera_position.y = move_towards(level->camera_position.y, level->camera_target.y, fabs(level->camera_target.y - level->camera_position.y) * level->camera_lerp_speed);
}

void level_update(Level* level, Vector2 player_position, float delta_time)
{
    level_update_current_room(level, player_position);
    level_update_camera(level, delta_time);
}

void level_draw(Level* level)
{
    if (level->current_room == NULL)
    {
        return;
    }

    for (int i = 0; i < level->room_count; i++)
    {
        if (level->rooms[i] == NULL)
        {
            return;
        }
        // Room *room = level->current_room;
        Room* room = level->rooms[i];
        for (int i = 0; i < room->solid_count; i++)
        {
            solid_draw(room->solids[i]);
        }
    }

    for (int i = 0; i < level->actor_count; i++)
    {
        if (level->actors[i]->is_collidable)
        {
            actor_draw(level->actors[i]);
        }
    }
}

Vector2 level_get_camera_position(Level* level)
{
    return level->camera_position;
}

Solid** level_get_solids(Level* level)
{
    return level->current_room->solids;
}

int level_get_solid_count(Level* level)
{
    if (level->current_room == NULL)
    {
        return 0;
    }
    return level->current_room->solid_count;
}

void level_remove_actor(Level* level, Actor* actor)
{
    if (level == NULL || actor == NULL)
    {
        return;
    }

    for (int i = 0; i < level->actor_count; i++)
    {
        if (level->actors[i] == actor)
        {
            // Shift elements to the left to remove the actor
            for (int j = i; j < level->actor_count - 1; j++)
            {
                level->actors[j] = level->actors[j + 1];
            }
            level->actor_count--;
            break; // Actor found and removed, exit loop
        }
    }
}