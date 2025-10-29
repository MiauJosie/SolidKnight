#pragma once
#include <allegro5/allegro5.h>
#include <stdbool.h>

typedef struct Animation
{
    ALLEGRO_BITMAP *spritesheet;
    int frame_width;
    int frame_height;
    int frame_count;
    int start_frame;
    float frame_duration;
    bool loop;
} Animation;

typedef struct Animator
{
    Animation *current_animation;
    int current_frame;
    float frame_timer;
    bool is_playing;
    bool finished;
} Animator;

// ficar animado demais pode trazer certas, consequências...
Animation *animation_create(ALLEGRO_BITMAP *spritesheet, int frame_width, int frame_height, int frame_count, int start_frame, float frame_duration, bool loop);
void animation_destroy(Animation *animation);
Animator *animator_create();
void animator_destroy(Animator *animator);
void animator_play(Animator *animator, Animation *animation);
void animator_update(Animator *animator, float delta_time);
void animator_draw(Animator *animator, float x, float y, bool flip_horizontal);
void animator_get_frame_bounds(Animator *animator, int *source_x, int *source_y, int *sw, int *sh);