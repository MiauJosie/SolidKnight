#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "level.h"

Level *level_create(void)
{
    Level *level = malloc(sizeof(Level));
    if (!level)
    {
        return NULL;
    }

    level->actor_count = 0;
    level->solid_count = 0;

    return level;
}

void level_destroy(Level *level)
{
    if (!level)
        return;

    for (int i = 0; i < level->actor_count; i++)
    {
        actor_destroy(level->actors[i]);
    }

    for (int i = 0; i < level->solid_count; i++)
    {
        solid_destroy(level->solids[i]);
    }

    free(level);
}

void level_add_actor(Level *level, Actor *actor)
{
    if (level->actor_count < MAX_ACTORS)
    {
        level->actors[level->actor_count++] = actor;
    }
}

void level_add_solid(Level *level, Solid *solid)
{
    if (level->solid_count < MAX_SOLIDS)
    {
        level->solids[level->solid_count++] = solid;
    }
}

Actor **level_get_actors(Level *level)
{
    return level->actors;
}

int level_get_actor_count(Level *level)
{
    return level->actor_count;
}

Solid **level_get_solids(Level *level)
{
    return level->solids;
}

int level_get_solid_count(Level *level)
{
    return level->solid_count;
}

void level_load_from_csv(Level *level, const char *csv_path, ALLEGRO_BITMAP *tileset)
{
    FILE *file = fopen(csv_path, "r");
    if (!file)
    {
        printf("Warning: Map file not found at %s\n", csv_path);
        return;
    }

    int tileset_width = al_get_bitmap_width(tileset);
    int tiles_per_row = tileset_width / TILE_SIZE;

    char line[512];
    int y = 0;

    while (fgets(line, sizeof(line), file) && y < MAX_SOLIDS)
    {
        int x = 0;
        char *token = strtok(line, ",");

        while (token && x < MAX_SOLIDS)
        {
            int tile_id = atoi(token);

            if (tile_id != -1)
            {
                Vector2 position = {.x = x * TILE_SIZE, .y = y * TILE_SIZE};

                Solid *solid = solid_create(level, position, TILE_SIZE, TILE_SIZE, true);
                solid->sprite = tileset;

                // Calculate source rect from tile_id
                int tile_x = (tile_id % tiles_per_row) * TILE_SIZE;
                int tile_y = (tile_id / tiles_per_row) * TILE_SIZE;

                solid->sx = tile_x;
                solid->sy = tile_y;

                level_add_solid(level, solid);
            }

            token = strtok(NULL, ",");
            x++;
        }

        y++;
    }

    fclose(file);
}

void level_update(Level *level)
{
    for (int i = 0; i < level->actor_count; i++)
    {
        // Add actor update logic here if needed
    }
}

void level_draw(Level *level)
{
    // Draw solids first (background)
    for (int i = 0; i < level->solid_count; i++)
    {
        solid_draw(level->solids[i]);
    }

    // Draw actors on top
    for (int i = 0; i < level->actor_count; i++)
    {
        actor_draw(level->actors[i]);
    }
}