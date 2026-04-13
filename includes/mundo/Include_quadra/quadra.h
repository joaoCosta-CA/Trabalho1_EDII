#ifndef QUADRA_H
#define QUADRA_H

/**
 * @brief Tipo opaco para representar uma Quadra.
 */
typedef void* Quadra;

/**
 * @brief Cria uma nova quadra.
 * @param cep: CEP identificador da quadra.
 * @param x: Posição X da quadra.
 * @param y: Posição Y da quadra.
 * @param width: Largura (w) da quadra.
 * @param height: Altura (h) da quadra.
 * @param sw: Espessura da borda.
 * @param cfill: Cor de preenchimento.
 * @param cstrk: Cor da borda.
 * @return Retorna a Quadra criada.
 */
Quadra criar_quadra(const char* cep, double x, double y, double width, double height, double sw, const char* cfill, const char* cstrk);

/**
 * @brief Destroi a quadra, liberando sua memória.
 */
void destruir_quadra(Quadra quadra);

/* Getters */
const char* quadra_get_cep(Quadra quadra);
double quadra_get_x(Quadra quadra);
double quadra_get_y(Quadra quadra);
double quadra_get_w(Quadra quadra);
double quadra_get_h(Quadra quadra);
double quadra_get_sw(Quadra quadra);
const char* quadra_get_cfill(Quadra quadra);
const char* quadra_get_cstrk(Quadra quadra);

#endif // QUADRA_H
