#include "unity/unity.h"
#include "mundo/Include_pessoa/pessoa.h"
#include <stdio.h>
#include <string.h>


void setUp(void) {
    // Inicialização antes de cada teste, se necessário.
}

void tearDown(void) {
    // Limpeza após cada teste, se necessário.
}

// --- Testes Unitários ---

void test_CriarPessoaSuccess(void) {
    Pessoa p = criar_pessoa("123.456.789-00", "João", "Silva", 'M', "01/01/1990");
    TEST_ASSERT_NOT_NULL(p);
    
    TEST_ASSERT_EQUAL_STRING("123.456.789-00", pessoa_get_cpf(p));
    TEST_ASSERT_EQUAL_STRING("João", pessoa_get_nome(p));
    TEST_ASSERT_EQUAL_STRING("Silva", pessoa_get_sobrenome(p));
    TEST_ASSERT_EQUAL_INT('M', pessoa_get_sexo(p));
    TEST_ASSERT_EQUAL_STRING("01/01/1990", pessoa_get_data_nascimento(p));
    
    destruir_pessoa(p);
}

void test_PessoaEmptyFields(void) {
    Pessoa p = criar_pessoa("", "", "", ' ', "");
    TEST_ASSERT_NOT_NULL(p);
    
    TEST_ASSERT_EQUAL_STRING("", pessoa_get_cpf(p));
    TEST_ASSERT_EQUAL_STRING("", pessoa_get_nome(p));
    TEST_ASSERT_EQUAL_STRING("", pessoa_get_sobrenome(p));
    TEST_ASSERT_EQUAL_INT(' ', pessoa_get_sexo(p));
    TEST_ASSERT_EQUAL_STRING("", pessoa_get_data_nascimento(p));
    
    destruir_pessoa(p);
}


// --- Funcao Principal de Testes ---
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_CriarPessoaSuccess);
    RUN_TEST(test_PessoaEmptyFields);
    
    return UNITY_END();
}
