#include "unity/unity.h"
#include "leitores/Include_leitorGeo/leitor_geo.h"
#include "estr_dados/Include_HashFile/HashFile.h"
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

void test_LerArquivoGeo(void) {
    FILE *f = fopen("test.geo", "w");
    fprintf(f, "cq 1.0 1.0 1.0 1.0 2.0 red blue\n");
    fprintf(f, "q cep1 10.0 20.0 50.0 50.0\n");
    fclose(f);
    
    HashFile hf = hash_open("test_geo.hf", "test_geo.idx");
    TEST_ASSERT_NOT_NULL(hf);
    
    ler_arquivo_geo(".", "test.geo", hf);
    
    char buffer[200];
    // Verifica se a quadra foi registrada com cep1
    TEST_ASSERT_EQUAL_INT(1, hash_search(hf, "cep1", buffer, 200));
    
    hash_close(hf);
    remove("test.geo");
    remove("test_geo.hf");
    remove("test_geo.idx");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_LerArquivoGeo);
    return UNITY_END();
}
