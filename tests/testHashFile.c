#include "unity/unity.h"
#include "includes/estr_dados/Include_HashFile/HashFile.h"
#include <stdio.h>
#include <string.h>


HashFile hf;

const char* DB_PATH = "test_data.dat";
const char* IDX_PATH = "test_index.idx";

int global_counter = 0;


void count_records_callback(const char* key, const char* value, void *userdata) {
    (void)key;
    (void)value;
    (void)userdata;
    global_counter++;
}

void setUp(void) {

    remove(DB_PATH);
    remove(IDX_PATH);
    hf = hash_open(DB_PATH, IDX_PATH);
}

void tearDown(void) {
    if (hf) {
        hash_close(hf);
        hf = NULL;
    }
}

// --- Testes Unitários ---

void test_InitShouldCreateEmptyHash(void) {
    TEST_ASSERT_NOT_NULL(hf);
    TEST_ASSERT_EQUAL_INT(0, hash_get_global_depth(hf));
}

void test_InsertAndSearchSuccess(void) {
    char buffer[50];
    
    int res = hash_insert(hf, "chave100", "Teste Unico");
    TEST_ASSERT_EQUAL_INT(1, res);

    int found = hash_search(hf, "chave100", buffer, 50);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_EQUAL_STRING("Teste Unico", buffer);
}

void test_UpdateExistingRecord(void) {
    char buffer[50];
    hash_insert(hf, "cpf42", "Original");
    
    int updated = hash_update(hf, "cpf42", "Atualizado");
    TEST_ASSERT_EQUAL_INT(1, updated);

    hash_search(hf, "cpf42", buffer, 50);
    TEST_ASSERT_EQUAL_STRING("Atualizado", buffer);
}

void test_SearchShouldReturnZeroForMissingKey(void) {
    char buffer[50];
    int found = hash_search(hf, "inexistente", buffer, 50);
    TEST_ASSERT_EQUAL_INT(0, found);
}

void test_SearchWithNullBufferShouldNotCrash(void) {
    hash_insert(hf, "cpf_null", "Valor Qualquer");

    /* Deve retornar 1 sem crashar, mesmo com buffer NULL */
    int found = hash_search(hf, "cpf_null", NULL, 0);
    TEST_ASSERT_EQUAL_INT(1, found);

    /* Chave inexistente com buffer NULL também não deve crashar */
    int not_found = hash_search(hf, "nao_existe", NULL, 0);
    TEST_ASSERT_EQUAL_INT(0, not_found);
}

void test_SplitAndDepthIncrease(void) {

    hash_insert(hf, "cep02", "Valor 2");  
    hash_insert(hf, "cep04", "Valor 4");  
    hash_insert(hf, "cep06", "Valor 6"); 
    hash_insert(hf, "cep08", "Valor 8");
    
    TEST_ASSERT_TRUE(hash_get_global_depth(hf) > 0);
}

void test_RemoveRecord(void) {
    char buffer[50];
    hash_insert(hf, "cpf50", "Remover");
    
    int res = hash_remove(hf, "cpf50");
    TEST_ASSERT_EQUAL_INT(1, res);

    int found = hash_search(hf, "cpf50", buffer, 50);
    TEST_ASSERT_EQUAL_INT(0, found);
}

void test_ForAllShouldVisitAllRecords(void) {
    global_counter = 0;
    
    hash_insert(hf, "k10", "A");
    hash_insert(hf, "k20", "B");
    hash_insert(hf, "k30", "C");
    
    hash_forall(hf, count_records_callback, NULL);
    
    TEST_ASSERT_EQUAL_INT(3, global_counter);
}

void test_PersistenceAfterClose(void) {
    char buffer[50];
    hash_insert(hf, "cpf77", "Persistente");
    
    hash_close(hf);
    hf = NULL; 

    HashFile hf2 = hash_open(DB_PATH, IDX_PATH);
    TEST_ASSERT_NOT_NULL(hf2);
    
    int found = hash_search(hf2, "cpf77", buffer, 50);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_EQUAL_STRING("Persistente", buffer);
    
    hash_close(hf2);
}

void test_InsertDuplicateShouldFail(void) {
    int res1 = hash_insert(hf, "dup_key", "Primeiro");
    TEST_ASSERT_EQUAL_INT(1, res1);

    int res2 = hash_insert(hf, "dup_key", "Segundo");
    TEST_ASSERT_EQUAL_INT(0, res2);

    /* O valor original deve permanecer */
    char buffer[50];
    hash_search(hf, "dup_key", buffer, 50);
    TEST_ASSERT_EQUAL_STRING("Primeiro", buffer);
}

// --- Funcao Principal de Testes ---
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_InitShouldCreateEmptyHash);
    RUN_TEST(test_InsertAndSearchSuccess);
    RUN_TEST(test_UpdateExistingRecord);
    RUN_TEST(test_SearchShouldReturnZeroForMissingKey);
    RUN_TEST(test_SearchWithNullBufferShouldNotCrash);
    RUN_TEST(test_SplitAndDepthIncrease);
    RUN_TEST(test_RemoveRecord);
    RUN_TEST(test_ForAllShouldVisitAllRecords);
    RUN_TEST(test_PersistenceAfterClose);
    RUN_TEST(test_InsertDuplicateShouldFail);
    
    return UNITY_END();
}