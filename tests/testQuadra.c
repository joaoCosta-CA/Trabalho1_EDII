#include "unity/unity.h"
#include "mundo/Include_quadra/quadra.h"
#include <stdio.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_CriarQuadraSuccess(void) {
    Quadra q = criar_quadra("12345", 10.5, 20.0, 100.0, 50.0, 2.5, "red", "blue");
    TEST_ASSERT_NOT_NULL(q);
    
    TEST_ASSERT_EQUAL_STRING("12345", quadra_get_cep(q));
    TEST_ASSERT_EQUAL_DOUBLE(10.5, quadra_get_x(q));
    TEST_ASSERT_EQUAL_DOUBLE(20.0, quadra_get_y(q));
    TEST_ASSERT_EQUAL_DOUBLE(100.0, quadra_get_w(q));
    TEST_ASSERT_EQUAL_DOUBLE(50.0, quadra_get_h(q));
    TEST_ASSERT_EQUAL_DOUBLE(2.5, quadra_get_sw(q));
    TEST_ASSERT_EQUAL_STRING("red", quadra_get_cfill(q));
    TEST_ASSERT_EQUAL_STRING("blue", quadra_get_cstrk(q));
    
    destruir_quadra(q);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_CriarQuadraSuccess);
    return UNITY_END();
}
