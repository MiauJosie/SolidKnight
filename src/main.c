#include <stdio.h>
#include <stdlib.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
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

    // opções para mudar a forma que o jogo é renderizado
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_TIMER *timer = al_create_timer(1.0 / 30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
    must_init(queue, "queue");

    ALLEGRO_DISPLAY *display = al_create_display(640, 480);
    must_init(display, "display");

    ALLEGRO_FONT *font = al_create_builtin_font();
    must_init(font, "font");

    ALLEGRO_BITMAP *yoda = al_load_bitmap("data/yoda.jpg");
    must_init(yoda, "yoda");

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
            al_clear_to_color(al_map_rgb(2, 1, 1));
            al_draw_text(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Depois que inventaram o 38, capoeira virou dança...");
            al_draw_bitmap(yoda, 100, 100, 0);

            al_draw_filled_triangle(35, 350, 85, 375, 35, 400, al_map_rgb_f(0, 1, 0));
            al_draw_filled_rectangle(240, 260, 340, 340, al_map_rgba_f(0, 0, 0.5, 0.5));
            al_draw_circle(450, 370, 30, al_map_rgb_f(1, 0, 1), 2);
            al_draw_line(440, 110, 460, 210, al_map_rgb_f(1, 0, 0), 1);
            al_draw_line(500, 220, 570, 200, al_map_rgb_f(1, 1, 0), 1);

            ALLEGRO_VERTEX v[] = {
                {.x = 210, .y = 320, .z = 0, .color = al_map_rgb_f(1, 0, 0)},
                {.x = 330, .y = 320, .z = 0, .color = al_map_rgb_f(0, 1, 0)},
                {.x = 210, .y = 420, .z = 0, .color = al_map_rgb_f(0, 0, 1)},
                {.x = 330, .y = 420, .z = 0, .color = al_map_rgb_f(1, 1, 0)},
            };

            al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);

            al_flip_display();

            // setamos redraw como false até a vez desenhar o próximo frame
            redraw = false;
        }
    }

    al_destroy_bitmap(yoda);
    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}