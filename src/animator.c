#include "animator.h"
#include <stdlib.h>

Animation *animation_create(ALLEGRO_BITMAP *spritesheet, int frame_width, int frame_height, int frame_count, int start_frame, float frame_duration, bool loop)
{
    Animation *animation = malloc(sizeof(Animation));
    if (animation == NULL)
    {
        return NULL;
    }

    animation->spritesheet = spritesheet;
    animation->frame_width = frame_width;
    animation->frame_height = frame_height;
    animation->frame_count = frame_count;
    animation->start_frame = start_frame;
    animation->frame_duration = frame_duration;
    animation->loop = loop;

    return animation;
}

void animation_destroy(Animation *animation)
{
    if (animation == NULL)
    {
        return;
    }

    free(animation);
}

Animator *animator_create()
{
    Animator *animator = malloc(sizeof(Animator));
    if (animator == NULL)
    {
        return NULL;
    }

    animator->current_animation = NULL;
    animator->current_frame = 0;
    animator->frame_timer = 0.0f;
    animator->is_playing = false;
    animator->finished = false;

    return animator;
}

void animator_destroy(Animator *animator)
{
    if (animator == NULL)
    {
        return;
    }

    free(animator);
}

void animator_play(Animator *animator, Animation *animation)
{
    if (animator->current_animation != animation)
    {
        animator->current_animation = animation;
        animator->current_frame = 0;
        animator->frame_timer = 0.0f;
        animator->finished = false;
    }

    animator->is_playing = true;
}

void animator_update(Animator *animator, float delta_time)
{
    if (!animator->is_playing || animator->current_animation == NULL || animator->finished)
    {
        return;
    }

    Animation *animation = animator->current_animation;

    animator->frame_timer += delta_time;

    // Avança pro próximo frame
    if (animator->frame_timer >= animation->frame_duration)
    {
        animator->frame_timer -= animation->frame_duration;
        animator->current_frame++;

        // Checa se a animação terminou
        if (animator->current_frame >= animation->frame_count)
        {
            if (animation->loop)
            {
                animator->current_frame = 0; // Volta pro começo da animação
            }
            else
            {
                animator->current_frame = animation->frame_count - 1; // fica no mesmo frame
                animator->finished = true;
            }
        }
    }
}

void animator_get_frame_bounds(Animator *animator, int *source_x, int *source_y, int *source_width, int *source_height)
{
    if (animator->current_animation == NULL)
    {
        *source_x = 0;
        *source_y = 0;
        *source_width = 0;
        *source_height = 0;
        return;
    }

    Animation *animation = animator->current_animation;
    int spritesheet_width = al_get_bitmap_width(animation->spritesheet);
    int frames_per_row = spritesheet_width / animation->frame_width;

    int frame = animation->start_frame + animator->current_frame;

    *source_x = (frame % frames_per_row) * animation->frame_width;
    *source_y = (frame / frames_per_row) * animation->frame_height;
    *source_width = animation->frame_width;
    *source_height = animation->frame_height;
}

void animator_draw(Animator *animator, float x, float y, bool flip_horizontal)
{
    if (!animator->current_animation)
    {
        return;
    }

    int source_x, source_y, source_width, source_height;
    animator_get_frame_bounds(animator, &source_x, &source_y, &source_width, &source_height);

    int flags = flip_horizontal ? ALLEGRO_FLIP_HORIZONTAL : 0;
    al_draw_bitmap_region(animator->current_animation->spritesheet, source_x, source_y, source_width, source_height, x, y, flags);
}