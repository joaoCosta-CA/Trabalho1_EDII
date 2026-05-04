/**
 * @file testQuadra.c
 * @brief Testes unitários para o módulo Quadra.
 *
 * Cobre: criação, todos os getters, destruição segura com NULL,
 * e criação com cores NULL (cfill/cstrk opcionais).
 */
#include "unity/unity.h"
#include "mundo/Include_quadra/quadra.h"
#include <stdio.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ─── Criação e getters ───────────────────────────────────────────────────── */

void test_CriarQuadraSuccess(void) {
    Quadra q = criar_quadra("CEP-01.1", 10.5, 20.0, 100.0, 50.0, 2.5, "red", "blue");
    TEST_ASSERT_NOT_NULL(q);

    TEST_ASSERT_EQUAL_STRING("CEP-01.1", quadra_get_cep(q));
    TEST_ASSERT_EQUAL_DOUBLE(10.5, quadra_get_x(q));
    TEST_ASSERT_EQUAL_DOUBLE(20.0, quadra_get_y(q));
    TEST_ASSERT_EQUAL_DOUBLE(100.0, quadra_get_w(q));
    TEST_ASSERT_EQUAL_DOUBLE(50.0, quadra_get_h(q));
    TEST_ASSERT_EQUAL_DOUBLE(2.5, quadra_get_sw(q));
    TEST_ASSERT_EQUAL_STRING("red", quadra_get_cfill(q));
    TEST_ASSERT_EQUAL_STRING("blue", quadra_get_cstrk(q));

    destruir_quadra(q);
}

/* ─── Criação com coordenadas de origem zero ─────────────────────────────── */

void test_CriarQuadraOrigemZero(void) {
    Quadra q = criar_quadra("B00-0.0", 0.0, 0.0, 50.0, 25.0, 1.0, "white", "black");
    TEST_ASSERT_NOT_NULL(q);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_get_x(q));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_get_y(q));
    destruir_quadra(q);
}

/* ─── Criação com cores NULL (campos opcionais) ───────────────────────────── */

void test_CriarQuadraNullColors(void) {
    /* Deve criar sem crash mesmo com cfill/cstrk NULL */
    Quadra q = criar_quadra("CEP-NULL", 5.0, 5.0, 30.0, 30.0, 1.0, NULL, NULL);
    TEST_ASSERT_NOT_NULL(q);
    /* Getters devem retornar string vazia (não NULL) para cores não definidas */
    TEST_ASSERT_NOT_NULL(quadra_get_cfill(q));
    TEST_ASSERT_NOT_NULL(quadra_get_cstrk(q));
    destruir_quadra(q);
}

/* ─── Getters com quadra NULL não crasham ─────────────────────────────────── */

void test_GettersComQuadraNullSaoSeguros(void) {
    TEST_ASSERT_NULL(quadra_get_cep(NULL));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_get_x(NULL));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_get_y(NULL));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_get_w(NULL));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_get_h(NULL));
    TEST_ASSERT_EQUAL_DOUBLE(0.0, quadra_get_sw(NULL));
    TEST_ASSERT_NULL(quadra_get_cfill(NULL));
    TEST_ASSERT_NULL(quadra_get_cstrk(NULL));
}

/* ─── Destruição segura com NULL ─────────────────────────────────────────── */

void test_DestruirQuadraNullNaoCrasha(void) {
    destruir_quadra(NULL); /* não deve crashar */
}

/* ─── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_CriarQuadraSuccess);
    RUN_TEST(test_CriarQuadraOrigemZero);
    RUN_TEST(test_CriarQuadraNullColors);
    RUN_TEST(test_GettersComQuadraNullSaoSeguros);
    RUN_TEST(test_DestruirQuadraNullNaoCrasha);
    return UNITY_END();
}
