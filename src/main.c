#include <stdio.h>
#include <stdbool.h>
#include <math.h>
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
#include "../include/level_manager.h"

static void shoot(Arrow* arrow, const PlayerInput* input, ShotResult* shot_result) {
    if (input->function_type == FUNC_NONE)
        return;

    arrow->pos_x = 0.0f;
    arrow->pos_y = calculate_trajectory(input->function_type, 0.0f, input->a.value, input->b.value, input->c.value);
    arrow->is_flying = true;
    *shot_result = SHOT_IN_FLIGHT;
}

static void playing_process_events(ALLEGRO_EVENT* event, GameContext* ctx, const LevelDefinition* level, Arrow* arrow, ShotResult* shot_result, PlayerInput* player_input, ALLEGRO_FONT* font) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
        ctx->current_state = STATE_MENU;
        return;
    }

    if (arrow->is_flying)
        return;

    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        switch (event->keyboard.keycode) {
        case ALLEGRO_KEY_1:
        case ALLEGRO_KEY_2:
        case ALLEGRO_KEY_3:
        case ALLEGRO_KEY_4:
        case ALLEGRO_KEY_5:
            if (level->function_type == FUNC_NONE)
                player_input->function_type = (FunctionType)(FUNC_LINEAR + event->keyboard.keycode - ALLEGRO_KEY_1);
            break;
        case ALLEGRO_KEY_A:
            player_input->select_param = (player_input->select_param == 1) ? 0 : 1;
            break;
        case ALLEGRO_KEY_B:
            player_input->select_param = (player_input->select_param == 2) ? 0 : 2;
            break;
        case ALLEGRO_KEY_C:
            player_input->select_param = (player_input->select_param == 3) ? 0 : 3;
            break;
        case ALLEGRO_KEY_SPACE:
            shoot(arrow, player_input, shot_result);
            break;
        }
    }

    if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && event->mouse.button == 1) {
        float mouse_x = (float)event->mouse.x;
        float mouse_y = (float)event->mouse.y;

        if (level->function_type == FUNC_NONE) {
            FunctionType selected = verification_mouse_click_function_type(mouse_x, mouse_y);
            if (selected != FUNC_NONE)
                player_input->function_type = selected;
        }

        verification_mouse_click_params(font, player_input, mouse_x, mouse_y);

        int button = verification_mouse_click_buttons(mouse_x, mouse_y);
        switch (button) {
        case 1:
        case 2: {
            float step_in_tenths = (button == 1) ? 1.0f : -1.0f;
            if (player_input->select_param == 1)
                player_input->a.value = roundf(player_input->a.value * 10.0f + step_in_tenths) / 10.0f;
            if (player_input->select_param == 2)
                player_input->b.value = roundf(player_input->b.value * 10.0f + step_in_tenths) / 10.0f;
            if (player_input->select_param == 3)
                player_input->c.value = roundf(player_input->c.value * 10.0f + step_in_tenths) / 10.0f;
            break;
        }
        case 3:
            shoot(arrow, player_input, shot_result);
            break;
        }
    }
}

static void playing_update(GameContext* ctx, LevelState* state, const LevelDefinition* level, Arrow* arrow, const PlayerInput* input, PlayerSprite* player, float* sprite_timer, ShotResult* shot_result) {
    
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
        *shot_result = level_check_shot(level, arrow->pos_x, arrow->pos_y);
        if (*shot_result == SHOT_IN_FLIGHT) {
            arrow->pos_x += arrow->vel;
            arrow->pos_y = calculate_trajectory(input->function_type, arrow->pos_x, input->a.value, input->b.value, input->c.value);
        }
        else {
            arrow->is_flying = false;
            player->already_attack = false;

            if (*shot_result == SHOT_HIT_TARGET) {
                state->current_score++;
                ctx->current_state = STATE_LEVEL_COMPLETE;
            }
            else {
                state->current_lives--;
                verification_game_state(ctx, state);
            }
        }
    }
}

static void draw_box(CollisionArea area, ALLEGRO_COLOR color) {
    al_draw_filled_rectangle(scale_to_screen_x(area.x_min), scale_to_screen_y(area.y_max),
        scale_to_screen_x(area.x_max), scale_to_screen_y(area.y_min), color);
}

static void playing_render(ALLEGRO_FONT* font, const LevelDefinition* level, PlayerSprite player, Arrow arrow, ShotResult shot_result, PlayerInput* input, LevelState state) {
    ALLEGRO_COLOR c_white = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR c_red = al_map_rgb(250, 75, 75);
    ALLEGRO_COLOR c_green = al_map_rgb(50, 220, 50);

    al_clear_to_color(al_map_rgb(27, 63, 37));
    for (int i = 0; i < level->obstacle_count; i++)
        draw_box(level->obstacles[i], al_map_rgb(105, 105, 105));

    draw_box(level->target, al_map_rgb(240, 200, 20));
    draw_player(player);

    render_axes(font);

    ALLEGRO_COLOR arrow_color = c_white;
    if (shot_result == SHOT_HIT_TARGET)
        arrow_color = c_green;
    else if (shot_result != SHOT_IN_FLIGHT)
        arrow_color = c_red;

    if ((arrow.is_flying || shot_result != SHOT_IN_FLIGHT) &&
        arrow.pos_x <= WORLD_MAX && arrow.pos_y >= 0.0f && arrow.pos_y <= WORLD_MAX) {
        al_draw_filled_circle(scale_to_screen_x(arrow.pos_x), scale_to_screen_y(arrow.pos_y), 4.0f, arrow_color);
    }

    al_draw_text(font, c_white, SCREEN_WIDTH / 2, TOP_PANEL_HEIGHT + 10, ALLEGRO_ALIGN_CENTER, level->objective);
    if (shot_result == SHOT_HIT_TARGET)
        al_draw_text(font, c_green, SCREEN_WIDTH / 2, TOP_PANEL_HEIGHT + 28, ALLEGRO_ALIGN_CENTER, "Alvo acertado! [ENTER] Continuar");
    else if (shot_result == SHOT_HIT_OBSTACLE)
        al_draw_text(font, c_red, SCREEN_WIDTH / 2, TOP_PANEL_HEIGHT + 28, ALLEGRO_ALIGN_CENTER, "A flecha bateu no obstaculo.");
    else if (shot_result == SHOT_MISSED)
        al_draw_text(font, c_red, SCREEN_WIDTH / 2, TOP_PANEL_HEIGHT + 28, ALLEGRO_ALIGN_CENTER, "A flecha saiu do campo sem acertar.");
    else if (arrow.is_flying && arrow.pos_x == 0.0f)
        al_draw_text(font, c_white, SCREEN_WIDTH / 2, TOP_PANEL_HEIGHT + 28, ALLEGRO_ALIGN_CENTER, "Preparando disparo...");

    render_hud(level->title, input, font, state, input->function_type, level->function_type);
}

static void start_level(const LevelDefinition* level, LevelState* state, PlayerInput* input, Arrow* arrow, PlayerSprite* player, float* sprite_timer, ShotResult* shot_result) {
    FunctionParams a = { level->initial_a, false, 10, -1 };
    FunctionParams b = { level->initial_b, false, 10, -1 };
    FunctionParams c = { level->initial_c, false, 10, -1 };
    PlayerInput initial_input = { level->function_type, a, b, c, 0 };

    *input = initial_input;
    state->current_lives = LIVES;
    arrow->pos_x = 0.0f;
    arrow->pos_y = 0.0f;
    arrow->is_flying = false;
    player->already_attack = false;
    player->current_sprite = 0;
    player->state = DEFAULT;
    *sprite_timer = 0.0f;
    *shot_result = SHOT_IN_FLIGHT;
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

    PlayerSprite robin_ra = { robin_sprite, WORLD_BASE_X - 92.0f, WORLD_BASE_Y - 86.0f, 86, 100, false, 0, DEFAULT };

    GameContext game_context = { STATE_MENU, 0, true };
    int current_level = 0;
    LevelState level_state = { LIVES, 0 };
    PlayerInput player_input = { 0 };

    float sprite_timer = 0.0f;
    Arrow arrow = { 0.0f, 0.0f, 0.05f, 25.0f, 50.0f, false };
    ShotResult shot_result = SHOT_IN_FLIGHT;

    bool redraw = true;

    al_start_timer(timer);

    while (game_context.is_running && game_context.current_state != STATE_EXIT) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            game_context.is_running = false;
            break;
        }

        GameState previous_state = game_context.current_state;
        switch (previous_state) {
        case STATE_MENU:
            menu_process_events(&event, &game_context);
            break;
        case STATE_GAME_WIN:
            if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    game_context.current_state = STATE_MENU;
                else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER)
                    game_context.current_state = STATE_PLAYING;
            }
            break;
        case STATE_PLAYING:
            playing_process_events(&event, &game_context, &levels[current_level], &arrow, &shot_result, &player_input, font);
            break;
        case STATE_LEVEL_COMPLETE:
            if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    game_context.current_state = STATE_MENU;
                else if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    current_level++;
                    if (current_level < level_count)
                        game_context.current_state = STATE_PLAYING;
                    else
                        game_context.current_state = STATE_GAME_WIN;
                }
            }
            break;
        case STATE_GAME_OVER:
            game_over_process_events(&event, &game_context, &level_state);
            break;
        default:
            break;
        }

        if (previous_state != game_context.current_state) {
            redraw = true;
            if (game_context.current_state == STATE_PLAYING) {
                if (previous_state == STATE_MENU || previous_state == STATE_GAME_WIN) {
                    current_level = 0;
                    level_state.current_score = 0;
                }
                start_level(&levels[current_level], &level_state, &player_input, &arrow, &robin_ra, &sprite_timer, &shot_result);
            }
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
            if (game_context.current_state == STATE_PLAYING)
                playing_update(&game_context, &level_state, &levels[current_level], &arrow, &player_input, &robin_ra, &sprite_timer, &shot_result);
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;

            switch (game_context.current_state) {
            case STATE_MENU:
                menu_render(font);
                break;
            case STATE_PLAYING:
            case STATE_LEVEL_COMPLETE:
                playing_render(font, &levels[current_level], robin_ra, arrow, shot_result, &player_input, level_state);
                break;
            case STATE_GAME_WIN:
                game_win_render(font, level_state.current_score);
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
