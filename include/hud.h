#ifndef HUD_H
#define HUD_H

#include "math_engine.h"
#include "param_selector.h"
#include "game_state.h"
#include "math_engine.h"
#include <stdbool.h>
#include <allegro5/allegro_font.h>

#define TOP_PANEL_HEIGHT 50.0f
#define BOTTOM_PANEL_HEIGHT 150.0f


typedef struct {
	FunctionType function_type;
	FunctionParams a, b, c;

	int select_param; // 1 = a, 2 = b, 3 = c
} PlayerInput;

void render_hud(const char* title_level, PlayerInput* player, ALLEGRO_FONT* font, LevelState level, FunctionType function_type, FunctionType required_function);
void render_axes(ALLEGRO_FONT* font);
FunctionType verification_mouse_click_function_type(float mouse_x, float mouse_y);
int verification_mouse_click_buttons(float mouse_x, float mouse_y);
void verification_mouse_click_params(ALLEGRO_FONT* font, PlayerInput* player_input, float mouse_x, float mouse_y);

#endif