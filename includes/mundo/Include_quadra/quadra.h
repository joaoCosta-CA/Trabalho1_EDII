/**
 * @file quadra.h
 * @author João Costa
 * @brief Módulo de representação de uma Quadra (quarteirão) no sistema.
 *
 * Define o tipo opaco @c Quadra e as funções de criação, destruição
 * e acesso (getters) aos seus atributos geométricos e de estilização.
 * Cada quadra é identificada por um CEP único e possui uma geometria
 * retangular com cores de preenchimento e borda configuráveis.
 */

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

/**
 * @brief Retorna o CEP identificador da quadra.
 * @param quadra Instância da Quadra.
 * @return String com o CEP.
 */
const char* quadra_get_cep(Quadra quadra);

/**
 * @brief Retorna a coordenada X da quadra.
 * @param quadra Instância da Quadra.
 * @return Valor de X.
 */
double quadra_get_x(Quadra quadra);

/**
 * @brief Retorna a coordenada Y da quadra.
 * @param quadra Instância da Quadra.
 * @return Valor de Y.
 */
double quadra_get_y(Quadra quadra);

/**
 * @brief Retorna a largura da quadra.
 * @param quadra Instância da Quadra.
 * @return Valor de largura (w).
 */
double quadra_get_w(Quadra quadra);

/**
 * @brief Retorna a altura da quadra.
 * @param quadra Instância da Quadra.
 * @return Valor de altura (h).
 */
double quadra_get_h(Quadra quadra);

/**
 * @brief Retorna a espessura da borda da quadra.
 * @param quadra Instância da Quadra.
 * @return Valor de stroke-width (sw).
 */
double quadra_get_sw(Quadra quadra);

/**
 * @brief Retorna a cor de preenchimento da quadra.
 * @param quadra Instância da Quadra.
 * @return String com a cor (ex: "blue", "#RRGGBB").
 */
const char* quadra_get_cfill(Quadra quadra);

/**
 * @brief Retorna a cor da borda da quadra.
 * @param quadra Instância da Quadra.
 * @return String com a cor (ex: "black", "#RRGGBB").
 */
const char* quadra_get_cstrk(Quadra quadra);

#endif // QUADRA_H
