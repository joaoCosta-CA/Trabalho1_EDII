#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pessoa.h"

typedef struct {
    char cpf[18];
    char nome[50];
    char sobrenome[50];
    char sex;
    char data_nascimento[15];
} InternalPessoa;

Pessoa criar_pessoa(const char* cpf, const char* nome, const char* sobrenome, char sex, const char* data_nascimento) {
    InternalPessoa* p = (InternalPessoa*) malloc(sizeof(InternalPessoa));
    if (p != NULL) {
        strncpy(p->cpf, cpf, 17);
        p->cpf[17] = '\0';
        
        strncpy(p->nome, nome, 49);
        p->nome[49] = '\0';
        
        strncpy(p->sobrenome, sobrenome, 49);
        p->sobrenome[49] = '\0';
        
        p->sex = sex;
        
        strncpy(p->data_nascimento, data_nascimento, 14);
        p->data_nascimento[14] = '\0';
    }
    return (Pessoa) p;
}

void destruir_pessoa(Pessoa pessoa) {
    if (pessoa != NULL) {
        free(pessoa);
    }
}

const char* pessoa_get_cpf(Pessoa pessoa) {
    if (pessoa == NULL) return NULL;
    return ((InternalPessoa*)pessoa)->cpf;
}

const char* pessoa_get_nome(Pessoa pessoa) {
    if (pessoa == NULL) return NULL;
    return ((InternalPessoa*)pessoa)->nome;
}

const char* pessoa_get_sobrenome(Pessoa pessoa) {
    if (pessoa == NULL) return NULL;
    return ((InternalPessoa*)pessoa)->sobrenome;
}

char pessoa_get_sexo(Pessoa pessoa) {
    if (pessoa == NULL) return '\0';
    return ((InternalPessoa*)pessoa)->sex;
}

const char* pessoa_get_data_nascimento(Pessoa pessoa) {
    if (pessoa == NULL) return NULL;
    return ((InternalPessoa*)pessoa)->data_nascimento;
}