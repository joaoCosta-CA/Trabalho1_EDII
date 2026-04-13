#ifndef SVG_H
#define SVG_H

#include "../estr_dados/Include_HashFile/HashFile.h"
#include <stdio.h>

/**
 * @brief Calcula a bounding box (caixa delimitadora) de todas as quadras.
 * @param hf_quadras HashFile de quadras.
 * @param min_x Ponteiro para retornar o min_x
 * @param min_y Ponteiro para retornar o min_y
 * @param max_x Ponteiro para retornar o max_x
 * @param max_y Ponteiro para retornar o max_y
 */
void svg_calcular_bbox(HashFile hf_quadras, double *min_x, double *min_y, double *max_x, double *max_y);

/**
 * @brief Inicia o arquivo SVG com as tags XML base.
 * @param caminho_arquivo Caminho completo (ex: "saida/teste-quadras.svg").
 * @param min_x Valor min-x do viewBox
 * @param min_y Valor min-y do viewBox
 * @param w Largura do viewBox
 * @param h Altura do viewBox
 * @return Retorna o ponteiro FILE* aberto para uso, ou NULL em caso de erro.
 */
FILE* svg_iniciar(const char* caminho_arquivo, double min_x, double min_y, double w, double h);

/**
 * @brief Finaliza o SVG fechando a tag </svg> e o arquivo.
 * @param f Ponteiro do arquivo SVG retornado pelo `svg_iniciar`.
 */
void svg_fechar(FILE* f);

/**
 * @brief Itera sobre o HashFile de quadras, lendo e imprimindo a representação <rect> de cada uma.
 * @param hf_quadras Ponteiro para o HashFile dinâmico populado no .geo
 * @param svg_file Arquivo SVG aberto
 */
void svg_gerar_quadras(HashFile hf_quadras, FILE* svg_file);

/**
 * @brief Itera sobre uma lista de strings SVG e escreve no final do arquivo.
 * @param decoracoes Lista de strings
 * @param f Arquivo SVG aberto
 */
void svg_gerar_decoracoes(void* decoracoes, FILE* f);

#endif // SVG_H
