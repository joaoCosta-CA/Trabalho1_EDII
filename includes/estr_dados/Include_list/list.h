/**
 * @file lista.h
 * @brief Módulo de Lista Duplamente Encadeada Genérica
 * 
 * Este módulo implementa uma estrutura de dados do tipo lista duplamente
 * encadeada genérica utilizando ponteiros opacos (void*). A lista pode
 * armazenar qualquer tipo de dado, desde formas geométricas até segmentos.
 * 
 * @note A estrutura interna é encapsulada - o usuário interage apenas
 *       através das funções públicas da API.
 * @note A lista não assume a propriedade dos itens armazenados;
 *       o usuário é responsável pela liberação de memória quando necessário.
 * 
 * @author João Costa
 */

#ifndef LISTA_H
#define LISTA_H

/**
 * @brief Tipo opaco para a lista.
 * O usuário manipula apenas ponteiros void*, sem conhecer a estrutura interna.
 */
typedef void *Lista;

/**
 * @brief Tipo opaco para posição na lista (iterador).
 */
typedef void *Posic;

/**
 * @brief Tipo opaco para item armazenado na lista.
 */
typedef void *Item;

/**
 * @brief Cria uma nova lista vazia.
 * @return Ponteiro para a lista criada, ou NULL se falhar
 */
Lista createList();

/**
 * @brief Insere um elemento no final da lista.
 * @param l Lista onde inserir
 * @param item Ponteiro para o dado a inserir
 * @return Posição do item inserido na lista
 */
Posic insert(Lista l, Item item);

/**
 * @brief Remove o elemento na posição especificada.
 * @param l Lista de onde remover
 * @param p Posição do elemento a remover
 * @return O item removido (para liberação pelo usuário)
 */
Item removePosic(Lista l, Posic p);

/**
 * @brief Retorna o número de elementos na lista.
 * @param l Lista a consultar
 * @return Quantidade de elementos
 */
int length(Lista l);

/**
 * @brief Obtém a primeira posição da lista.
 * @param l Lista a consultar
 * @return Posição do primeiro elemento, ou NULL se vazia
 */
Posic getFirst(Lista l);

/**
 * @brief Obtém a próxima posição na lista.
 * @param l Lista a consultar
 * @param p Posição atual
 * @return Próxima posição, ou NULL se fim da lista
 */
Posic getNext(Lista l, Posic p);

/**
 * @brief Obtém a última posição da lista.
 * @param l Lista a consultar
 * @return Posição do último elemento, ou NULL se vazia
 */
Posic getLast(Lista l);

/**
 * @brief Obtém a posição anterior na lista.
 * @param l Lista a consultar
 * @param p Posição atual
 * @return Posição anterior, ou NULL se início da lista
 */
Posic getPrevious(Lista l, Posic p);

/**
 * @brief Obtém o item armazenado na posição.
 * @param l Lista a consultar
 * @param p Posição do item
 * @return Ponteiro para o item armazenado
 */
Item get(Lista l, Posic p);

/**
 * @brief Destrói a lista e libera sua memória.
 * 
 * @param l Lista a destruir
 * @param freeItem Função callback para liberar cada item, ou NULL
 *                 se os itens não devem ser liberados
 */
void killList(Lista l, void (*freeItem)(void*));

#endif