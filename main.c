
//int pixels_x = al_get_display_width(display);
//int pixels_y = al_get_display_height(display);
//al_draw_pixel(20, 40, c_white);
//al_draw_line(40, 60, 80, 60, c_white, 2);


#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

const int WIDTH = 800;
const int HEIGHT = 600;
const int FPS = 60;


typedef struct Arrow {
	float pos_x;
	float pos_y;
	float vel;

	bool active;
} Arrow;

float SCALE = 80.0f;
float BASE_X = 10.0;
float BASE_Y = 550.0f;
float current_y = 0.0;


bool target_evaluated = false;
bool hit_target = false;


static float scale_to_screen_x(float x) {
	return BASE_X + (x * SCALE);
}

static float scale_to_screen_y(float y) {
	return BASE_Y - (y * 80);
}

float calculate_1(float x, float a, float b, float c) {
	return (a * (x * x)) + (b * x) + c;
}

int main() {

	ALLEGRO_COLOR c_white = al_map_rgb(255, 255, 255);
	bool redraw = true;
	bool game_running = true;

	// INICIALIZA O ALLEGRO
	if (!al_init()) {
		printf("ERRO AO INICIALIZAR O ALLEGRO");
		return 1;
	}

	ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
	if (!event_queue)
	{
		return 1;
	}

	ALLEGRO_DISPLAY* display = al_create_display(WIDTH, HEIGHT);
	if (!display) {
		printf("ERRO AO INICIALIZAR O DISPLAY");
		return 1;
	}

	ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
	if (!timer) {
		return 1;
	}

	// INICIALIZA ADDONS
	al_init_primitives_addon();
	al_init_font_addon();
	al_install_keyboard();

	// REGISTRO DE TIPOS DE EVENTOS ACEITOS 
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));

	// GAME LOOP 

	al_start_timer(timer);

	bool is_flying = false;

	float current_x = 0.0f;
	float step_x = 0.02f;

	float a = -0.96f;
	float b = 8.0f;
	float c = 0.0f;




	const float target_x = 8.0f;       // Posição X escolhida pelo jogador/fase
	const float target_y_min = 2.0f;
	const float target_y_max = 2.5f;

	float obs_x = 2.0f;
	float obs_height = 4.0f;
	float obs_thickness = 0.4f;


	while (game_running) {

		ALLEGRO_EVENT event;
		al_wait_for_event(event_queue, &event);

		event.type == ALLEGRO_EVENT_DISPLAY_CLOSE ? game_running = false : NULL; /* Permite fechar o jogo via display */

		if (event.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
			{
				game_running = false; /* Permite fechar o jogo via ESC */
			}
			else if (event.keyboard.keycode == ALLEGRO_KEY_SPACE)
			{
				is_flying = true;
			}

		}


		if (event.type == ALLEGRO_EVENT_TIMER) /* */
		{
			redraw = true;

			if (is_flying) {
				current_x += step_x;
				current_y = calculate_1(current_x, a, b, c);

				if (current_y <= 0.0f) {
					is_flying = false;
					current_y = 0.0f;
				}
			}

		}


		if (redraw && al_is_event_queue_empty(event_queue)) {
			redraw = false;
			float screen_x = scale_to_screen_x(current_x);
			float screen_y = scale_to_screen_y(current_y);

			al_clear_to_color(al_map_rgb(0, 0, 0));

			al_draw_filled_circle(screen_x, screen_y, 2.0f, c_white);


			al_draw_line(
				scale_to_screen_x(target_x), scale_to_screen_y(target_y_min),
				scale_to_screen_x(target_x), scale_to_screen_y(target_y_max),
				c_white, 6.0f
			);

			//al_draw_circle(scale_to_screen_x(10), scale_to_screen_y(10), 10, c_white, 2);
			al_flip_display();

		}
	}

	// LIMPEZA DE MEMORIA
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_shutdown_primitives_addon();
}
