/**
 * @file leitorPM.h
 * @author João Costa
 * @brief Módulo de leitura do arquivo .pm (Pessoas e Moradores).
 *
 * Processa o arquivo de entrada .pm, que contém registros de criação
 * e atualização de pessoas e seus endereços residenciais no sistema.
 */
#ifndef LEITOR_PM_H
#define LEITOR_PM_H

#include "../../estr_dados/Include_HashFile/HashFile.h"

/**
 * @brief Lê o arquivo .pm e insere/atualiza os habitantes no HashFile.
 * 
 * Processa comandos:
 * - p cpf nome sobrenome sexo nasc
 * - m cpf cep face num compl
 * 
 * @param diretorio_base Diretório-base de entrada (-e)
 * @param nome_arquivo Nome do arquivo .pm
 * @param hf_pessoas HashFile onde as pessoas serão cadastradas / atualizadas
 */
void ler_arquivo_pm(const char *diretorio_base, const char *nome_arquivo, HashFile hf_pessoas);

#endif // LEITOR_PM_H
