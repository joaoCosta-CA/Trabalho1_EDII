#ifndef LEITOR_QRY_H
#define LEITOR_QRY_H

/**
*@brief: Módulo de para a implementação da leitura dos arquivos que conteram as modificações do 'mundo' e das pessoas.
 */

#include "estr_dados/Include_HashFile/HashFile.h"

typedef void* LeitorQry;

/**
*@brief Função que será usada para a leitura dos comandos dos arquivos do .qry.
*@param diretorio_base: caminho do arquivo que será usado para a consulta.
*@param hf_pessoas: HashFile que contém as pessoas para alteração e consulta.
*@param dir_saida: diretório onde será salvo o relatório de output.
*@param nome_base: nome base derivado do arquivo geo para geração do relatório txt.
*/
LeitorQry leitor_qry(const char* diretorio_base, const char* nome_arquivo, HashFile hf_quadras, HashFile hf_pessoas, const char* dir_saida, const char* nome_base);

#endif // LEITOR_QRY_H