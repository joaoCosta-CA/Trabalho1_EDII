/**
 * @file leitorQry.h
 * @author João Costa
 * @brief Módulo de leitura e processamento de arquivos .qry.
 *
 * Este módulo implementa o parser para arquivos de consulta (.qry), que
 * contêm comandos de modificação e consulta sobre o estado do mundo
 * (quadras e habitantes) da cidade simulada.
 *
 * Comandos suportados:
 *  - rq  cep               : Consulta quadra por CEP.
 *  - pq  cep               : Consulta pessoas residentes em uma quadra.
 *  - censo                 : Gera relatório censitário completo.
 *  - h?  cpf               : Consulta endereço de um habitante.
 *  - nasc cpf nome sob s d : Registra nascimento de uma nova pessoa.
 *  - rip cpf               : Registra óbito de um habitante.
 *  - mud cpf cep face num c: Registra mudança de endereço de um habitante.
 *  - dspj cep              : Despeja todos os moradores de uma quadra.
 */

#ifndef LEITOR_QRY_H
#define LEITOR_QRY_H

#include "estr_dados/Include_HashFile/HashFile.h"

/**
 * @brief Tipo opaco que encapsula o resultado do processamento do .qry.
 *        Contém a lista de decorações SVG geradas durante a leitura.
 */
typedef void* LeitorQry;

/**
 * @brief Lê e processa um arquivo .qry, executando cada comando sobre
 *        os HashFiles de quadras e pessoas.
 *
 * Para cada comando processado, pode gerar anotações SVG (decorações)
 * e escrever saída textual no arquivo de relatório correspondente.
 *
 * @param diretorio_base Diretório-base de entrada onde o arquivo .qry está localizado.
 * @param nome_arquivo   Nome do arquivo .qry a ser lido (sem o caminho).
 * @param hf_quadras     HashFile contendo as quadras da cidade.
 * @param hf_pessoas     HashFile contendo as pessoas cadastradas.
 * @param dir_saida      Diretório onde o relatório de saída (.txt) será salvo.
 * @param nome_base      Nome base (sem extensão) usado para nomear o arquivo de saída.
 * @return               Instância de LeitorQry com a lista de decorações SVG geradas,
 *                       ou NULL em caso de erro de abertura do arquivo.
 */
LeitorQry leitor_qry(const char* diretorio_base, const char* nome_arquivo,
                     HashFile hf_quadras, HashFile hf_pessoas,
                     const char* dir_saida, const char* nome_base);

#endif // LEITOR_QRY_H