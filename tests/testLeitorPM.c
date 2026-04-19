#include "unity/unity.h"
#include "leitores/Include_leitorPm/leitorPM.h"
#include "estr_dados/Include_HashFile/HashFile.h"
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

void test_LerArquivoPM(void) {
    FILE *f = fopen("test.pm", "w");
    fprintf(f, "p 123456789 Joao Silva m 01/01/2000\n");
    fprintf(f, "m cep1 N 10 compl 123456789\n");
    fclose(f);
    
    HashFile hf = hash_open("test_pm.hf", "test_pm.idx");
    TEST_ASSERT_NOT_NULL(hf);
    
    ler_arquivo_pm(".", "test.pm", hf);
    
    char buffer[300];
    int res = hash_search(hf, "123456789", buffer, 300);
    TEST_ASSERT_EQUAL_INT(1, res);
    
    hash_close(hf);
    remove("test.pm");
    remove("test_pm.hf");
    remove("test_pm.idx");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_LerArquivoPM);
    return UNITY_END();
}
