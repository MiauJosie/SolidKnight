#include <stdio.h>
#include <stdlib.h>

#include "actor.h"
#include "solid.h"
#include "level.h"
#include "manager.h"
#include "types.h"
#include "player.h"
#include "animator.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>

#define VIRTUAL_WIDTH 320
#define VIRTUAL_HEIGHT 180

#define SCALE 4
#define WINDOW_W (VIRTUAL_WIDTH * SCALE)
#define WINDOW_H (VIRTUAL_HEIGHT * SCALE)

#define TARGET_FPS (1.0f / 144.0f)

#define ROOM_HEIGHT_TILES 23
#define ROOM_HEIGHT_PIXELS (ROOM_HEIGHT_TILES * TILE_SIZE)

bool DEBUG_HITBOXES = false; // Global flag for hitbox debugging

void must_init(bool test, const char* description)
{
    if (test)
    {
        return;
    }

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int main(void)
{
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_init_image_addon(), "image addon");
    must_init(al_init_ttf_addon(), "ttf addon");
    must_init(al_init_font_addon(), "font addon");

    ALLEGRO_TIMER* timer = al_create_timer(TARGET_FPS);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY* display = al_create_display(WINDOW_W, WINDOW_H);
    must_init(display, "display");
    al_set_window_title(display, "SolidKnight");

    ALLEGRO_BITMAP* buffer = al_create_bitmap(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    must_init(buffer, "buffer");

    ALLEGRO_FONT* font = al_load_font("data/JetBrainsMono-Regular.ttf", 10, 0);
    must_init(font, "font");

    ALLEGRO_BITMAP* player_static_sprite = al_load_bitmap("data/player_static.png");
    must_init(player_static_sprite, "player static sprite");

    ALLEGRO_BITMAP* player_spritesheet = al_load_bitmap("data/player_atlas.png");
    must_init(player_spritesheet, "player spritesheet");

    ALLEGRO_BITMAP* bat_spritesheet = al_load_bitmap("data/bat_atlas.png");
    must_init(bat_spritesheet, "bat spritesheet");

    ALLEGRO_BITMAP* slime_spritesheet = al_load_bitmap("data/slime_atlas.png");
    must_init(bat_spritesheet, "slime spritesheet");

    ALLEGRO_BITMAP* level_sprite = al_load_bitmap("data/Tileset-Forest-Ground-8x8.png");
    must_init(level_sprite, "level sprite");

    ALLEGRO_BITMAP* enemy_sprite = al_create_bitmap(8, 8);
    al_set_target_bitmap(enemy_sprite);
    al_clear_to_color(al_map_rgb(255, 0, 0)); // Red square
    al_set_target_backbuffer(display);

    // Create animations
    Animation* bat_fly_anim = animation_create(
        bat_spritesheet,
        48,
        48,
        8,
        0,
        0.15f,
        true
    );

    Animation* slime_patrol_anim = animation_create(
        slime_spritesheet,
        48,
        48,
        8,
        0,
        0.15f,
        true
    );

    // Create animations
    Animation* idle_anim = animation_create(
        player_spritesheet,
        48,
        48,
        4,
        0,
        0.15f,
        true
    );

    Animation* run_anim = animation_create(
        player_spritesheet,
        48,
        48,
        4,
        4,
        0.10f,
        true
    );

    Animation* jump_anim = animation_create(
        player_spritesheet,
        48,
        48,
        4,
        8,
        0.12f,
        false
    );

    LevelManager* level_manager = level_manager_create();

    Level* level_0 = level_create(VIRTUAL_WIDTH, ROOM_HEIGHT_PIXELS, (Vector2)
    {
        296, 112
    });
    level_0->camera_lerp_speed = 0.05f;

    Room* level_0_room_0 = room_create(0, 0);
    strcpy(level_0_room_0->name, "Level 0 - Room 0");
    level_load_room_from_csv(level_0, level_0_room_0, "data/level-0_room-0.csv", level_sprite);
    level_add_room(level_0, level_0_room_0);
    level_0_room_0->kill_on_exit_bottom = true;

    Room* level_0_room_1 = room_create(0, ROOM_HEIGHT_PIXELS);
    strcpy(level_0_room_1->name, "Level 0 - Room 1");
    level_load_room_from_csv(level_0, level_0_room_1, "data/level-0_room-1.csv", level_sprite);
    level_add_room(level_0, level_0_room_1);

    Room* level_0_room_2 = room_create(VIRTUAL_WIDTH, ROOM_HEIGHT_PIXELS);
    strcpy(level_0_room_2->name, "Level 0 - Room 2");
    level_load_room_from_csv(level_0, level_0_room_2, "data/level-0_room-2.csv", level_sprite);
    level_add_room(level_0, level_0_room_2);

    Room* level_0_room_3 = room_create(2 * VIRTUAL_WIDTH, ROOM_HEIGHT_PIXELS);
    strcpy(level_0_room_3->name, "Level 0 - Room 3");
    level_load_room_from_csv(level_0, level_0_room_3, "data/level-0_room-3.csv", level_sprite);
    level_add_room(level_0, level_0_room_3);

    level_manager_add_level(level_manager, level_0, "Level 0");
    level_manager_switch_to_level_by_index(level_manager, 0);

    Level* level_1 = level_create(VIRTUAL_WIDTH, ROOM_HEIGHT_PIXELS, (Vector2)
    {
        32, 144
    });
    level_1->camera_lerp_speed = 0.05f;

    Room* level_1_room_0 = room_create(0, 0);
    strcpy(level_1_room_0->name, "Level 1 - Room 0");
    level_load_room_from_csv(level_1, level_1_room_0, "data/level-1_room-0.csv", level_sprite);
    level_add_room(level_1, level_1_room_0);

    Room* level_1_room_1 = room_create(VIRTUAL_WIDTH, 0);
    strcpy(level_1_room_1->name, "Level 1 - Room 1");
    level_load_room_from_csv(level_1, level_1_room_1, "data/level-1_room-1.csv", level_sprite);
    level_add_room(level_1, level_1_room_1);

    Room* level_1_room_2 = room_create(VIRTUAL_WIDTH * 2, 0);
    strcpy(level_1_room_2->name, "Level 1 - Room 1");
    level_load_room_from_csv(level_1, level_1_room_2, "data/level-1_room-2.csv", level_sprite);
    level_add_room(level_1, level_1_room_2);

    Room* level_1_room_3 = room_create(VIRTUAL_WIDTH * 3, 0);
    strcpy(level_1_room_3->name, "Level 1 - Room 3");
    level_load_room_from_csv(level_1, level_1_room_3, "data/level-1_room-3.csv", level_sprite);
    level_add_room(level_1, level_1_room_3);

    level_manager_add_level(level_manager, level_1, "Level 1");

    Enemy* enemy1 = enemy_create(level_0, (Vector2)
    {
        80, 128
    }, ENEMY_TYPE_PATROL, 1);
    enemy1->actor->sprite = enemy_sprite;
    enemy1->walk_anim = slime_patrol_anim;
    enemy1->idle_anim = slime_patrol_anim;
    enemy1->actor->sprite_offset.x = -18;
    enemy1->actor->sprite_offset.y = -24;
    enemy1->data.patrol.patrol_distance = 60.0f;
    enemy1->actor->width = 12;
    enemy1->actor->height = 7;
    level_manager_add_enemy(level_manager, enemy1);

    Enemy* enemy2 = enemy_create(level_0, (Vector2)
    {
        200, 80
    }, ENEMY_TYPE_FLYING, -1);
    enemy2->actor->sprite = enemy_sprite;
    enemy2->data.flying.fly_distance = 20.0f;
    enemy2->data.flying.hover_amplitude = 20.0f;
    enemy2->walk_anim = bat_fly_anim;
    enemy2->idle_anim = bat_fly_anim;
    enemy2->actor->sprite_offset.x = -16;
    enemy2->actor->sprite_offset.y = -20;
    enemy2->actor->width = 16;
    enemy2->actor->height = 8;
    level_manager_add_enemy(level_manager, enemy2);

    Enemy* enemy3 = enemy_create(level_0, (Vector2)
    {
        24, 336
    }, ENEMY_TYPE_PATROL, 1);
    enemy3->actor->sprite = enemy_sprite;
    enemy3->walk_anim = slime_patrol_anim;
    enemy3->idle_anim = slime_patrol_anim;
    enemy3->actor->sprite_offset.x = -18;
    enemy3->actor->sprite_offset.y = -24;
    enemy3->data.patrol.patrol_distance = 8.0f;
    enemy3->actor->width = 12;
    enemy3->actor->height = 7;
    level_manager_add_enemy(level_manager, enemy3);

    // Set up player animations
    Player* player = level_manager_get_player(level_manager);
    player->actor->sprite = player_static_sprite;
    player->idle_anim = idle_anim;
    player->run_anim = run_anim;
    player->jump_anim = jump_anim;

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool running = true;
    bool redraw = true;
    bool won = false;

    ALLEGRO_EVENT event;
    ALLEGRO_KEYBOARD_STATE keys;

    al_start_timer(timer);
    while (running)
    {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            al_get_keyboard_state(&keys);
            level_manager_update(level_manager, &keys, TARGET_FPS);

            Player* player = level_manager_get_player(level_manager);
            if (player->trigger_hit == 1)
            {
                level_manager_switch_to_next_level(level_manager);
                player->trigger_hit = 0;  // Reset
            }
            else if (player->trigger_hit == 2)
            {
                won = true;
            }
            else if (player->is_dead)
            {
                level_manager_respawn_player(level_manager);
            }

            redraw = true;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                running = false;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_F11)
            {
                al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, !(al_get_display_flags(display) & ALLEGRO_FULLSCREEN_WINDOW));
                al_acknowledge_resize(display);
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_H)
            {
                DEBUG_HITBOXES = !DEBUG_HITBOXES;
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_EQUALS)
            {
                Player* player = level_manager_get_player(level_manager);
                level_manager_switch_to_next_level(level_manager);
            }
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        {
            running = false;
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_RESIZE)
        {
            al_acknowledge_resize(display);
        }

        // Rendering
        if (redraw && al_event_queue_is_empty(queue))
        {
            al_set_target_bitmap(buffer);
            al_clear_to_color(al_map_rgb(3, 1, 20));

            // Get current level's camera
            Level* current_level = level_manager_get_current_level(level_manager);
            if (current_level != NULL)
            {
                Vector2 cam_pos = level_get_camera_position(current_level);
                ALLEGRO_TRANSFORM transform;
                al_identity_transform(&transform);
                al_translate_transform(&transform, -cam_pos.x, -cam_pos.y);
                al_use_transform(&transform);

                level_manager_draw(level_manager);

                // Reset transform for UI
                al_identity_transform(&transform);
                al_use_transform(&transform);
            }

            al_draw_text(font, al_map_rgb(255, 255, 255), 2, 0, 0, current_level->current_room->name);

            Player* player = level_manager_get_player(level_manager);
            if (player != NULL)
            {
                al_draw_textf(font, al_map_rgb(255, 255, 255), VIRTUAL_WIDTH - 2, 0, ALLEGRO_ALIGN_RIGHT, "Health: %d/%d", player->health, player->max_health);
            }

            if (won)
            {
                al_clear_to_color(al_map_rgb(0, 0, 0));
                al_draw_textf(font, al_map_rgb(255, 255, 255), VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2 - 10, ALLEGRO_ALIGN_CENTER, "YOU WON!");
                al_draw_textf(font, al_map_rgb(255, 255, 255), VIRTUAL_WIDTH / 2, VIRTUAL_HEIGHT / 2, ALLEGRO_ALIGN_CENTER, "Congrats...");
            }

            // Draw to screen with scalinga
            al_set_target_bitmap(al_get_backbuffer(display));
            al_clear_to_color(al_map_rgb(0, 0, 0));

            int viewport_w = al_get_display_width(display);
            int viewport_h = al_get_display_height(display);

            float scale_x = (float)viewport_w / VIRTUAL_WIDTH;
            float scale_y = (float)viewport_h / VIRTUAL_HEIGHT;
            float scale = fminf(scale_x, scale_y);

            float scaled_w = VIRTUAL_WIDTH * scale;
            float scaled_h = VIRTUAL_HEIGHT * scale;
            float dest_x = (viewport_w - scaled_w) / 2.0f;
            float dest_y = (viewport_h - scaled_h) / 2.0f;

            al_draw_scaled_bitmap(buffer, 0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT,
                dest_x, dest_y, scaled_w, scaled_h, 0);

            al_flip_display();
            redraw = false;
        }
    }

    // Cleanup
    level_manager_destroy(level_manager);
    animation_destroy(idle_anim);
    animation_destroy(run_anim);
    animation_destroy(jump_anim);
    al_destroy_bitmap(player_static_sprite);
    al_destroy_bitmap(player_spritesheet);
    al_destroy_bitmap(level_sprite);
    al_destroy_bitmap(buffer);
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}