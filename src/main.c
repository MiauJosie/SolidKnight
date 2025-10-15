#include <stdio.h>
#include <stdlib.h>

#include "actor.h"
#include "solid.h"
#include "level.h"
#include "types.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

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
    // inicia as partes da biblioteca
    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_init_image_addon(), "image addon");
    must_init(al_init_ttf_addon(), "ttf addon");
    must_init(al_init_font_addon(), "font addon");

    // opções para mudar a forma que o jogo é renderizado
    // al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    // al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    // al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    must_init(queue, "queue");

    ALLEGRO_DISPLAY *display = al_create_display(1280, 820);
    must_init(display, "display");

    ALLEGRO_FONT *font = al_load_font("data/JetBrainsMono-Regular.ttf", 16, 0);
    must_init(font, "font");

    ALLEGRO_BITMAP *player_sprite = al_load_bitmap("data/player.png");
    must_init(player_sprite, "player sprite");

    Vector2 player_pos = {100, 100};
    Actor *player = actor_create(player_pos, 32, 32);
    player->sprite = player_sprite;

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool running = true;
    bool redraw = true;
    ALLEGRO_EVENT event;

    al_start_timer(timer);
    while (1)
    {
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER)
        {
            // setamos redraw como true para que os elementos visuais sejam atualizados posteriormente
            redraw = true;
        }

        // checa esses dois tipos de evento e para de rodar o jogo se algum for verdadeiro
        else if ((event.type == ALLEGRO_EVENT_KEY_DOWN && event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) || (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE))
        {
            running = false;
            break;
        }

        if (!running)
        {
            break;
        }

        // atualiza os elementos visuais do jogo
        if (redraw && al_event_queue_is_empty(queue))
        {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            actor_draw(player);

            al_flip_display();

            // setamos redraw como false até a vez desenhar o próximo frame
            redraw = false;
        }
    }

    al_destroy_bitmap(player_sprite);
    // level_destroy(level);
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}