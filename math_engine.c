#include "math_engine.h"
#include <math.h>

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
	if (b <= 0.0f) return c; 
	return (a * powf(b, x)) + c;
}

// 4. LOGARÍTMICA: y = a * ln(b*x + 1) + c
// Nota: Usa-se (b*x + 1) no lugar de (x) para evitar ln(0) = -infinito na saída (em x = 0, ln(1) = 0, partindo exatamente da altura 'c').
// - 'a' (Escala vertical): Controla o quão alto o gancho sobe antes de estabilizar quase na horizontal.
// - 'b' (Taxa de compressão interna): Controla a velocidade da subida inicial logo ao sair do arco.
// - 'c' (Altura inicial): Ponto exato de partida no eixo Y quando x = 0.
float calc_logarithmic(float x, float a, float b, float c) {
	if ((b * x + 1.0f) <= 0.0f) return 0.0f;
	return (a * logf((b * x) + 1.0f)) + c;
}

// 5. SENOIDAL: y = a * sin(b*x) + c
// - 'a' (Amplitude): Altura máxima das cristas e vales da onda em relação ao eixo central (a onda oscila entre c - a e c + a).
// - 'b' (Frequência Angular): Velocidade da oscilação. Quanto maior o 'b', mais curvas completas (sobe e desce) a flecha faz no ar.
// - 'c' (Linha Média / Offset Vertical): Altura central em torno da qual a onda oscila (também é a altura de saída em x = 0).
float calc_sinusoidal(float x, float a, float b, float c) {
	return (a * sinf(b * x)) + c;
}


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