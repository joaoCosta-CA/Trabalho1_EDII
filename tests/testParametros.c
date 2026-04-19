#include "unity/unity.h"
#include "parametros/parametros.h"
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

void test_ParametrosParseSuccess(void) {
    char *argv[] = {"ted", "-f", "cidade.geo", "-o", "saida_dir", "-pm", "pessoas.pm"};
    int argc = 7;
    
    Parametros p = parametros_parse(argc, argv);
    TEST_ASSERT_NOT_NULL(p);
    
    TEST_ASSERT_EQUAL_STRING("cidade.geo", parametros_get_geo(p));
    TEST_ASSERT_EQUAL_STRING("saida_dir", parametros_get_saida(p));
    TEST_ASSERT_EQUAL_STRING("pessoas.pm", parametros_get_pm(p));
    TEST_ASSERT_NULL(parametros_get_qry(p));
    
    parametros_destroy(p);
}

void test_ParametrosMissingObligatory(void) {
    char *argv_missing_f[] = {"ted", "-o", "saida_dir"};
    Parametros p1 = parametros_parse(3, argv_missing_f);
    TEST_ASSERT_NULL(p1);
    
    char *argv_missing_o[] = {"ted", "-f", "cidade.geo"};
    Parametros p2 = parametros_parse(3, argv_missing_o);
    TEST_ASSERT_NULL(p2);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ParametrosParseSuccess);
    RUN_TEST(test_ParametrosMissingObligatory);
    return UNITY_END();
}
