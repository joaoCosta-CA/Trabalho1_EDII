/**
 * @file leitor_geo.h
 * @author João Costa
 * @brief Módulo de Leitura de Arquivos .geo
 * 
 * Este módulo implementa o parser para arquivos de descrição geométrica (.geo).
 * O arquivo .geo contém definições de formas geométricas (círculos, retângulos,
 * linhas, textos) e estilos de texto que serão renderizados no SVG.
 * 
 * Comandos suportados:
 * - c: Círculo (id, x, y, raio, corb, corp)
 * - r: Retângulo (id, x, y, largura, altura, corb, corp)
 * - l: Linha (id, x1, y1, x2, y2, cor)
 * - t: Texto (id, x, y, corb, corp, ancora, conteudo)
 * - ts: Estilo de texto (familia, peso, tamanho)
 * 
 * @note Linhas começando com '#' são ignoradas (comentários).
 * @note Atualiza max_id com o maior ID encontrado para evitar colisões.
 * 
 * @author João Costa
 */

#ifndef LEITOR_GEO_H
#define LEITOR_GEO_H

#include "../../estr_dados/Include_HashFile/HashFile.h"

/**
 * @brief Lê e processa um arquivo de descrição geométrica (.geo).
 * 
 * Abre o arquivo especificado, interpreta os comandos de criação de
 * formas geométricas e insere cada forma criada na lista fornecida.
 * 
 * @param diretorio_base Diretório onde está localizado o arquivo.
 * @param nome_arquivo   Nome do arquivo .geo a ser lido (sem o caminho).
 * @param hash_quadras   HashFile onde as quadras lidas serão inseridas (deve estar aberto).
 */
void ler_arquivo_geo(const char *diretorio_base, const char *nome_arquivo, HashFile hash_quadras);

#endif