#include <stdio.h>
#include <stdlib.h>

#include "actor.h"
#include "solid.h"
#include "level.h"
#include "types.h"
#include "player.h"

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

void must_init(bool test, const char *description)
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

    ALLEGRO_TIMER *timer = al_create_timer(TARGET_FPS);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    must_init(queue, "queue");

    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    ALLEGRO_DISPLAY *display = al_create_display(WINDOW_W, WINDOW_H);
    must_init(display, "display");
    al_set_window_title(display, "SolidKnight");

    ALLEGRO_BITMAP *buffer = al_create_bitmap(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    must_init(buffer, "buffer");

    ALLEGRO_FONT *font = al_load_font("data/JetBrainsMono-Regular.ttf", 16, 0);
    must_init(font, "font");

    ALLEGRO_BITMAP *player_sprite = al_load_bitmap("data/player_static.png");
    must_init(player_sprite, "player sprite");

    ALLEGRO_BITMAP *level_sprite = al_load_bitmap("data/Tileset-Forest-Ground-8x8.png");
    must_init(level_sprite, "level sprite");

    Level *level = level_create(VIRTUAL_WIDTH, ROOM_HEIGHT_PIXELS);
    level->camera_lerp_speed = 0.1f;

    Room *room1 = room_create(0, 0);
    strcpy(room1->name, "Starting Room");
    level_load_room_from_csv(level, room1, "data/levelforest-room1.csv", level_sprite);
    level_add_room(level, room1);

    Room *room2 = room_create(VIRTUAL_WIDTH, 0);
    strcpy(room2->name, "Right Room");
    level_load_room_from_csv(level, room2, "data/levelforest-room3.csv", level_sprite);
    level_add_room(level, room2);

    Room *room3 = room_create(0, ROOM_HEIGHT_PIXELS);
    strcpy(room3->name, "Bottom Room");
    level_load_room_from_csv(level, room3, "data/levelforest-room2.csv", level_sprite);
    level_add_room(level, room3);

    Room *room4 = room_create(VIRTUAL_WIDTH, ROOM_HEIGHT_PIXELS);
    strcpy(room4->name, "Bottom Right Room");
    level_load_room_from_csv(level, room4, "data/levelforest-room4.csv", level_sprite);
    level_add_room(level, room4);

    Vector2 player_pos = {0, 112};
    Player *player = player_create(level, player_pos);
    player->actor->sprite = player_sprite;
    level_add_actor(level, player->actor);

    level_update_current_room(level, player_pos);
    if (level->current_room)
    {
        level->camera_position.x = level->current_room->x;
        level->camera_position.y = level->current_room->y;
        level->camera_target = level->camera_position;
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool running = true;
    bool redraw = true;
    ALLEGRO_EVENT event;
    ALLEGRO_KEYBOARD_STATE keys;

    al_start_timer(timer);
    while (running)
    {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            al_get_keyboard_state(&keys);
            player_update(player, &keys, TARGET_FPS);
            level_update(level, player->actor->position, TARGET_FPS);
            redraw = true;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
        {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
            {
                // running = false;
                player->actor->position.x = 10;
                player->actor->position.y = 112;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_F11)
            {
                al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, !(al_get_display_flags(display) & ALLEGRO_FULLSCREEN_WINDOW));
                al_acknowledge_resize(display);
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

        // Rendering ...pipeline?
        if (redraw && al_event_queue_is_empty(queue))
        {
            al_set_target_bitmap(buffer);
            al_clear_to_color(al_map_rgb(3, 1, 20));

            // move todos os objetos no level por cam_pos pra aparecerem relativos à camera
            Vector2 cam_pos = level_get_camera_position(level);
            ALLEGRO_TRANSFORM transform;
            al_identity_transform(&transform);
            al_translate_transform(&transform, -cam_pos.x, -cam_pos.y);
            al_use_transform(&transform);

            level_draw(level);
            player_draw(player);

            // remove o efeito pra desenhar UI estática
            al_identity_transform(&transform);
            al_use_transform(&transform);

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

            al_draw_scaled_bitmap(buffer, 0, 0, VIRTUAL_WIDTH, VIRTUAL_HEIGHT, dest_x, dest_y, scaled_w, scaled_h, 0);

            al_flip_display();
            redraw = false;
        }
    }

    player_destroy(player);
    al_destroy_bitmap(player_sprite);
    al_destroy_bitmap(buffer);
    level_destroy(level);
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}