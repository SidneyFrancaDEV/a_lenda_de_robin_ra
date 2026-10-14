
//int pixels_x = al_get_display_width(display);
//int pixels_y = al_get_display_height(display);
//al_draw_pixel(20, 40, c_white);
//al_draw_line(40, 60, 80, 60, c_white, 2);


#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

const int WIDTH = 800;
const int HEIGHT = 600;
const int FPS = 60;


typedef struct Arrow {
	float pos_x;
	float pos_y;
	float vel;

	float width;
	float height;

	bool active;
} Arrow;

typedef struct Target {
	float pos_x;
	float pos_y;

	float width;
	float height;

	bool active;
} Target;


bool colidVerificator(Arrow one, Target two) {

	if (one.active == true && two.active == true) {

		return ((one.pos_x - (one.width / 2.0f)) < (two.pos_x + (two.width / 2.0f)) &&
			(one.pos_x + (one.width / 2.0f)) > (two.pos_x - (two.width / 2.0f)) &&
			(one.pos_y - (one.height / 2.0f)) < (two.pos_y + (two.height / 2.0f)) &&
			(one.pos_y + (one.height / 2.0f)) > (two.pos_y - (two.height / 2.0f)));
	}
	else {
		return false;
	}
}

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
	al_init_image_addon();
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

	bool collision = false;

	ALLEGRO_BITMAP* arrowImage = al_load_bitmap("arrow.png");
	
	float arrowWidht = 25.0;
	float arrowHeight = 50.0;

	float targetWidht = 10;
	float targetHeight = 50;

	Arrow arrow = {current_x, current_y, step_x, arrowWidht, arrowHeight, true};		// Arrow var
	Target target = {scale_to_screen_x(target_x), scale_to_screen_y(target_y_min), targetWidht, targetHeight, true};		// Target var

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

			if (is_flying){
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

			arrow.pos_x = scale_to_screen_x(current_x);
			arrow.pos_y = scale_to_screen_y(current_y);

			//float screen_x = scale_to_screen_x(current_x);
			//float screen_y = scale_to_screen_y(current_y);

			collision = colidVerificator(arrow, target);
			al_clear_to_color(al_map_rgb(0, 100, 0));

			//al_draw_filled_rectangle(arrow.pos_x - (arrow.width / 2.0f), arrow.pos_y - (arrow.height / 2.0f), arrow.pos_x + (arrow.width / 2.0f), arrow.pos_y + (arrow.height / 2.0f), c_white);



			//al_draw_scaled_bitmap(arrowImage, 0, 0, 20, 46, arrow.pos_x, arrow.pos_y, arrow.width, arrow.height, 0);

			ALLEGRO_BITMAP* arrowSprite = al_create_sub_bitmap(arrowImage, 0, 0, 20, 46);

			al_draw_scaled_rotated_bitmap(
				arrowSprite,
				10.0f, 23.0f,                 // cx, cy: centro da sub-imagem (20/2, 46/2)
				arrow.pos_x, arrow.pos_y,     // dx, dy: posição na tela
				(float)arrow.width / 20.0f,   // xscale
				(float)arrow.height / 46.0f,  // yscale
				al_fixatan2(arrow.pos_y, arrow.pos_x),                       // ângulo em radianos
				0                             // flags
			);

			//al_draw_line(
			//	scale_to_screen_x(target_x), scale_to_screen_y(target_y_min),
			//	scale_to_screen_x(target_x), scale_to_screen_y(target_y_max),
			//	c_white, 6.0f
			//);

			al_draw_filled_rectangle(target.pos_x - (target.width / 2.0f), target.pos_y - (target.height / 2.0f), target.pos_x + (target.width / 2.0f), target.pos_y + (target.height / 2.0f), c_white);

			//al_draw_circle(scale_to_screen_x(10), scale_to_screen_y(10), 10, c_white, 2);

			al_flip_display();

			if (collision) {
				break;
			}
		}
	}

	// LIMPEZA DE MEMORIA
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
	al_destroy_timer(timer);
	al_shutdown_primitives_addon();
}
