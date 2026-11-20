#ifndef GAME_STATE_H
#define GAME_STATE_H

#define LIVES 3

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <stdbool.h>

typedef enum {
    STATE_MENU,
    STATE_PLAYING,
    STATE_LEVEL_COMPLETE,
    STATE_PAUSE,
    STATE_GAME_WIN,
    STATE_GAME_OVER,
    STATE_EXIT
} GameState;

typedef struct {
    GameState current_state;
    int score;
    bool is_running;
} GameContext;

typedef struct {
    int current_lives;
    int current_score;
} LevelState;

void menu_process_events(ALLEGRO_EVENT* event, GameContext* ctx);
void game_over_process_events(ALLEGRO_EVENT* event, GameContext* ctx, LevelState* lvl);
void menu_render(ALLEGRO_FONT* font);
void game_over_render(ALLEGRO_FONT* font);
void game_win_render(ALLEGRO_FONT* font, int score);
void verification_game_state(GameContext* ctx, LevelState* lvl);

#endif
