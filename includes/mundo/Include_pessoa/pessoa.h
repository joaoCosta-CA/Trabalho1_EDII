#ifndef PESSOA_H
#define PESSOA_H

#include <stdio.h>

/**
 * @brief Tipo opaco para representar uma Pessoa.
 */
typedef void* Pessoa;

/**
 * @brief Cria uma pessoa
 * @param cpf: string que identifica a pessoa criada.
 * @param nome: string que identifica o nome da pessoa.
 * @param sobrenome: string que identifica o sobrenome da pessoa.
 * @param sex: caractere que identifica o sexo da pessoa.
 * @param data_nascimento: string que identifica a data de nascimento da pessoa.
 * @return Pessoa: pessoa criada.
 */
Pessoa criar_pessoa(const char* cpf, const char* nome, const char* sobrenome, char sex, const char* data_nascimento);

/**
 * @brief Destroi uma pessoa, liberando sua memória.
 * @param pessoa: pessoa a ser destruída.
 */
void destruir_pessoa(Pessoa pessoa);

/**
 * @brief Retorna o CPF da pessoa.
 */
const char* pessoa_get_cpf(Pessoa pessoa);

/**
 * @brief Retorna o nome da pessoa.
 */
const char* pessoa_get_nome(Pessoa pessoa);

/**
 * @brief Retorna o sobrenome da pessoa.
 */
const char* pessoa_get_sobrenome(Pessoa pessoa);

/**
 * @brief Retorna o sexo da pessoa.
 */
char pessoa_get_sexo(Pessoa pessoa);

/**
 * @brief Retorna a data de nascimento da pessoa.
 */
const char* pessoa_get_data_nascimento(Pessoa pessoa);

#endif // PESSOA_H