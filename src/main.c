#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#include "../include/game_config.h"
#include "../include/math_engine.h"
#include "../include/sprites_maneger.h"

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

// ============================================================================
// ESTADO: MENU
// ============================================================================

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

// ============================================================================
// ESTADO: PLAYING 
// ============================================================================

void playing_process_events(ALLEGRO_EVENT* event, GameContext* ctx, FunctionType* func, float* a, float* b, float* c, Arrow* arrow) {
    if (event->type != ALLEGRO_EVENT_KEY_DOWN) return;

    if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
        ctx->current_state = STATE_MENU;
        return;
    }

    switch (event->keyboard.keycode) {
    case ALLEGRO_KEY_1:
        *func = FUNC_LINEAR; *a = 2.0f; *b = 0.0f; *c = 0.0f;
        arrow->is_flying = false;
        break;
    case ALLEGRO_KEY_2:
        *func = FUNC_QUADRATIC; *a = -1.0f; *b = 4.0f; *c = 0.0f;
        arrow->is_flying = false;
        break;
    case ALLEGRO_KEY_3:
        *func = FUNC_EXPONENTIAL; *a = 1.0f; *b = 2.0f; *c = 0.0f;
        arrow->is_flying = false;
        break;
    case ALLEGRO_KEY_4:
        *func = FUNC_LOGARITHMIC; *a = 3.0f; *b = 2.0f; *c = 0.0f;
        arrow->is_flying = false;
        break;
    case ALLEGRO_KEY_5:
        *func = FUNC_SINUSOIDAL; *a = 2.0f; *b = 1.0f; *c = 4.0f;
        arrow->is_flying = false;
        break;
    case ALLEGRO_KEY_SPACE:
        if (*func != FUNC_NONE) {
            arrow->pos_x = 0.0f;
            arrow->pos_y = calculate_trajectory(*func, 0.0f, *a, *b, *c);
            arrow->is_flying = true;
        }
        break;
    }
}

void playing_update(GameContext* ctx, Arrow* arrow, FunctionType func, float a, float b, float c, PlayerSprite* player, float* sprite_timer) {
    if (arrow->is_flying) {
        arrow->pos_x += arrow->vel;
        arrow->pos_y = calculate_trajectory(func, arrow->pos_x, a, b, c);

        if (arrow->pos_y < 0.0f || arrow->pos_x > 20.0f) {
            arrow->is_flying = false;
        }
    }

    if (*sprite_timer >= FPS / 3) {
        *sprite_timer = 0;
        player->current_sprite = (player->current_sprite + 1) % 8;
    }
    (*sprite_timer)++;
}

void playing_render(ALLEGRO_BITMAP* bg, ALLEGRO_FONT* font, GameContext* ctx, FunctionType func, float a, float b, float c, PlayerSprite player, Arrow arrow) {
    al_clear_to_color(al_map_rgb(15, 15, 20));

    if (bg) {
        al_draw_bitmap(bg, 0, 0, 0);
    }

    // "HUD"
    const char* func_names[] = { "NENHUMA", "LINEAR", "QUADRATICA", "EXPONENCIAL", "LOGARITMICA", "SENOIDAL" };
    al_draw_filled_rectangle(15, 15, SCREEN_WIDTH - 15, 45, al_map_rgb(0, 0, 0));
    al_draw_textf(font, al_map_rgb(255, 255, 255), 20, 22, 0, "Modelo: %s (a=%.1f b=%.1f c=%.1f) | Score: %d | [ESC] Menu", func_names[func], a, b, c, ctx->score);

    // Eixos Cartesianos
    al_draw_line(scale_to_screen_x(0), scale_to_screen_y(0), scale_to_screen_x(20), scale_to_screen_y(0), al_map_rgb(255, 255, 255), 2.0f);
    al_draw_line(scale_to_screen_x(0), scale_to_screen_y(0), scale_to_screen_x(0), scale_to_screen_y(15), al_map_rgb(255, 255, 255), 2.0f);

    // Ticks do eixos
    for (int i = 1; i <= 20; i++) {
        float sx = scale_to_screen_x((float)i);
        al_draw_line(sx, scale_to_screen_y(0) - 4, sx, scale_to_screen_y(0) + 4, al_map_rgb(255, 255, 255), 1.5f);
    }

    for (int j = 1; j <= 15; j++) {
        float sy = scale_to_screen_y((float)j);
        al_draw_line(scale_to_screen_x(0) - 4, sy, scale_to_screen_x(0) + 4, sy, al_map_rgb(255, 255, 255), 1.5f);
    }

    // Personagem
    draw_player(player);

    // Flecha
    if (arrow.is_flying || arrow.pos_x > 0.0f) {
        al_draw_filled_circle(scale_to_screen_x(arrow.pos_x), scale_to_screen_y(arrow.pos_y), 4.0f, al_map_rgb(240, 200, 20));
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main(void) {
    if (!al_init()) return 1;

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);

    al_init_primitives_addon();
    al_init_image_addon();
    al_init_font_addon();
    al_install_keyboard();

    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));

    ALLEGRO_BITMAP* background = al_load_bitmap("assets/backgrounds/background_treinamento.jpg");
    ALLEGRO_BITMAP* robin_sprite = al_load_bitmap("assets/sprites/robin_ra.png");

    PlayerSprite robin_ra = { robin_sprite, 50.0f, WORLD_BASE_Y - 60.0f, 100, 100, 0, DEFAULT };

    GameContext game_context = { STATE_MENU, 0, true };
    FunctionType current_func = FUNC_NONE;
    float a = 0.0f, b = 0.0f, c = 0.0f;
    float sprite_timer = 0.0f;
    Arrow arrow = { 0.0f, 0.0f, 0.05f, 25.0f, 50.0f, false };
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
            playing_process_events(&event, &game_context, &current_func, &a, &b, &c, &arrow);
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
                playing_update(&game_context, &arrow, current_func, a, b, c, &robin_ra, &sprite_timer);
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
                playing_render(background, font, &game_context, current_func, a, b, c, robin_ra, arrow);
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