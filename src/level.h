#pragma once
#include "solid.h"
#include "actor.h"

#define MAX_ACTORS 1000
#define MAX_SOLIDS 1000
#define TILE_SIZE 8
#define MAX_ROOMS 50

typedef struct Room
{
    int x;
    int y;

    Solid* solids[MAX_SOLIDS];
    int solid_count;

    char name[64];
    bool kill_on_exit_left;
    bool kill_on_exit_right;
    bool kill_on_exit_top;
    bool kill_on_exit_bottom;
} Room;

typedef struct Level
{
    Actor* actors[MAX_ACTORS];
    int actor_count;

    Room* rooms[MAX_ROOMS];
    int room_count;

    Room* current_room;
    Vector2 spawn_position;

    Vector2 camera_position;
    Vector2 camera_target;
    float camera_lerp_speed;

    int room_width;
    int room_height;
} Level;

Level* level_create(int room_width, int room_height, Vector2 spawn_position);
void level_destroy(Level* level);
void level_add_actor(Level* level, Actor* actor);
Room* room_create(int x, int y);
void room_destroy(Room* room);
void room_add_solid(Room* room, Solid* solid);
void level_add_room(Level* level, Room* room);
Room* level_get_room_at(Level* level, Vector2 position);
void level_update_current_room(Level* level, Vector2 player_position);
void level_load_room_from_csv(Level* level, Room* room, const char* csv_path, ALLEGRO_BITMAP* tileset);
void level_update(Level* level, Vector2 player_position);
void level_draw(Level* level);
void level_remove_actor(Level* level, Actor* actor);
void level_update_camera(Level* level);
Vector2 level_get_camera_position(Level* level);
Solid** level_get_solids(Level* level);
int level_get_solid_count(Level* level);