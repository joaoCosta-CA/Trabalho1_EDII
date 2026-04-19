#include "unity/unity.h"
#include "svg/svg.h"
#include "estr_dados/Include_list/list.h"
#include "estr_dados/Include_HashFile/HashFile.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void setUp(void) {}
void tearDown(void) {}

void test_SvgInitAndGeneration(void) {
    HashFile hf = hash_open("test_svg_quadras.hf", "test_svg_quadras.idx");
    hash_insert(hf, "cep1", "cep1;10.0;10.0;100.0;100.0;2.0;red;blue");
    
    FILE *f = svg_iniciar("test.svg", 0, 0, 500, 500);
    TEST_ASSERT_NOT_NULL(f);
    
    svg_gerar_quadras(hf, f);
    
    Lista dec = createList();
    char* str = malloc(100);
    snprintf(str, 100, "<line x1=\"0\" y1=\"0\" x2=\"10\" y2=\"10\"/>");
    insert(dec, str);
    
    svg_gerar_decoracoes(dec, f);
    killList(dec, free);
    
    svg_fechar(f);
    hash_close(hf);
    
    struct stat st;
    TEST_ASSERT_EQUAL_INT(0, stat("test.svg", &st));
    TEST_ASSERT_TRUE(st.st_size > 0);
    
    remove("test.svg");
    remove("test_svg_quadras.hf");
    remove("test_svg_quadras.idx");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_SvgInitAndGeneration);
    return UNITY_END();
}
