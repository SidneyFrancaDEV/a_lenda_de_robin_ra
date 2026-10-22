#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include "Funções_Matematicas/funcoes.h"

#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

#define WIDTH 800
#define HEIGHT 600
#define FPS 60

// Seletores de modelos matemáticos para o disparo
typedef enum {
	FUNC_NONE = 0,
	FUNC_LINEAR = 1,
	FUNC_QUADRATIC = 2,
	FUNC_EXPONENTIAL = 3,
	FUNC_LOGARITHMIC = 4,
	FUNC_SINUSOIDAL = 5
} FunctionType;

typedef enum {
	STANDARD_POSITION = 0,
	BOW_POSIITON = 1,
	VICTORY_POSITION = 2,
	DEAD_POSITION = 3
} RobinRaSprites;

typedef struct Arrow {
	float pos_x;
	float pos_y;
	float vel;

	float width;
	float height;

	bool is_flying;
} Arrow;

typedef struct Robin_ra {
	float pos_x;
	float pos_y;

	float width;
	float height;
} Robin;

// Escala 30:1 (30px por unidade matemática)
const float SCALE = 30.0f;
const float BASE_X = 50.0f;
const float BASE_Y = HEIGHT - 60.0f;

static float scale_to_screen_x(float x) {
	return BASE_X + (x * SCALE);
}

static float scale_to_screen_y(float y) {
	return BASE_Y - (y * SCALE);
}

// Enum que seleciona qual fórmula calcular com base na escolha atual
float calculate_trajectory(FunctionType type, float x, float a, float b, float c) {

	switch (type) {
	case FUNC_LINEAR:      return calc_linear(x, a, b, c);
	case FUNC_QUADRATIC:   return calc_quadratic(x, a, b, c);
	case FUNC_EXPONENTIAL: return calc_exponential(x, a, b, c);
	case FUNC_LOGARITHMIC: return calc_logarithmic(x, a, b, c);
	case FUNC_SINUSOIDAL:  return calc_sinusoidal(x, a, b, c);
	default:               return 0;
	}
}

int state_robin_ra_sprite(RobinRaSprites robin_ra){
	switch (robin_ra) {
	case STANDARD_POSITION:      return 0;
	case BOW_POSIITON:			 return 1;
	case VICTORY_POSITION:		 return 2;
	case DEAD_POSITION:			 return 3;
	default:					 return 0;
	}
}

void preencher_array_sprites(ALLEGRO_BITMAP *img, ALLEGRO_BITMAP* array[], int colunas, int linhas, int  linha_escolha){

	float w = al_get_bitmap_width(img) / colunas;
	float h = al_get_bitmap_height(img) / linhas;

	float h_line = h * linha_escolha;

	for(int i = 0; i < colunas; i++){
		array[i] = al_create_sub_bitmap(img, i * w, h_line, w, h);
	}

}

int main() {
	if (!al_init()) {
		printf("ERRO AO INICIALIZAR O ALLEGRO\n");
		return 1;
	}

	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	if (!event_queue) return 1;

	ALLEGRO_DISPLAY* display = al_create_display(WIDTH, HEIGHT);
	if (!display) {
		printf("ERRO AO INICIALIZAR O DISPLAY\n");
		return 1;
	}

	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
	if (!timer) return 1;

	al_init_primitives_addon();
	al_init_image_addon();
	al_init_font_addon();
	al_install_keyboard();

	ALLEGRO_FONT* font = al_create_builtin_font();

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	// Cores úteis
	ALLEGRO_COLOR c_white = al_map_rgb(255, 255, 255);
	ALLEGRO_COLOR c_gray = al_map_rgb(80, 80, 80);
	ALLEGRO_COLOR c_red = al_map_rgb(250, 50, 50);
	ALLEGRO_COLOR c_green = al_map_rgb(50, 220, 50);
	ALLEGRO_COLOR c_yellow = al_map_rgb(240, 200, 20);
	ALLEGRO_COLOR c_black = al_map_rgb(0, 0, 0);

	bool redraw = true;
	bool game_running = true;

	//bool arrow.is_flying = false;

	// ========================================================================
	// ESCOLHA DA FUNÇÃO E PARÂMETROS INICIAIS
	// ========================================================================
	FunctionType current_func = FUNC_NONE;

	//float a = -1.0f;
	//float b = 4.0f;
	//float c = 0.0f;

	float a = 0.0f;
	float b = 0.0f;
	float c = 0.0f;

	// POSIÇÃO ORIGINAL robin_ra e arrow

	float original_position_x = 0.0f;
	float original_position_y = 0.0f;

	// ROBIN_RA

	float current_robin_ra_x = original_position_x;
	float current_robin_ra_y = original_position_y;
	float width_roin_ra = 100.0f;
	float height_robin_ra = 100.0f;

	// ARROW
	float current_x = original_position_x;
	float current_y = original_position_y;
	float step_x = 0.05f;

	// ALVO
	const float target_x = 4.0f;
	const float target_y_min = 0.0f;
	const float target_y_max = 8.0f;

	// OBSTÁCULO 
	float obs_x = 2.0f;
	float obs_height = 3.0f;
	float obs_thickness = 0.3f;


	// SPRITES

	//FLECHA
	ALLEGRO_BITMAP* arrowImage = al_load_bitmap("Imagens/arrow.png");	// carregando 

	//CENÁRIO
	ALLEGRO_BITMAP* backdground = al_load_bitmap("Imagens/Cenarios/fundo_treinamento.jpg");

	//ROBIN_RA
	ALLEGRO_BITMAP* robinRa = al_load_bitmap("Imagens/Personagens/robin_ra.png");

	RobinRaSprites robin_ra_enum_state  = STANDARD_POSITION;
	int roobin_ra_state = state_robin_ra_sprite(robin_ra_enum_state);



	// LOOP PREENCHENDO MATRIZ DE SPRITES

	ALLEGRO_BITMAP* sprites_robin_ra[4][8];

	int larg = al_get_bitmap_width(robinRa) / 8;
	int alt = al_get_bitmap_height(robinRa) / 4;
	int cont_widht = 0;

	for (int linha = 0; linha < 4; linha++) {
		for (int coluna = 0; coluna < 8; coluna++) {
			sprites_robin_ra[linha][coluna] = al_create_sub_bitmap(robinRa, coluna * larg, linha * alt, larg, alt);
		}
	}

	// Resultado da trajetória: 0 = voando/pronto, 1 = acertou, 2 = bateu em obstáculo, 3 = caiu no solo
	int traj_result = 0;

	Robin robin_ra = { current_robin_ra_x, current_robin_ra_y, width_roin_ra, height_robin_ra};
	Arrow arrow = { current_x, current_y, step_x, 25, 50, false };		// Arrow var


	//Target target = { scale_to_screen_x(target_x), scale_to_screen_y(target_y_min), targetWidht, targetHeight, true };
	/*bool colidVerificator(Arrow one, Target two) {

		if (one.active == true && two.active == true) {

			return ((one.pos_x - (one.width / 2.0f)) < (two.pos_x + (two.width / 2.0f)) &&
				(one.pos_x + (one.width / 2.0f)) > (two.pos_x - (two.width / 2.0f)) &&
				(one.pos_y - (one.height / 2.0f)) < (two.pos_y + (two.height / 2.0f)) &&
				(one.pos_y + (one.height / 2.0f)) > (two.pos_y - (two.height / 2.0f)));
		}
		else {
			return false;
		}
	}*/

	al_start_timer(timer);

	int cont_teste = 0;

	while (game_running) {
		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			game_running = false;
		}

		if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				game_running = false;
			}

			if (event.keyboard.keycode == ALLEGRO_KEY_1) {
				current_func = FUNC_LINEAR;
				a = 2.0f; b = 0.0f; c = 0.0f;
				arrow.is_flying = false; current_x = 0.0f;
				current_y = calculate_trajectory(current_func, 0.0f, a, b, c);
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_2) {
				current_func = FUNC_QUADRATIC;
				a = -1.0f; b = 4.0f; c = 0.0f;
				arrow.is_flying = false; current_x = 0.0f;
				current_y = calculate_trajectory(current_func, 0.0f, a, b, c);
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_3) {
				current_func = FUNC_EXPONENTIAL;
				a = 1.0f; b = 2.0f; c = 0.0f;
				arrow.is_flying = false; current_x = 0.0f;
				current_y = calculate_trajectory(current_func, 0.0f, a, b, c);
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_4) {
				current_func = FUNC_LOGARITHMIC;
				a = 3.0f; b = 2.0f; c = 0.0f;
				arrow.is_flying = false; current_x = 0.0f;
				current_y = calculate_trajectory(current_func, 0.0f, a, b, c);
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_5) {
				current_func = FUNC_SINUSOIDAL;
				a = 2.0f; b = 1.0f; c = 4.0f;
				arrow.is_flying = false; current_x = 0.0f;
				current_y = calculate_trajectory(current_func, 0.0f, a, b, c);
			}

			if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
				if (current_func != FUNC_NONE || calculate_trajectory(current_func, 0.0f, a, b, c) != 0)
				{
					current_x = 0.0f;
					current_y = calculate_trajectory(current_func, 0.0f, a, b, c);
					traj_result = 0;
					arrow.is_flying = true;
				}
			}
		}

		if (event.type == ALLEGRO_EVENT_TIMER) {
			redraw = true;

			if (arrow.is_flying) {
				current_x += step_x;
				current_y = calculate_trajectory(current_func, current_x, a, b, c);

				// 1. Checagem de Colisão com o Obstáculo (Corrigido obs_x + obs_thickness)
				if (current_x >= obs_x && current_x <= (obs_x + obs_thickness)) {
					if (current_y <= obs_height) {
						arrow.is_flying = false;
						traj_result = 2;
						printf("Colidiu com o obstaculo em X=%.2f, Y=%.2f\n", current_x, current_y);
					}
				}

				// 2. Checagem de Chegada no Alvo
				if (arrow.is_flying && current_x >= target_x) {
					current_x = target_x;
					current_y = calculate_trajectory(current_func, current_x, a, b, c);
					arrow.is_flying = false;

					if (current_y >= target_y_min && current_y <= target_y_max) {
						traj_result = 1;
						printf("ACERTOU O ALVO! Y=%.2f\n", current_y);
					}
					else {
						traj_result = 3;
						printf("ERROU O ALVO: Altura final Y=%.2f\n", current_y);
					}
				}

				// 3. Checagem de Solo
				if (arrow.is_flying && current_y < 0.0f && current_x > 0.0f) {
					arrow.is_flying = false;
					current_y = 0.0f;
					traj_result = 3;
					printf("Caiu no solo em X=%.2f\n", current_x);
				}
			}
		}

		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;

			al_clear_to_color(al_map_rgb(15, 15, 20));

			al_draw_bitmap(backdground, 0, 0, 0); // background

			// HUD NO TOPO DA TELA
			const char* func_names[] = { "SELECIONE UMA DAS FUNCOES", "LINEAR", "QUADRATICA", "EXPONENCIAL", "LOGARITMICA", "SENOIDAL" };
			al_draw_filled_rectangle(15, 17, WIDTH - 50, 35, c_black);
			al_draw_textf(font, c_white, 20, 20, 0, "Teclas 1-5 Mudar Funcao | [ESPACO] Disparar");

			al_draw_filled_rectangle(15, 37, WIDTH - 50, 55, c_black);
			al_draw_textf(font, (current_func == FUNC_NONE ? c_red : c_yellow), 20, 40, 0, "Modelo Ativo: %s  (a=%.1f, b=%.1f, c=%.1f)", func_names[current_func], a, b, c);

			// GRADE E EIXOS CARTESIANOS
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

			// DESENHO DO OBSTÁCULO
			al_draw_filled_rectangle(
				scale_to_screen_x(obs_x), scale_to_screen_y(obs_height),
				scale_to_screen_x(obs_x + obs_thickness), scale_to_screen_y(0.0f),
				c_red
			);

			// DESENHO DO ALVO
			al_draw_line(
				scale_to_screen_x(target_x), scale_to_screen_y(target_y_min),
				scale_to_screen_x(target_x), scale_to_screen_y(target_y_max),
				c_yellow, 6.0f
			);

			ALLEGRO_BITMAP* robin_ra_teste[8];

			preencher_array_sprites(robinRa, &robin_ra_teste, 8, 4, 0);

			if (cont_teste == 8 * 60)
				cont_teste = 0;
			else
				cont_teste += 1 * 60;

			al_draw_scaled_bitmap(robin_ra_teste[cont_teste / 60], 0, 0, al_get_bitmap_width(robin_ra_teste[cont_teste / 60]), al_get_bitmap_height(robin_ra_teste[cont_teste / 60]), scale_to_screen_x(robin_ra.pos_x) - (robin_ra.width / 2), scale_to_screen_y(robin_ra.pos_y) - (robin_ra.height / 2), robin_ra.width, robin_ra.height, 0);

			//al_draw_scaled_bitmap(sprites_robin_ra[1][cont_widht / 60], 0, 0, al_get_bitmap_width(sprites_robin_ra[1][cont_widht / 60]), al_get_bitmap_height(sprites_robin_ra[1][cont_widht / 60]), scale_to_screen_x(robin_ra.pos_x) - (robin_ra.width / 2), scale_to_screen_y(robin_ra.pos_y) - (robin_ra.height / 2), robin_ra.width, robin_ra.height, 0);
			
			//if (cont_widht == 240)
				//cont_widht = 0;
			//else
				//cont_widht+= 3;
				// 
			// al_draw_scaled_bitmap(robinRa, 0, 0, 150, 150, scale_to_screen_x(robin_ra.pos_x) - (robin_ra.width / 2), scale_to_screen_y(robin_ra.pos_y) - (robin_ra.height / 2), robin_ra.width, robin_ra.height, 0); // ROBIN RA BITMAP

			// PROJÉTIL
			ALLEGRO_COLOR arrow_color = c_white;
			if (traj_result == 1) arrow_color = c_green;
			else if (traj_result == 2 || traj_result == 3) arrow_color = c_red;

			arrow.pos_x = scale_to_screen_x(current_x);
			arrow.pos_y= scale_to_screen_y(current_y);


			//al_draw_scaled_bitmap(arrowImage, 0, 0, 20, 43, screen_x, screen_y, arrow.width, arrow.height, 0); // ARROW BITMAP
			if(arrow.is_flying)
				al_draw_filled_circle(arrow.pos_x, arrow.pos_y, 4.0f, arrow_color);

			al_flip_display();
		}
	}

	al_destroy_font(font);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_shutdown_primitives_addon();
	return 0;
}