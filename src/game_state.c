#include "../include/game_state.h"
#include "../include/game_config.h"
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

void verification_game_state(GameContext* ctx, LevelState* lvl) {
    if (lvl->current_lives == 0)
        ctx->current_state = STATE_GAME_OVER;

    if (lvl->current_score == 3)
        ctx->current_state = STATE_GAME_WIN;
}

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

void game_over_process_events(ALLEGRO_EVENT* event, GameContext* ctx, LevelState* lvl) {
    if (event->type == ALLEGRO_EVENT_KEY_DOWN) {
        if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
            ctx->current_state = STATE_MENU;
            lvl->current_lives = LIVES;
            lvl->current_score = 0;
        }
        else if (event->keyboard.keycode == ALLEGRO_KEY_ENTER) {
            lvl->current_lives = LIVES;
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

void game_over_render(ALLEGRO_FONT* font) {
    al_clear_to_color(al_map_rgb(20, 20, 30));
    al_draw_text(font, al_map_rgb(255, 20, 20), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 20, ALLEGRO_ALIGN_CENTER, "=== GAME OVER ===");
    al_draw_text(font, al_map_rgb(240, 200, 20), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 10, ALLEGRO_ALIGN_CENTER, "Pressione [ENTER] para Iniciar Novamente");
    al_draw_text(font, al_map_rgb(80, 80, 80), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 30, ALLEGRO_ALIGN_CENTER, "Pressione [ESC] para Voltar ao Menu");
}
