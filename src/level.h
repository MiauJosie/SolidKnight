#pragma once
#include "solid.h"
#include "actor.h"

#define MAX_ACTORS 100
#define MAX_SOLIDS 100
#define TITLE_SIZE 8

typedef struct Level
{
    Actor *actors[MAX_ACTORS];
    int actor_count;

    Solid *solids[MAX_SOLIDS];
    int solid_count;
} Level;

Level *level_create(void);
void level_destroy(Level *level);

void level_add_actor(Level *level, Actor *actor);
void level_add_solid(Level *level, Solid *solid);

Actor **level_get_actors(Level *level);
int level_get_actor_count(Level *level);

Solid **level_get_solids(Level *level);
int level_get_solid_count(Level *level);

void level_load_from_csv(Level *level, const char *csv_path, ALLEGRO_BITMAP *tileset);
void level_update(Level *level);
void level_draw(Level *level);