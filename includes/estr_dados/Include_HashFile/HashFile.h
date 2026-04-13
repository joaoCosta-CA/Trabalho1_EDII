/**
 * @file HashFile.h
 * @author João Henrique
 * @brief Interface Oca (Strict Opaque) para Hash Extensível com chaves string.
 * Utiliza retornos inteiros (1 para Sucesso, 0 para Falha) conforme padrão C.
 */

#ifndef HASH_FILE_H
#define HASH_FILE_H

typedef void* HashFile;

/**
 * @brief Inicializa a estrutura de Hash Extensível.
 * @param dat_path: Caminho para o arquivo de dados (.dat).
 * @param idx_path: Caminho para o arquivo de índice (.idx).
 * @return Ponteiro para a instância ou NULL em caso de erro de abertura.
 */
HashFile hash_open(const char* dat_path, const char* idx_path);

/**
 * @brief Finaliza a instância, salva o índice e limpa a memória.
 * @param hf Instância a ser fechada.
 */
void hash_close(HashFile hf);

/**
 * @brief Insere um registro baseado em chave string e valor de texto.
 * @param hf: Instância do hashFile.
 * @param key: Chave string para o registro (ex: CPF, CEP).
 * @param value: Valor associado a chave.
 * @return 1 se a operação foi bem-sucedida, 0 caso contrário.
 */
int hash_insert(HashFile hf, const char* key, const char* value);

/**
 * @brief Busca um valor associado a uma chave string.
 * @param hf: Instância do HashFile.
 * @param key: Chave de busca (string).
 * @param out_buffer: Ponteiro para o array onde o valor será copiado (pode ser NULL para apenas verificar existência).
 * @param buffer_size: Tamanho do buffer de saída para segurança.
 * @return 1 se a chave foi encontrada, 0 caso contrário.
 */
int hash_search(HashFile hf, const char* key, char* out_buffer, int buffer_size);

/**
 * @brief Remove o registro correspondente à chave informada.
 * @param hf: Instância do HashFile.
 * @param key: Chave string do registro a ser removido.
 * @return 1 se o registro foi removido, 0 se não foi encontrado.
 */
int hash_remove(HashFile hf, const char* key);

/**
 * @brief Retorna a profundidade global atual do diretório.
 * @param hf: Instância do HashFile.
 * @return Valor inteiro da profundidade global.
 */
int hash_get_global_depth(HashFile hf); // ---------> colocar como função static, não faz sentido o usuário acessar isso.

/**
 * @brief Atualiza o valor associado com a chave fornecida.
 * @param hf: Instância da HashFile.
 * @param key: Chave string do registro a ser atualizado.
 * @param new_value: Novo valor a ser associado com a chave.
 * @return 1 se a atualização foi bem sucedida, 0 se a chave não foi encontrada ou ocorreu algum erro.
 */
int hash_update(HashFile hf, const char* key, const char *new_value);

/**
 * @brief Percorre todos os registros ativos da Hash.
 * @param hf: Instância do HashFile.
 * @param callback Função que será chamada para cada registro:
 * recebe a chave (string), o ponteiro para o dado, e o contexto.
 * @param userdata Ponteiro arbitrário repassado para o callback.
 */
void hash_forall(HashFile hf, void (*callback)(const char* key, const char* value, void *userdata), void *userdata);

#endif // HASH_FILE_H