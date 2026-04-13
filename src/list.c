#include <stdio.h>
#include <stdlib.h>
#include "estr_dados/Include_list/list.h"

/* Definição das estruturas */
typedef struct node {
    Item item;// Ponteiro genérico para o dado (Círculo, Retângulo, etc)
    struct node *next;
    struct node *prev;
} Node;

typedef struct list {
    Node *head;
    Node *tail;
    int length;
} StList;

Lista createList() {
    StList *new_list = (StList *) malloc(sizeof(StList));
    if (new_list) {
        new_list->head = NULL;
        new_list->tail = NULL;
        new_list->length = 0;
    }
    return (Lista) new_list;
}

Posic insert(Lista l, Item item) {
    StList *list = (StList *) l;
    Node *new_node = (Node *) malloc(sizeof(Node));
    if (!new_node) return NULL;

    new_node->item = item;
    new_node->next = NULL;
    new_node->prev = list->tail;

    if (list->length == 0) {
        list->head = new_node;
    } else {
        list->tail->next = new_node;
    }

    list->tail = new_node;
    list->length++;
    return (Posic) new_node;
}

Item get(Lista l, Posic p) {
    StList *list = (StList *) l;

    if(!list) return NULL;

    Node *node = (Node *) p;
    if (!node) return NULL;
    return node->item;
}

Posic getFirst(Lista l) {
    StList *list = (StList *) l;

    if(!list) return NULL;

    return (Posic) list->head;
}

Posic getNext(Lista l, Posic p) {
    (void) l;

    Node *node = (Node *) p;
    if (!node) return NULL;
    return (Posic) node->next;
}

Posic getLast(Lista l) {
    StList *list = (StList *) l;

    if(!list) return NULL;

    return (Posic) list->tail;
}


void killList(Lista l, void (*freeItem)(void*)) {
    StList *list = (StList *) l;
    if(!list) return;
    Node *current = list->head;
    Node *next_node;

    while (current != NULL) {
        next_node = current->next;
        
        if (freeItem != NULL && current->item != NULL) {
            freeItem(current->item);
        }
        
        free(current);
        current = next_node;
    }
    free(list); // Libera a estrutura da lista
}

int length(Lista l) {
    if (l == NULL) return 0;
    StList *list = (StList *) l;
    return list->length;
}

Item removePosic(Lista l, Posic p) {
    if (l == NULL || p == NULL) return NULL;
    
    StList *list = (StList *) l;
    Node *node = (Node *) p;
    Item item = node->item; // Salva o item para retornar
    
    // Caso 1: O nó é o primeiro da lista (HEAD)
    if (node == list->head) {
        list->head = node->next;
        if (list->head != NULL) {
            list->head->prev = NULL;
        } else {
            // Se a lista ficou vazia, tail também deve ser NULL
            list->tail = NULL;
        }
    }
    // Caso 2: O nó não é o primeiro (tem anterior)
    else {
        node->prev->next = node->next;
    }

    // Caso 3: O nó é o último da lista (TAIL)
    if (node == list->tail) {
        list->tail = node->prev;
        if (list->tail != NULL) {
            list->tail->next = NULL;
        } else {
             // Caso já tratado no head, mas reforçando: lista vazia
             list->head = NULL;
        }
    }
    // Caso 4: O nó não é o último (tem próximo)
    else if (node->next != NULL) {
        node->next->prev = node->prev;
    }

    free(node); // Libera apenas a estrutura do nó, não o item
    list->length--;
    
    return item;
}