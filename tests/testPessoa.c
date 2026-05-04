/**
 * @file testPessoa.c
 * @brief Testes unitários para o módulo Pessoa.
 *
 * Cobre: criação, todos os getters, campos vazios, destruição com NULL,
 * e segurança de todos os getters quando pessoa é NULL.
 */
#include "unity/unity.h"
#include "mundo/Include_pessoa/pessoa.h"
#include <stdio.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ─── Criação e getters ───────────────────────────────────────────────────── */

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

void test_PessoaSexoFeminino(void) {
    Pessoa p = criar_pessoa("000.000.001-00", "Maria", "Souza", 'F', "15/06/2000");
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT('F', pessoa_get_sexo(p));
    destruir_pessoa(p);
}

/* ─── Campos vazios ───────────────────────────────────────────────────────── */

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

/* ─── Getters com pessoa NULL não crasham ─────────────────────────────────── */

void test_GettersComPessoaNullSaoSeguros(void) {
    TEST_ASSERT_NULL(pessoa_get_cpf(NULL));
    TEST_ASSERT_NULL(pessoa_get_nome(NULL));
    TEST_ASSERT_NULL(pessoa_get_sobrenome(NULL));
    TEST_ASSERT_EQUAL_INT('\0', pessoa_get_sexo(NULL));
    TEST_ASSERT_NULL(pessoa_get_data_nascimento(NULL));
}

/* ─── Destruição segura com NULL ─────────────────────────────────────────── */

void test_DestruirPessoaNullNaoCrasha(void) {
    destruir_pessoa(NULL); /* não deve crashar */
}

/* ─── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_CriarPessoaSuccess);
    RUN_TEST(test_PessoaSexoFeminino);
    RUN_TEST(test_PessoaEmptyFields);
    RUN_TEST(test_GettersComPessoaNullSaoSeguros);
    RUN_TEST(test_DestruirPessoaNullNaoCrasha);
    return UNITY_END();
}
