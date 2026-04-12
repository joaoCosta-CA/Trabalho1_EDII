/**
 * @file parametros.h
 * @author João Henrique
 * @brief Módulo para leitura e validação dos parâmetros de linha de comando do programa TED.
 *
 * Parâmetros aceitos:
 *   -e dir        Diretório-base de entrada (BED) [opcional]
 *   -f arq.geo    Arquivo de descrição da cidade  [obrigatório]
 *   -o dir        Diretório-base de saída (BSD)   [obrigatório]
 *   -q arq.qry    Arquivo de consultas             [opcional]
 *   -pm arq.pm    Arquivo de pessoas/moradores     [opcional]
 */

#ifndef PARAMETROS_H
#define PARAMETROS_H

/**
 * @brief Tipo opaco para os parâmetros do programa.
 */
typedef void *Parametros;

/**
 * @brief Faz o parsing dos argumentos de linha de comando.
 *
 * Valida que os parâmetros obrigatórios (-f e -o) estão presentes.
 * Se houver erro, imprime mensagem de uso e retorna NULL.
 *
 * @param argc Quantidade de argumentos (do main).
 * @param argv Vetor de argumentos (do main).
 * @return Instância de Parametros ou NULL se inválido.
 */
Parametros parametros_parse(int argc, char *argv[]);

/**
 * @brief Retorna o diretório-base de entrada (BED).
 * Pode retornar string vazia ("") se -e não foi fornecido.
 */
const char *parametros_get_entrada(Parametros p);

/**
 * @brief Retorna o nome do arquivo .geo (relativo ao BED).
 */
const char *parametros_get_geo(Parametros p);

/**
 * @brief Retorna o diretório-base de saída (BSD).
 */
const char *parametros_get_saida(Parametros p);

/**
 * @brief Retorna o nome do arquivo .qry (relativo ao BED), ou NULL se não informado.
 */
const char *parametros_get_qry(Parametros p);

/**
 * @brief Retorna o nome do arquivo .pm (relativo ao BED), ou NULL se não informado.
 */
const char *parametros_get_pm(Parametros p);

/**
 * @brief Imprime um resumo dos parâmetros carregados (para depuração).
 */
void parametros_print(Parametros p);

/**
 * @brief Libera a memória alocada para os parâmetros.
 */
void parametros_destroy(Parametros p);

#endif // PARAMETROS_H
