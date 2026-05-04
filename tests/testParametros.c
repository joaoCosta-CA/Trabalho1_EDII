/**
 * @file testParametros.c
 * @brief Testes unitários para o módulo Parametros.
 *
 * Cobre: parse completo com todos os flags, flags obrigatórios ausentes,
 * flag -e independente, retorno de NULL para qry/pm quando não fornecidos,
 * e flag inválido.
 */
#include "unity/unity.h"
#include "parametros/parametros.h"
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

/* ─── Parse completo ──────────────────────────────────────────────────────── */

void test_ParametrosParseSuccess(void) {
    char *argv[] = {"ted", "-f", "cidade.geo", "-o", "saida_dir", "-pm", "pessoas.pm"};
    int argc = 7;

    Parametros p = parametros_parse(argc, argv);
    TEST_ASSERT_NOT_NULL(p);

    TEST_ASSERT_EQUAL_STRING("cidade.geo", parametros_get_geo(p));
    TEST_ASSERT_EQUAL_STRING("saida_dir", parametros_get_saida(p));
    TEST_ASSERT_EQUAL_STRING("pessoas.pm", parametros_get_pm(p));
    TEST_ASSERT_NULL(parametros_get_qry(p));   /* -q não foi dado */

    parametros_destroy(p);
}

void test_ParametrosComTodosFlags(void) {
    char *argv[] = {"ted", "-e", "entrada", "-f", "c.geo", "-o", "out",
                    "-q", "c.qry", "-pm", "p.pm"};
    int argc = 11;

    Parametros p = parametros_parse(argc, argv);
    TEST_ASSERT_NOT_NULL(p);

    TEST_ASSERT_EQUAL_STRING("entrada", parametros_get_entrada(p));
    TEST_ASSERT_EQUAL_STRING("c.geo",   parametros_get_geo(p));
    TEST_ASSERT_EQUAL_STRING("out",     parametros_get_saida(p));
    TEST_ASSERT_EQUAL_STRING("c.qry",  parametros_get_qry(p));
    TEST_ASSERT_EQUAL_STRING("p.pm",   parametros_get_pm(p));

    parametros_destroy(p);
}

/* ─── Parâmetros obrigatórios ausentes ────────────────────────────────────── */

void test_ParametrosMissingObrigatorio_F(void) {
    char *argv[] = {"ted", "-o", "saida_dir"};
    Parametros p = parametros_parse(3, argv);
    TEST_ASSERT_NULL(p);
}

void test_ParametrosMissingObrigatorio_O(void) {
    char *argv[] = {"ted", "-f", "cidade.geo"};
    Parametros p = parametros_parse(3, argv);
    TEST_ASSERT_NULL(p);
}

void test_ParametrosMissingObligatory(void) {
    /* Mantido por compatibilidade — equivalente aos dois acima juntos */
    char *argv_missing_f[] = {"ted", "-o", "saida_dir"};
    TEST_ASSERT_NULL(parametros_parse(3, argv_missing_f));

    char *argv_missing_o[] = {"ted", "-f", "cidade.geo"};
    TEST_ASSERT_NULL(parametros_parse(3, argv_missing_o));
}

/* ─── Comportamento de -e quando omitido ─────────────────────────────────── */

void test_ParametrosSemEntrada_RetornaStringVazia(void) {
    char *argv[] = {"ted", "-f", "c.geo", "-o", "out"};
    Parametros p = parametros_parse(5, argv);
    TEST_ASSERT_NOT_NULL(p);

    /* Sem -e, get_entrada deve retornar string vazia (não NULL) */
    const char *entrada = parametros_get_entrada(p);
    TEST_ASSERT_NOT_NULL(entrada);
    TEST_ASSERT_EQUAL_STRING("", entrada);

    parametros_destroy(p);
}

/* ─── Flag desconhecido ───────────────────────────────────────────────────── */

void test_ParametrosFlagDesconhecidoRetornaNull(void) {
    char *argv[] = {"ted", "-f", "c.geo", "-o", "out", "--invalido"};
    Parametros p = parametros_parse(6, argv);
    TEST_ASSERT_NULL(p);
}

/* ─── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ParametrosParseSuccess);
    RUN_TEST(test_ParametrosComTodosFlags);
    RUN_TEST(test_ParametrosMissingObrigatorio_F);
    RUN_TEST(test_ParametrosMissingObrigatorio_O);
    RUN_TEST(test_ParametrosMissingObligatory);
    RUN_TEST(test_ParametrosSemEntrada_RetornaStringVazia);
    RUN_TEST(test_ParametrosFlagDesconhecidoRetornaNull);
    return UNITY_END();
}
