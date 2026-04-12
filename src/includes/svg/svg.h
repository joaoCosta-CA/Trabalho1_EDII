#ifndef SVG_H
#define SVG_H

#include "../estr_dados/Include_HashFile/HashFile.h"
#include <stdio.h>

/**
 * @brief Inicia o arquivo SVG com as tags XML base.
 * @param caminho_arquivo Caminho completo (ex: "saida/teste-quadras.svg").
 * @return Retorna o ponteiro FILE* aberto para uso, ou NULL em caso de erro.
 */
FILE* svg_iniciar(const char* caminho_arquivo);

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

#endif // SVG_H
