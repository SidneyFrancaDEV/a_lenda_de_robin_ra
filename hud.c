#include <stdbool.h>

#include "../hud.h"
#include "../game_config.h"
#include <allegro5/allegro_primitives.h>

void render_hud() {
		// Implement the HUD rendering logic here
		// This function should draw the HUD elements on the screen

	ALLEGRO_COLOR c_black = al_map_rgb(0, 0, 0);

	al_draw_filled_rectangle(0, 0, SCREEN_WIDTH, TOP_PANEL_HEIGHT, c_black);
	const char* func_names[] = { "SELECIONE UMA DAS FUNCOES", "LINEAR", "QUADRATICA", "EXPONENCIAL", "LOGARITMICA", "SENOIDAL" };










	int panel_y = SCREEN_HEIGHT - BOTTOM_PANEL_HEIGHT;
	al_draw_filled_rectangle(0, panel_y, SCREEN_WIDTH, SCREEN_HEIGHT, c_black);





	// al_draw_textf(font, c_white, 20, 20, 0, "Teclas 1-5 Mudar Funcao | [ESPACO] Disparar | [ESC] Menu | Score: %d", ctx->score);
   // al_draw_filled_rectangle(15, 37, SCREEN_WIDTH, TOP_PANEL_HEIGHT, c_black);
   // al_draw_textf(font, (func == FUNC_NONE ? c_red : c_yellow), 20, 40, 0, "Modelo Ativo: %s  (a=%.1f, b=%.1f, c=%.1f)", func_names[func], a.value, b.value, c.value);

	//  al_draw_filled_rectangle(15, 57, SCREEN_WIDTH, TOP_PANEL_HEIGHT, c_black);

  /*  if (a.state) al_draw_textf(font, c_green, 20, 60, 0, "Parametro Ativo: (a=%.1f)", a.value);
	if (b.state) al_draw_textf(font, c_green, 20, 60, 0, "Parametro Ativo: (b=%.1f)", b.value);
	if (c.state) al_draw_textf(font, c_green, 20, 60, 0, "Parametro Ativo: (c=%.1f)", c.value);
	if (!a.state && !b.state && !c.state) al_draw_textf(font, c_white, 20, 60, 0, "Nenhum Parametro Selecionado");*/



}