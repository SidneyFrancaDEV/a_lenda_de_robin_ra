#include <stdbool.h>

#include "hud.h"
#include "game_config.h"
#include "game_state.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

void render_hud(const char* title_level, PlayerInput* player_input, ALLEGRO_FONT* font, LevelState state, FunctionType function_type, FunctionType required_function) {
	// Implement the HUD rendering logic here
	// This function should draw the HUD elements on the screen

	ALLEGRO_COLOR c_black = al_map_rgb(0, 0, 0);
	ALLEGRO_COLOR c_white = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR c_green = al_map_rgb(0, 255, 0);
	ALLEGRO_COLOR c_disabled = al_map_rgb(110, 110, 110);

	//TOP_PANEL
	al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, TOP_PANEL_HEIGHT, c_black);
	al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH/2, (TOP_PANEL_HEIGHT/2) - al_get_font_line_height(font), ALLEGRO_ALIGN_CENTER, "%s", title_level);
	al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH * 0.9, (TOP_PANEL_HEIGHT / 2) - al_get_font_line_height(font), ALLEGRO_ALIGN_CENTER, "Pontuação : %d", state.current_score);
	al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH * 0.1, (TOP_PANEL_HEIGHT / 2) - al_get_font_line_height(font), ALLEGRO_ALIGN_CENTER, "Disparos : %d", state.current_lives);

	//DOWM_PAINEL

	int panel_y = SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT;
	float params_and_text_functions_x = SCREEN_WIDTH / 1.6;

	al_draw_filled_rectangle(0, panel_y, SCREEN_WIDTH, SCREEN_HEIGHT, c_black);

	switch (function_type)
	{
	case FUNC_NONE:
		al_draw_textf(font, c_white, params_and_text_functions_x, panel_y + (SCREEN_HEIGHT * 0.03), ALLEGRO_ALIGN_LEFT, "NENHUMA FUNÇÃO SELECIONADA");
			break;
	case FUNC_LINEAR:
		al_draw_textf(font, c_white, params_and_text_functions_x, panel_y + (SCREEN_HEIGHT * 0.03), ALLEGRO_ALIGN_LEFT, "f(x) = (a * x) + b");
		break;
	case FUNC_QUADRATIC:
		al_draw_textf(font, c_white, params_and_text_functions_x, panel_y + (SCREEN_HEIGHT * 0.03), ALLEGRO_ALIGN_LEFT, "f(x) = ax² + bx + c");
		break;
	case FUNC_EXPONENTIAL:
		al_draw_textf(font, c_white, params_and_text_functions_x, panel_y + (SCREEN_HEIGHT * 0.03), ALLEGRO_ALIGN_LEFT, "f(x) = a · b^x + c");
		break;
	case FUNC_LOGARITHMIC:
		al_draw_textf(font, c_white, params_and_text_functions_x, panel_y + (SCREEN_HEIGHT * 0.03), ALLEGRO_ALIGN_LEFT, "f(x) = a·ln(bx + 1) + c");
		break;
	case FUNC_SINUSOIDAL:
		al_draw_textf(font, c_white, params_and_text_functions_x, panel_y + (SCREEN_HEIGHT * 0.03), ALLEGRO_ALIGN_LEFT, "f(x) = a . sin(bx) + c");
		break;
	}
	
	ALLEGRO_COLOR color_a = (player_input->select_param == 1) ? c_green : c_white;
	ALLEGRO_COLOR color_b = (player_input->select_param == 2) ? c_green : c_white;
	ALLEGRO_COLOR color_c = (player_input->select_param == 3) ? c_green : c_white;

	al_draw_textf(font, color_a, params_and_text_functions_x, panel_y + (SCREEN_HEIGHT * 0.08), ALLEGRO_ALIGN_LEFT, "A : %.1f", player_input->a.value);
	al_draw_textf(font, color_b, params_and_text_functions_x, panel_y + (SCREEN_HEIGHT * 0.12), ALLEGRO_ALIGN_LEFT, "B : %.1f", player_input->b.value);
	al_draw_textf(font, color_c, params_and_text_functions_x, panel_y + (SCREEN_HEIGHT * 0.16), ALLEGRO_ALIGN_LEFT, "C : %.1f", player_input->c.value);

	float rect_btn_min_x = SCREEN_WIDTH * 0.85;
	float rect_btn_max_x = SCREEN_WIDTH * 0.95;

	float btn_center_x = (rect_btn_min_x + rect_btn_max_x) / 2.0;
	float font_h = al_get_font_line_height(font) / 2.0;

	float btn_sum_y1 = panel_y + (SCREEN_HEIGHT * 0.03);
	float btn_sum_y2 = panel_y + (SCREEN_HEIGHT * 0.06);
	al_draw_filled_rectangle(rect_btn_min_x, btn_sum_y1, rect_btn_max_x, btn_sum_y2, c_white);
	al_draw_text(font, c_black, btn_center_x, ((btn_sum_y1 + btn_sum_y2) / 2.0f) - font_h, ALLEGRO_ALIGN_CENTER, "+");

	float btn_sub_y1 = panel_y + (SCREEN_HEIGHT * 0.08);
	float btn_sub_y2 = panel_y + (SCREEN_HEIGHT * 0.11);
	al_draw_filled_rectangle(rect_btn_min_x, btn_sub_y1, rect_btn_max_x, btn_sub_y2, c_white);
	al_draw_text(font, c_black, btn_center_x, ((btn_sub_y1 + btn_sub_y2) / 2.0f) - font_h, ALLEGRO_ALIGN_CENTER, "-");

	float btn_fire_y1 = panel_y + (SCREEN_HEIGHT * 0.13);
	float btn_fire_y2 = panel_y + (SCREEN_HEIGHT * 0.16);
	al_draw_filled_rectangle(rect_btn_min_x, btn_fire_y1, rect_btn_max_x, btn_fire_y2, c_white);
	al_draw_text(font, c_black, btn_center_x, ((btn_fire_y1 + btn_fire_y2) / 2.0f) - font_h, ALLEGRO_ALIGN_CENTER, "DISPARAR");

	float rect_functions_min_y = panel_y + (SCREEN_HEIGHT * 0.05);
	float rect_functions_max_y = panel_y + (SCREEN_HEIGHT * 0.16);
	float letter_functions_y = panel_y + (SCREEN_HEIGHT * 0.03);

	ALLEGRO_COLOR color_linear = (required_function == FUNC_NONE || required_function == FUNC_LINEAR) ? c_white : c_disabled;
	ALLEGRO_COLOR color_quadratic = (required_function == FUNC_NONE || required_function == FUNC_QUADRATIC) ? c_white : c_disabled;
	ALLEGRO_COLOR color_exponential = (required_function == FUNC_NONE || required_function == FUNC_EXPONENTIAL) ? c_white : c_disabled;
	ALLEGRO_COLOR color_logarithmic = (required_function == FUNC_NONE || required_function == FUNC_LOGARITHMIC) ? c_white : c_disabled;
	ALLEGRO_COLOR color_sinusoidal = (required_function == FUNC_NONE || required_function == FUNC_SINUSOIDAL) ? c_white : c_disabled;

	al_draw_text(font, color_linear, SCREEN_WIDTH * 0.09, letter_functions_y, ALLEGRO_ALIGN_CENTER, "LINEAR");
	al_draw_text(font, color_quadratic, SCREEN_WIDTH * 0.20, letter_functions_y, ALLEGRO_ALIGN_CENTER, "QUADRATICA");
	al_draw_text(font, color_exponential, SCREEN_WIDTH * 0.31, letter_functions_y, ALLEGRO_ALIGN_CENTER, "EXPONENCIAL");
	al_draw_text(font, color_logarithmic, SCREEN_WIDTH * 0.42, letter_functions_y, ALLEGRO_ALIGN_CENTER, "LOGARITMICA");
	al_draw_text(font, color_sinusoidal, SCREEN_WIDTH * 0.53, letter_functions_y, ALLEGRO_ALIGN_CENTER, "SENOIDAL");

	al_draw_filled_rectangle(SCREEN_WIDTH * 0.05, rect_functions_min_y, SCREEN_WIDTH * 0.13, rect_functions_max_y, color_linear);
	al_draw_filled_rectangle(SCREEN_WIDTH * 0.16, rect_functions_min_y, SCREEN_WIDTH * 0.24, rect_functions_max_y, color_quadratic);
	al_draw_filled_rectangle(SCREEN_WIDTH * 0.27, rect_functions_min_y, SCREEN_WIDTH * 0.35, rect_functions_max_y, color_exponential);
	al_draw_filled_rectangle(SCREEN_WIDTH * 0.38, rect_functions_min_y, SCREEN_WIDTH * 0.46, rect_functions_max_y, color_logarithmic);
	al_draw_filled_rectangle(SCREEN_WIDTH * 0.49, rect_functions_min_y, SCREEN_WIDTH * 0.57, rect_functions_max_y, color_sinusoidal);
}

FunctionType verification_mouse_click_function_type(float mouse_x, float mouse_y) {
	float height_min = (SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT) + (SCREEN_HEIGHT * 0.05);
	float height_max = (SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT) + (SCREEN_HEIGHT * 0.16);

	if ((mouse_x > SCREEN_WIDTH * 0.05 && mouse_x < SCREEN_WIDTH * 0.13) && (mouse_y > height_min && mouse_y < height_max))
		return FUNC_LINEAR;
	if ((mouse_x > SCREEN_WIDTH * 0.16 && mouse_x < SCREEN_WIDTH * 0.24) && (mouse_y > height_min && mouse_y < height_max))
		return FUNC_QUADRATIC;
	if ((mouse_x > SCREEN_WIDTH * 0.27 && mouse_x < SCREEN_WIDTH * 0.35) && (mouse_y > height_min && mouse_y < height_max))
		return FUNC_EXPONENTIAL;
	if ((mouse_x > SCREEN_WIDTH * 0.38 && mouse_x < SCREEN_WIDTH * 0.46) && (mouse_y > height_min && mouse_y < height_max))
		return FUNC_LOGARITHMIC;
	if ((mouse_x > SCREEN_WIDTH * 0.49 && mouse_x < SCREEN_WIDTH * 0.57) && (mouse_y > height_min && mouse_y < height_max))
		return FUNC_SINUSOIDAL;

	return FUNC_NONE;
}

int verification_mouse_click_buttons(float mouse_x, float mouse_y) {

	/*
	0 = Nothing happen
	1 = Sum 
	2 = Sub
	3 = Fire
	*/

	int panel_y = SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT;
	float rect_btn_min_x = SCREEN_WIDTH * 0.85;
	float rect_btn_max_x = SCREEN_WIDTH * 0.95;

	if ((mouse_x > rect_btn_min_x && mouse_x < rect_btn_max_x) && (mouse_y > panel_y + (SCREEN_HEIGHT * 0.03) && mouse_y < panel_y + (SCREEN_HEIGHT * 0.06)))
		return 1;
	if ((mouse_x > rect_btn_min_x && mouse_x < rect_btn_max_x) && (mouse_y > panel_y + (SCREEN_HEIGHT * 0.08) && mouse_y < panel_y + (SCREEN_HEIGHT * 0.11)))
		return 2;
	if ((mouse_x > rect_btn_min_x && mouse_x < rect_btn_max_x) && (mouse_y > panel_y + (SCREEN_HEIGHT * 0.13) && mouse_y < panel_y + (SCREEN_HEIGHT * 0.16)))
		return 3;

	return 0;
}

void verification_mouse_click_params(ALLEGRO_FONT* font, PlayerInput* player_input, float mouse_x, float mouse_y) {
	float params_and_text_functions_x = SCREEN_WIDTH / 1.6;
	int panel_y = SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT;

	///considerando numeros até -9.9
	const char* text_exemple = "A : -0.0";
	float padding_y = 5.0f;
	float text_width = al_get_text_width(font, text_exemple);
	float text_height = al_get_font_line_height(font);

	if ((mouse_x >= params_and_text_functions_x - padding_y && mouse_x <= params_and_text_functions_x + text_width) &&
		(mouse_y >= ((panel_y + (SCREEN_HEIGHT * 0.08)) - padding_y) && mouse_y <= ((panel_y + (SCREEN_HEIGHT * 0.08)) + text_height))) {
		player_input->select_param = (player_input->select_param == 1) ? 0 : 1;
	}
	if ((mouse_x >= params_and_text_functions_x - padding_y && mouse_x <= params_and_text_functions_x + text_width) &&
		(mouse_y >= ((panel_y + (SCREEN_HEIGHT * 0.12)) - padding_y) && mouse_y <= ((panel_y + (SCREEN_HEIGHT * 0.12)) + text_height))) {
		player_input->select_param = (player_input->select_param == 2) ? 0 : 2;
	}
	if ((mouse_x >= params_and_text_functions_x - padding_y && mouse_x <= params_and_text_functions_x + text_width) &&
		(mouse_y >= ((panel_y + (SCREEN_HEIGHT * 0.16)) - padding_y) && mouse_y <= ((panel_y + (SCREEN_HEIGHT * 0.16)) + text_height))) {
		player_input->select_param = (player_input->select_param == 3) ? 0 : 3;
	}
}

void render_axes(ALLEGRO_FONT* font) {
	ALLEGRO_COLOR c_white = al_map_rgb(255, 255, 255);

	al_draw_line(scale_to_screen_x(0), scale_to_screen_y(0), scale_to_screen_x(WORLD_MAX), scale_to_screen_y(0), c_white, 2.0f);
	al_draw_line(scale_to_screen_x(0), scale_to_screen_y(0), scale_to_screen_x(0), scale_to_screen_y(WORLD_MAX), c_white, 2.0f);

	for (int i = 1; i <= WORLD_MAX; i++) {
		float sx = scale_to_screen_x((float)i);
		al_draw_line(sx, scale_to_screen_y(0) - 4, sx, scale_to_screen_y(0) + 4, c_white, 1.5f);
		al_draw_textf(font, c_white, sx, scale_to_screen_y(0) - 15, ALLEGRO_ALIGN_RIGHT, "%d", i);
	}

	for (int j = 1; j <= WORLD_MAX; j++) {
		float sy = scale_to_screen_y((float)j);
		al_draw_line(scale_to_screen_x(0) - 4, sy, scale_to_screen_x(0) + 4, sy, c_white, 1.5f);
		al_draw_textf(font, c_white, scale_to_screen_x(0) - 8, sy, ALLEGRO_ALIGN_RIGHT, "%d", j);
	}
	al_draw_text(font, c_white, scale_to_screen_x(0) + 6, scale_to_screen_y(0) - 15, ALLEGRO_ALIGN_LEFT, "0");
	al_draw_text(font, c_white, scale_to_screen_x(WORLD_MAX), scale_to_screen_y(0) - 28, ALLEGRO_ALIGN_RIGHT, "X");
	al_draw_text(font, c_white, scale_to_screen_x(0) + 8, scale_to_screen_y(WORLD_MAX), ALLEGRO_ALIGN_LEFT, "Y");
}
