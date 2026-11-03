#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/mouse.h>

#include "../include/game_config.h"
#include "../include/math_engine.h"
#include "../include/sprites_maneger.h"
#include "../include/param_selector.h"

typedef struct Arrow {
    float pos_x;
    float pos_y;
    float vel;

    float width;
    float height;

    bool is_flying;
} Arrow;

int main(){
    if (!al_init()) {
        printf("ERRO AO INICIALIZAR O ALLEGRO\n");
        return 1;
    }

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    if (!event_queue) return 1;

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
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
    al_install_mouse();

    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(event_queue, al_get_mouse_event_source());
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

    // ========================================================================
    // BOTÕES DE SOMA E SUBTRAÇÃO
    // ========================================================================
        
    Button soma = { al_get_display_width(display) - 70 ,al_get_display_height(display) - 70, 50, 50, ADDITION};

    Button subtracao = { al_get_display_width(display) - 70 ,al_get_display_height(display) - 160, 50, 50, SUBTRACTION };

    // ========================================================================
    // ESCOLHA DA FUNÇÃO E PARÂMETROS INICIAIS
    // ========================================================================

    FunctionParams a = { 0.0f, false, 10, -1};
    FunctionParams b = { 0.0f, false, 10,  -1};
    FunctionParams c = { 0.0f, false, 10, -1};

    FunctionType current_func = FUNC_NONE;

    //float a = 0.0f;
    //float b = 0.0f;
    //float c = 0.0f;

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

    // ========================================================================
    // INICIALIZAÇÃO CENÁRIO
    // ========================================================================

    ALLEGRO_BITMAP* backdground = al_load_bitmap("assets/backgrounds/background_treinamento.jpg");

    // ========================================================================
    // INICIALIZAÇÃO PERSONAGENS
    // ========================================================================
   
    // ROBIN RA

    float chac_width = 50.0f;
    float chac_height = 60.0f;

    ALLEGRO_BITMAP* robin_ra_sprite = al_load_bitmap("assets/sprites/robin_ra.png");

    PlayerSprite robin_ra = {
        robin_ra_sprite,
        chac_width, WORLD_BASE_Y - chac_height,
        100, 100,
        0,
        DEFAULT
    };

    float cont_current_sprite = 0;

    // Resultado da trajetória: 0 = voando/pronto, 1 = acertou, 2 = bateu em obstáculo, 3 = caiu no solo
    int traj_result = 0;

    Arrow arrow = { current_x, current_y, step_x, 25.0f, 50.0f, false };

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

           // ========================================================================
           // VERIFICAÇÃO DO TIPO DE FUNÇÃO
           // ========================================================================

            if (event.keyboard.keycode == ALLEGRO_KEY_1) {
                current_func = FUNC_LINEAR;
                //a = 2.0f; b = 0.0f; c = 0.0f;
                //a.value = 2.0f; b.value = 0.0f; c.value = 0.0f;
                arrow.is_flying = false; current_x = 0.0f;
                current_y = calculate_trajectory(current_func, 0.0f, a.value, b.value, c.value);
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_2) {
                current_func = FUNC_QUADRATIC;
                //a = -1.0f; b = 4.0f; c = 0.0f;
                //a.value = -1.0f; b.value = 4.0f; c.value = 0.0f;
                arrow.is_flying = false; current_x = 0.0f;
                current_y = calculate_trajectory(current_func, 0.0f, a.value, b.value, c.value);
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_3) {
                current_func = FUNC_EXPONENTIAL;
                ///a = 1.0f; b = 2.0f; c = 0.0f;
                //a.value = 1.0f; b.value = 2.0f; c.value = 0.0f;
                arrow.is_flying = false; current_x = 0.0f;
                current_y = calculate_trajectory(current_func, 0.0f, a.value, b.value, c.value);
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_4) {
                current_func = FUNC_LOGARITHMIC;
                //a = 3.0f; b = 2.0f; c = 0.0f;
                //a.value = 3.0f; b.value = 2.0f; c.value = 0.0f;
                arrow.is_flying = false; current_x = 0.0f;
                current_y = calculate_trajectory(current_func, 0.0f, a.value, b.value, c.value);
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_5) {
                current_func = FUNC_SINUSOIDAL;
                //a = 2.0f; b = 1.0f; c = 4.0f;
                //a.value = 2.0f; b.value = 1.0f; c.value = 4.0f;
                arrow.is_flying = false; current_x = 0.0f;
                current_y = calculate_trajectory(current_func, 0.0f, a.value, b.value, c.value);
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                if (current_func != FUNC_NONE || calculate_trajectory(current_func, 0.0f, a.value, b.value, c.value) != 0) {
                    current_x = 0.0f;
                    current_y = calculate_trajectory(current_func, 0.0f, a.value, b.value, c.value);
                    traj_result = 0;
                    arrow.is_flying = true;
                }
            }

            // ========================================================================
            // VERIFICAÇÃO DE QUAL PARAMETRO O USUARIO VAI ALTERAR
            // ========================================================================

            if (event.keyboard.keycode == ALLEGRO_KEY_A){
                a.state = !a.state;
                if (a.state) {
                    b.state = false;
                    c.state = false;
                }
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_B) {
                b.state = !b.state;
                if (b.state) {
                    a.state = false;
                    c.state = false;
                }
            }

            if (event.keyboard.keycode == ALLEGRO_KEY_C) {
                c.state = !c.state;
                if (c.state) {
                    a.state = false;
                    b.state = false;
                }
            }
        }

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;

            if (arrow.is_flying) {
                current_x += step_x;
                current_y = calculate_trajectory(current_func, current_x, a.value, b.value, c.value);

                // 1. Checagem de Colisão com o Obstáculo
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
                    current_y = calculate_trajectory(current_func, current_x, a.value, b.value, c.value);
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

        // ========================================================================
        // ALTERAÇÃO DOS PARAMETROS
        // ========================================================================

        if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
            if (event.mouse.button == 1) {
                float mouse_x = (float)event.mouse.x;
                float mouse_y = (float)event.mouse.y;

                if (mouse_verificator(soma, mouse_x, mouse_y)) {
                    if (a.state)
                        param_operation(&a, soma);
                   
                    if (b.state) param_operation(&b, soma);
                    if (c.state) param_operation(&c, soma);
                }

                if (mouse_verificator(subtracao, mouse_x, mouse_y)) {
                    if (a.state) param_operation(&a, subtracao);
                    if (b.state) param_operation(&b, subtracao);
                    if (c.state) param_operation(&c, subtracao);
                }
            }
        }

        if (redraw && al_is_event_queue_empty(event_queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb(15, 15, 20));

            if (backdground) {
                al_draw_bitmap(backdground, 0, 0, 0);
            }

            // HUD NO TOPO DA TELA
            const char* func_names[] = { "SELECIONE UMA DAS FUNCOES", "LINEAR", "QUADRATICA", "EXPONENCIAL", "LOGARITMICA", "SENOIDAL" };
            al_draw_filled_rectangle(15, 17, SCREEN_WIDTH - 50, 35, c_black);
            al_draw_textf(font, c_white, 20, 20, 0, "Teclas 1-5 Mudar Funcao | [ESPACO] Disparar");

            al_draw_filled_rectangle(15, 37, SCREEN_WIDTH - 50, 55, c_black);
            al_draw_textf(font, (current_func == FUNC_NONE ? c_red : c_yellow), 20, 40, 0, "Modelo Ativo: %s  (a=%.1f, b=%.1f, c=%.1f)", func_names[current_func], a.value, b.value, c.value);

            al_draw_filled_rectangle(15, 57, SCREEN_WIDTH - 50, 75, c_black);

            if(a.state)
                al_draw_textf(font, c_green, 20, 60, 0, "Parametro Ativo: (a=%.1f)", a.value);
            if(b.state)
                al_draw_textf(font, c_green, 20, 60, 0, "Parametro Ativo: (b=%.1f)", b.value);
            if(c.state)
                al_draw_textf(font, c_green, 20, 60, 0, "Parametro Ativo: (c=%.1f)", c.value);

            if (!a.state && !b.state && !c.state)
                al_draw_textf(font, c_white, 20, 60, 0, "Nenhum Parametro Selecionado");


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

            //DESENHAR ROBIN RA E CONTADOR DE ESTADO

            if (cont_current_sprite >= FPS / 3) {
                cont_current_sprite = 0;
                robin_ra.current_sprite++;
            }

            if (robin_ra.current_sprite == 8)
                robin_ra.current_sprite = 0;

            draw_player(robin_ra);
            cont_current_sprite++;


            //DESENHAR BOTOES DE SUB E SOM
                
            al_draw_filled_rectangle(soma.position_x, soma.position_y, soma.position_x + soma.widht, soma.position_y + soma.height, c_white);

            al_draw_filled_rectangle(subtracao.position_x, subtracao.position_y, subtracao.position_x + subtracao.widht, subtracao.position_y + subtracao.height, c_white);

            // PROJÉTIL
            ALLEGRO_COLOR arrow_color = c_white;
            if (traj_result == 1) arrow_color = c_green;
            else if (traj_result == 2 || traj_result == 3) arrow_color = c_red;

            float screen_x = scale_to_screen_x(current_x);
            float screen_y = scale_to_screen_y(current_y);
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