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
*@param nome_arquivo: nome do arquivo que será usado para a consulta.
*@param hf_quadras: HashFile que contém as quadras para alteração.
*/
LeitorQry leitor_qry(const char* diretorio_base, const char* nome_arquivo, HashFile hf_quadras);

#endif // LEITOR_QRY_H