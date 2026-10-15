#include <stdio.h>
#include <stdbool.h>
#include <math.h>
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

typedef struct Arrow {
	float pos_x;
	float pos_y;
	float vel;

	float width;
	float height;

	bool is_flying;
} Arrow;


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

// TO-DO EXTRAIR O MOTOR PARA UM CABEÇALHO SEPARADO

// ============================================================================
// MOTOR DE FUNÇÕES MATEMÁTICAS (PRÉ-CÁLCULO)
// ============================================================================

// 1. LINEAR: y = a*x + b
// - 'a' (Coeficiente Angular): Inclinação do tiro. Se a > 0, sobe em linha reta; se a < 0, desce; se a = 0, tiro reto horizontal.
// - 'b' (Coeficiente Linear): Altura inicial de onde o tiro parte no eixo Y (quando x = 0).
// - 'c': Não utilizado na linear padrão (mantido na assinatura para padronização).
float calc_linear(float x, float a, float b, float c) {
	return (a * x) + b;
}

// 2. QUADRÁTICA: y = a*x² + b*x + c
// - 'a' (Concavidade/Gravidade): Se a < 0, faz arco de queda (∩). Se a > 0, faz curva em U (∪). Quanto maior |a|, mais fechada é a curva.
// - 'b' (Impulso/Inclinação de saída): Define o ângulo de subida inicial em x = 0 e desloca o vértice horizontalmente.
// - 'c' (Termo Independente): Altura inicial do lançamento em x = 0.
float calc_quadratic(float x, float a, float b, float c) {
	return (a * (x * x)) + (b * x) + c;
}

// 3. EXPONENCIAL: y = a * (b^x) + c
// - 'a' (Multiplicador/Escala inicial): Define a intensidade e direção do disparo (se a > 0 explode para cima; se a < 0 mergulha).
// - 'b' (Base/Taxa de crescimento): Deve ser > 0. Se b > 1 (ex: b = 2), a curva começa rasa e dispara verticalmente. Se 0 < b < 1, decai rápido e estabiliza.
// - 'c' (Assíntota Horizontal): Desloca toda a curva verticalmente. Nota: em x = 0, a altura inicial será sempre (a + c), pois b^0 = 1.
float calc_exponential(float x, float a, float b, float c) {
	if (b <= 0.0f) return c; // Proteção matemática para base negativa/zero em expoente real
	return (a * powf(b, x)) + c;
}

// 4. LOGARÍTMICA: y = a * ln(b*x + 1) + c
// Nota: Usa-se (b*x + 1) no lugar de (x) para evitar ln(0) = -infinito na saída (em x = 0, ln(1) = 0, partindo exatamente da altura 'c').
// - 'a' (Escala vertical): Controla o quão alto o gancho sobe antes de estabilizar quase na horizontal.
// - 'b' (Taxa de compressão interna): Controla a velocidade da subida inicial logo ao sair do arco.
// - 'c' (Altura inicial): Ponto exato de partida no eixo Y quando x = 0.
float calc_logarithmic(float x, float a, float b, float c) {
	if ((b * x + 1.0f) <= 0.0f) return 0.0f; // Proteção de domínio do logaritmo
	return (a * logf((b * x) + 1.0f)) + c;
}

// 5. SENOIDAL: y = a * sin(b*x) + c
// - 'a' (Amplitude): Altura máxima das cristas e vales da onda em relação ao eixo central (a onda oscila entre c - a e c + a).
// - 'b' (Frequência Angular): Velocidade da oscilação. Quanto maior o 'b', mais curvas completas (sobe e desce) a flecha faz no ar.
// - 'c' (Linha Média / Offset Vertical): Altura central em torno da qual a onda oscila (também é a altura de saída em x = 0).
float calc_sinusoidal(float x, float a, float b, float c) {
	return (a * sinf(b * x)) + c;
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

	float current_x = 0.0f;
	float current_y = 0.0f;
	float step_x = 0.05f;

	// ALVO
	const float target_x = 4.0f;
	const float target_y_min = 0.0f;
	const float target_y_max = 8.0f;

	// OBSTÁCULO 
	float obs_x = 2.0f;
	float obs_height = 3.0f;
	float obs_thickness = 0.3f;


	// SPRITE PERSONAGEM
	ALLEGRO_BITMAP* arrowImage = al_load_bitmap("arrow.png");	// carregando imagens
	ALLEGRO_BITMAP* backdground = al_load_bitmap("fundo_treinamento.jpg");
	ALLEGRO_BITMAP* robinRa = al_load_bitmap("robin_ra_sprites.png");

	float chac_width = 50.0;
	float chac_height = 60.0;



	// Resultado da trajetória: 0 = voando/pronto, 1 = acertou, 2 = bateu em obstáculo, 3 = caiu no solo
	int traj_result = 0;

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

			al_draw_scaled_bitmap(robinRa, 0, 0, 150, 150, 50, BASE_Y - chac_height, chac_width, chac_height, 0); // ROBIN RA BITMAP
			
			// PROJÉTIL
			ALLEGRO_COLOR arrow_color = c_white;
			if (traj_result == 1) arrow_color = c_green;
			else if (traj_result == 2 || traj_result == 3) arrow_color = c_red;

			float screen_x = scale_to_screen_x(current_x);
			float screen_y = scale_to_screen_y(current_y);
			
			
			//al_draw_scaled_bitmap(arrowImage, 0, 0, 20, 43, screen_x, screen_y, arrow.width, arrow.height, 0); // ARROW BITMAP
			al_draw_filled_circle(screen_x, screen_y, 4.0f, arrow_color);

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