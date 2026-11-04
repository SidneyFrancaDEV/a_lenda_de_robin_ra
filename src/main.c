#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/mouse.h>

#include "../include/game_config.h"
#include "../include/math_engine.h"
#include "../include/sprites_maneger.h"
#include "../include/param_selector.h"

typedef struct Arrow {
    float pos_x;
    float pos_y;
    float vel;
    float width;
    float height;
    bool is_flying;
} Arrow;

typedef struct {
    GameState current_state;
    int score;
    bool is_running;
} GameContext;

void menu_process_events(ALLEGRO_EVENT* event, GameContext* ctx) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            ctx->is_running = false;
            ctx->current_state = STATE_EXIT;
        }
        else if (event->keyboard.keycode == ALLEGRO_KEY_ENTER) {
            ctx->current_state = STATE_PLAYING;
        }
    }
}

void menu_render(ALLEGRO_FONT* font) {
    al_clear_to_color(al_map_rgb(20, 20, 30));
    al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 20, ALLEGRO_ALIGN_CENTER, "=== A LENDA DE ROBIN RÃ ===");
    al_draw_text(font, al_map_rgb(240, 200, 20), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 10, ALLEGRO_ALIGN_CENTER, "Pressione [ENTER] para Iniciar");
    al_draw_text(font, al_map_rgb(80, 80, 80), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 30, ALLEGRO_ALIGN_CENTER, "Pressione [ESC] para Sair");
}

void playing_process_events(ALLEGRO_EVENT* event, GameContext* ctx, FunctionType* func, FunctionParams* a, FunctionParams* b, FunctionParams* c, Arrow* arrow, Button soma, Button subtracao, int* traj_result) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            ctx->current_state = STATE_MENU;
            return;
        }

        switch (event->keyboard.keycode) {
        case ALLEGRO_KEY_1:
            *func = FUNC_LINEAR;
            arrow->is_flying = false;
            break;
        case ALLEGRO_KEY_2:
            *func = FUNC_QUADRATIC;
            arrow->is_flying = false;
            break;
        case ALLEGRO_KEY_3:
            *func = FUNC_EXPONENTIAL;
            arrow->is_flying = false;
            break;
        case ALLEGRO_KEY_4:
            *func = FUNC_LOGARITHMIC;
            arrow->is_flying = false;
            break;
        case ALLEGRO_KEY_5:
            *func = FUNC_SINUSOIDAL;
            arrow->is_flying = false;
            break;
        case ALLEGRO_KEY_A:
            a->state = !a->state;
            if (a->state) {
                b->state = false;
                c->state = false;
            }
            break;
        case ALLEGRO_KEY_B:
            b->state = !b->state;
            if (b->state) {
                a->state = false;
                c->state = false;
            }
            break;
        case ALLEGRO_KEY_C:
            c->state = !c->state;
            if (c->state) {
                a->state = false;
                b->state = false;
            }
            break;
        case ALLEGRO_KEY_SPACE:
            if (*func != FUNC_NONE) {
                arrow->pos_x = 0.0f;
                arrow->pos_y = calculate_trajectory(*func, 0.0f, a->value, b->value, c->value);
                arrow->is_flying = true;
                *traj_result = 0;
            }
            break;
        }
    }

    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && event->mouse.button == 1) {
        float mouse_x = (float)event->mouse.x;
        float mouse_y = (float)event->mouse.y;

        if (mouse_verificator(soma, mouse_x, mouse_y)) {
            if (a->state) param_operation(a, soma);
            if (b->state) param_operation(b, soma);
            if (c->state) param_operation(c, soma);
        }

        if (mouse_verificator(subtracao, mouse_x, mouse_y)) {
            if (a->state) param_operation(a, subtracao);
            if (b->state) param_operation(b, subtracao);
            if (c->state) param_operation(c, subtracao);
        }
    }
}

void playing_update(GameContext* ctx, Arrow* arrow, FunctionType func, FunctionParams a, FunctionParams b, FunctionParams c, PlayerSprite* player, float* sprite_timer, int* traj_result, float target_x, float target_y_min, float target_y_max, float obs_x, float obs_height, float obs_thickness) {
    if (arrow->is_flying) {
        arrow->pos_x += arrow->vel;
        arrow->pos_y = calculate_trajectory(func, arrow->pos_x, a.value, b.value, c.value);

        if (arrow->pos_x >= obs_x && arrow->pos_x <= obs_x + obs_thickness && arrow->pos_y <= obs_height) {
            arrow->is_flying = false;
            *traj_result = 2;
            printf("Colidiu com o obstaculo em X=%.2f, Y=%.2f\n", arrow->pos_x, arrow->pos_y);
        }

        if (arrow->is_flying && arrow->pos_x >= target_x) {
            arrow->pos_x = target_x;
            arrow->pos_y = calculate_trajectory(func, arrow->pos_x, a.value, b.value, c.value);
            arrow->is_flying = false;

            if (arrow->pos_y >= target_y_min && arrow->pos_y <= target_y_max) {
                *traj_result = 1;
                ctx->score++;
                printf("ACERTOU O ALVO! Y=%.2f\n", arrow->pos_y);
            }
            else {
                *traj_result = 3;
                printf("ERROU O ALVO: Altura final Y=%.2f\n", arrow->pos_y);
            }
        }

        if (arrow->is_flying && arrow->pos_y < 0.0f && arrow->pos_x > 0.0f) {
            arrow->is_flying = false;
            arrow->pos_y = 0.0f;
            *traj_result = 3;
            printf("Caiu no solo em X=%.2f\n", arrow->pos_x);
        }

        if (arrow->is_flying && arrow->pos_x > 20.0f) {
            arrow->is_flying = false;
            *traj_result = 3;
        }
    }

    if (*sprite_timer >= FPS / 3) {
        *sprite_timer = 0;
        player->current_sprite = (player->current_sprite + 1) % 8;
    }
    (*sprite_timer)++;
}

void playing_render(ALLEGRO_BITMAP* bg, ALLEGRO_FONT* font, GameContext* ctx, FunctionType func, FunctionParams a, FunctionParams b, FunctionParams c, PlayerSprite player, Arrow arrow, Button soma, Button subtracao, int traj_result, float target_x, float target_y_min, float target_y_max, float obs_x, float obs_height, float obs_thickness) {
    ALLEGRO_COLOR c_white = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR c_red = al_map_rgb(250, 50, 50);
    ALLEGRO_COLOR c_green = al_map_rgb(50, 220, 50);
    ALLEGRO_COLOR c_yellow = al_map_rgb(240, 200, 20);
    ALLEGRO_COLOR c_black = al_map_rgb(0, 0, 0);

    al_clear_to_color(al_map_rgb(15, 15, 20));

    if (bg) {
        al_draw_bitmap(bg, 0, 0, 0);
    }

    const char* func_names[] = { "SELECIONE UMA DAS FUNCOES", "LINEAR", "QUADRATICA", "EXPONENCIAL", "LOGARITMICA", "SENOIDAL" };
    al_draw_filled_rectangle(15, 17, SCREEN_WIDTH - 50, 35, c_black);
    al_draw_textf(font, c_white, 20, 20, 0, "Teclas 1-5 Mudar Funcao | [ESPACO] Disparar | [ESC] Menu | Score: %d", ctx->score);

    al_draw_filled_rectangle(15, 37, SCREEN_WIDTH - 50, 55, c_black);
    al_draw_textf(font, (func == FUNC_NONE ? c_red : c_yellow), 20, 40, 0, "Modelo Ativo: %s  (a=%.1f, b=%.1f, c=%.1f)", func_names[func], a.value, b.value, c.value);

    al_draw_filled_rectangle(15, 57, SCREEN_WIDTH - 50, 75, c_black);
    if (a.state) al_draw_textf(font, c_green, 20, 60, 0, "Parametro Ativo: (a=%.1f)", a.value);
    if (b.state) al_draw_textf(font, c_green, 20, 60, 0, "Parametro Ativo: (b=%.1f)", b.value);
    if (c.state) al_draw_textf(font, c_green, 20, 60, 0, "Parametro Ativo: (c=%.1f)", c.value);
    if (!a.state && !b.state && !c.state) al_draw_textf(font, c_white, 20, 60, 0, "Nenhum Parametro Selecionado");

    al_draw_line(scale_to_screen_x(0), scale_to_screen_y(0), scale_to_screen_x(20), scale_to_screen_y(0), c_white, 2.0f);
    al_draw_line(scale_to_screen_x(0), scale_to_screen_y(0), scale_to_screen_x(0), scale_to_screen_y(15), c_white, 2.0f);

    for (int i = 1; i <= 20; i++) {
        float sx = scale_to_screen_x((float)i);
        al_draw_line(sx, scale_to_screen_y(0) - 4, sx, scale_to_screen_y(0) + 4, c_white, 1.5f);
    }

    for (int j = 1; j <= 15; j++) {
        float sy = scale_to_screen_y((float)j);
        al_draw_line(scale_to_screen_x(0) - 4, sy, scale_to_screen_x(0) + 4, sy, c_white, 1.5f);
    }

    al_draw_filled_rectangle(scale_to_screen_x(obs_x), scale_to_screen_y(obs_height), scale_to_screen_x(obs_x + obs_thickness), scale_to_screen_y(0.0f), c_red);
    al_draw_line(scale_to_screen_x(target_x), scale_to_screen_y(target_y_min), scale_to_screen_x(target_x), scale_to_screen_y(target_y_max), c_yellow, 6.0f);

    draw_player(player);

    al_draw_filled_rectangle(soma.position_x, soma.position_y, soma.position_x + soma.widht, soma.position_y + soma.height, c_white);
    al_draw_filled_rectangle(subtracao.position_x, subtracao.position_y, subtracao.position_x + subtracao.widht, subtracao.position_y + subtracao.height, c_white);

    ALLEGRO_COLOR arrow_color = c_white;
    if (traj_result == 1) arrow_color = c_green;
    else if (traj_result == 2 || traj_result == 3) arrow_color = c_red;

    if (arrow.is_flying || arrow.pos_x > 0.0f) {
        al_draw_filled_circle(scale_to_screen_x(arrow.pos_x), scale_to_screen_y(arrow.pos_y), 4.0f, arrow_color);
    }
}

int main(void) {
    if (!al_init()) return 1;

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

    al_init_primitives_addon();
    al_init_image_addon();
    al_init_font_addon();
    al_install_keyboard();
    al_install_mouse();

    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    ALLEGRO_BITMAP* background = al_load_bitmap("assets/backgrounds/background_treinamento.jpg");
    ALLEGRO_BITMAP* robin_sprite = al_load_bitmap("assets/sprites/robin_ra.png");

    PlayerSprite robin_ra = { robin_sprite, 50.0f, WORLD_BASE_Y - 60.0f, 100, 100, 0, DEFAULT };

    Button soma = { al_get_display_width(display) - 70, al_get_display_height(display) - 70, 50, 50, ADDITION };
    Button subtracao = { al_get_display_width(display) - 70, al_get_display_height(display) - 160, 50, 50, SUBTRACTION };

    GameContext game_context = { STATE_MENU, 0, true };
    FunctionType current_func = FUNC_NONE;
    FunctionParams a = { 0.0f, false, 10, -1 };
    FunctionParams b = { 0.0f, false, 10, -1 };
    FunctionParams c = { 0.0f, false, 10, -1 };
    float sprite_timer = 0.0f;
    Arrow arrow = { 0.0f, 0.0f, 0.05f, 25.0f, 50.0f, false };
    int traj_result = 0;

    const float target_x = 4.0f;
    const float target_y_min = 0.0f;
    const float target_y_max = 8.0f;
    float obs_x = 2.0f;
    float obs_height = 3.0f;
    float obs_thickness = 0.3f;
    bool redraw = true;

    al_start_timer(timer);

    while (game_context.is_running && game_context.current_state != STATE_EXIT) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            game_context.is_running = false;
            break;
        }

        switch (game_context.current_state) {
        case STATE_MENU:
            menu_process_events(&event, &game_context);
            break;
        case STATE_PLAYING:
            playing_process_events(&event, &game_context, &current_func, &a, &b, &c, &arrow, soma, subtracao, &traj_result);
            break;
        default:
            break;
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;

            switch (game_context.current_state) {
            case STATE_MENU:
                break;
            case STATE_PLAYING:
                playing_update(&game_context, &arrow, current_func, a, b, c, &robin_ra, &sprite_timer, &traj_result, target_x, target_y_min, target_y_max, obs_x, obs_height, obs_thickness);
                break;
            default:
                break;
            }
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            switch (game_context.current_state) {
            case STATE_MENU:
                menu_render(font);
                break;
            case STATE_PLAYING:
                playing_render(background, font, &game_context, current_func, a, b, c, robin_ra, arrow, soma, subtracao, traj_result, target_x, target_y_min, target_y_max, obs_x, obs_height, obs_thickness);
                break;
            default:
                break;
            }

            al_flip_display();
        }
    }

    if (background) al_destroy_bitmap(background);
    if (robin_sprite) al_destroy_bitmap(robin_sprite);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_shutdown_primitives_addon();

    return 0;
}
