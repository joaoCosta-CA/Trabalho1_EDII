#include "unity/unity.h"
#include "leitores/Include_leitorQry/leitorQry.h"
#include "estr_dados/Include_HashFile/HashFile.h"
#include "estr_dados/Include_list/list.h"
#include <stdio.h>
#include <stdlib.h>

void setUp(void) {}
void tearDown(void) {}

void test_LerArquivoQryRQ(void) {
    FILE *f = fopen("test.qry", "w");
    fprintf(f, "rq cep_test\n");
    fclose(f);
    
    HashFile hq = hash_open("test_qry_q.hf", "test_qry_q.idx");
    hash_insert(hq, "cep_test", "cep_test;10;10;100;100;1;red;black");
    
    HashFile hp = hash_open("test_qry_p.hf", "test_qry_p.idx");
    
    Lista dec = leitor_qry(".", "test.qry", hq, hp, ".", "baseout");
    TEST_ASSERT_NOT_NULL(dec);
    
    char buffer[200];
    // Verifica se cep_test foi removido
    TEST_ASSERT_EQUAL_INT(0, hash_search(hq, "cep_test", buffer, 200)); 
    
    if (dec) killList(dec, free);
    
    hash_close(hq);
    hash_close(hp);
    
    remove("test.qry");
    remove("baseout-test.txt");
    remove("baseout.txt");
    remove("test_qry_q.hf");
    remove("test_qry_q.idx");
    remove("test_qry_p.hf");
    remove("test_qry_p.idx");
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_LerArquivoQryRQ);
    return UNITY_END();
}
