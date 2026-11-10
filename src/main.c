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
#include "../include/game_state.h"
#include "../include/param_selector.h"
#include "../include/hud.h"

void playing_process_events(ALLEGRO_EVENT* event, GameContext* ctx, FunctionType* func, Arrow* arrow, Button soma, Button subtracao, int* traj_result, PlayerInput* player_input) {
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
            player_input->shoot_trigger = !player_input->shoot_trigger;
            player_input->select_param = (player_input->shoot_trigger) ? 1 : 0;

            break;
        case ALLEGRO_KEY_B:
            player_input->shoot_trigger = !player_input->shoot_trigger;
            player_input->select_param = (player_input->shoot_trigger) ? 2 : 0;

            break;
        case ALLEGRO_KEY_C:
            player_input->shoot_trigger = !player_input->shoot_trigger;
            player_input->select_param = (player_input->shoot_trigger) ? 3 : 0;

            break;
        case ALLEGRO_KEY_SPACE:
            if (*func != FUNC_NONE && arrow->is_flying == false) {
                arrow->pos_x = 0.0f;
                arrow->pos_y = calculate_trajectory(*func, 0.0f, player_input->a.value, player_input->b.value, player_input->c.value);
                arrow->is_flying = true;
                *traj_result = 0;
            }
            break;
        }
    }

    /*
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

      */
    
}

void playing_update(GameContext* ctx,LevelState* lvl, Arrow* arrow, FunctionType func, FunctionParams a, FunctionParams b, FunctionParams c, PlayerSprite* player, float* sprite_timer, int* traj_result) {
    
    //ALÉM DE ATUALIZAR OS SPRITES, AGORA FAZ A VERIFICAÇÃO DA ANIMAÇÃO DE ATAQUE

    if (arrow->is_flying && player->already_attack == false && player->state == DEFAULT) {
        *sprite_timer = 0;
        player->state = ATTACK;
    }

    if (player->state == DEFAULT) {
        if (*sprite_timer >= FPS / 3) {
            *sprite_timer = 0;
            player->current_sprite = (player->current_sprite + 1) % 8;
        }
        (*sprite_timer)++;
    }
    else if (player->state == ATTACK) {
        if (*sprite_timer >= FPS / 3) {
            *sprite_timer = 0;

            if ((player->current_sprite + 1) % 8 == 1) {
                player->already_attack = true;
                player->state = DEFAULT;
            }
            else
                player->current_sprite = (player->current_sprite + 1) % 8;
        }
        (*sprite_timer)++;
    }
    
    if (arrow->is_flying && player->already_attack) {
        arrow->pos_x += arrow->vel;
        arrow->pos_y = calculate_trajectory(func, arrow->pos_x, a.value, b.value, c.value);

        if (arrow->is_flying && arrow->pos_y < 0.0f && arrow->pos_x > 0.0f) {
            arrow->is_flying = false;
            player->already_attack = false;
            arrow->pos_y = 0.0f;
            *traj_result = 3;
            lvl->current_lives--;
            printf("Caiu no solo em X=%.2f\n", arrow->pos_x);
        }

        if (arrow->is_flying && arrow->pos_x > 20.0f) {
            arrow->is_flying = false;
            player->already_attack = false;
            lvl->current_lives--;
            *traj_result = 3;
        }
    }
}

void playing_render(ALLEGRO_BITMAP* bg, ALLEGRO_FONT* font, GameContext* ctx, FunctionType func, FunctionParams a, FunctionParams b, FunctionParams c, PlayerSprite player, Arrow arrow, Button soma, Button subtracao, int traj_result, PlayerInput* input, const char* title, LevelState state) {
    ALLEGRO_COLOR c_white = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR c_red = al_map_rgb(250, 50, 50);
    ALLEGRO_COLOR c_green = al_map_rgb(50, 220, 50);
    ALLEGRO_COLOR c_yellow = al_map_rgb(240, 200, 20);
    ALLEGRO_COLOR c_black = al_map_rgb(0, 0, 0);

    al_clear_to_color(al_map_rgb(15, 15, 20));

    if (bg) {
        al_draw_bitmap(bg, 0, 0, 0);
    }
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

    ALLEGRO_COLOR arrow_color = c_white;
    if (traj_result == 1) arrow_color = c_green;
    else if (traj_result == 2 || traj_result == 3) arrow_color = c_red;

    if (arrow.is_flying || arrow.pos_x > 0.0f) {
        al_draw_filled_circle(scale_to_screen_x(arrow.pos_x), scale_to_screen_y(arrow.pos_y), 4.0f, arrow_color);
    }

    render_hud(title, input, font, state, func);
    //draw_player(player);
  
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

    ALLEGRO_BITMAP* robin_sprite = al_load_bitmap("assets/sprites/robin_ra.png");

    PlayerSprite robin_ra = { robin_sprite, 50.0f, WORLD_BASE_Y - 60.0f, 100, 100, 0, DEFAULT };

    Button soma = { al_get_display_width(display) - 70, al_get_display_height(display) - 70, 50, 50, ADDITION };
    Button subtracao = { al_get_display_width(display) - 70, al_get_display_height(display) - 160, 50, 50, SUBTRACTION };

    GameContext game_context = { STATE_MENU, 0, true };
    LevelState level_state = { LIVES, 0};
    FunctionType current_func = FUNC_NONE;

    FunctionParams a = { 0.0f, false, 10, -1 };
    FunctionParams b = { 0.0f, false, 10, -1 };
    FunctionParams c = { 0.0f, false, 10, -1 };

    PlayerInput player_input = { current_func, a, b, c, 0, false};
    const char* title = "FASE DE TREINAMENTO";

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
            playing_process_events(&event, &game_context, &current_func, &arrow, soma, subtracao, &traj_result, &player_input);
            verification_game_state(&game_context, &level_state);
            break;
        case STATE_GAME_OVER:
            game_over_process_events(&event, &game_context, &level_state);
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
                playing_update(&game_context, &level_state, &arrow, current_func, a, b, c, &robin_ra, &sprite_timer, &traj_result);
                break;
            case STATE_GAME_OVER:
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
                playing_render(NULL, font, &game_context, current_func, a, b, c, robin_ra, arrow, soma, subtracao, traj_result, &player_input, title, level_state);
                break;
            case STATE_GAME_OVER:
                game_over_render(font);
                break;
            default:
                break;
            }

            al_flip_display();
        }
    }

    if (robin_sprite) al_destroy_bitmap(robin_sprite);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_shutdown_primitives_addon();

    return 0;
}
