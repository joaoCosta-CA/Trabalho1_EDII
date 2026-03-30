#include "unity/unity.h"
#include "src/includes/Lib_HashFile/HashFile.h"
#include <stdio.h>
#include <string.h>


HashFile hf;

const char* DB_PATH = "test_data.dat";
const char* IDX_PATH = "test_index.idx";

int global_counter = 0;


void count_records_callback(int key, const char* value) {
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
    
    int res = hash_insert(hf, 100, "Teste Unico");
    TEST_ASSERT_EQUAL_INT(1, res);

    int found = hash_search(hf, 100, buffer, 50);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_EQUAL_STRING("Teste Unico", buffer);
}

void test_UpdateExistingRecord(void) {
    char buffer[50];
    hash_insert(hf, 42, "Original");
    
    int updated = hash_update(hf, 42, "Atualizado");
    TEST_ASSERT_EQUAL_INT(1, updated);

    hash_search(hf, 42, buffer, 50);
    TEST_ASSERT_EQUAL_STRING("Atualizado", buffer);
}

void test_SearchShouldReturnZeroForMissingKey(void) {
    char buffer[50];
    int found = hash_search(hf, 999, buffer, 50);
    TEST_ASSERT_EQUAL_INT(0, found);
}

void test_SplitAndDepthIncrease(void) {

    hash_insert(hf, 2, "Valor 2");  
    hash_insert(hf, 4, "Valor 4");  
    hash_insert(hf, 6, "Valor 6"); 
    hash_insert(hf, 8, "Valor 8");
    
    TEST_ASSERT_TRUE(hash_get_global_depth(hf) > 0);
}

void test_RemoveRecord(void) {
    char buffer[50];
    hash_insert(hf, 50, "Remover");
    
    int res = hash_remove(hf, 50);
    TEST_ASSERT_EQUAL_INT(1, res);

    int found = hash_search(hf, 50, buffer, 50);
    TEST_ASSERT_EQUAL_INT(0, found);
}

void test_ForAllShouldVisitAllRecords(void) {
    global_counter = 0;
    
    hash_insert(hf, 10, "A");
    hash_insert(hf, 20, "B");
    hash_insert(hf, 30, "C");
    
    hash_forall(hf, count_records_callback);
    
    TEST_ASSERT_EQUAL_INT(3, global_counter);
}

void test_PersistenceAfterClose(void) {
    char buffer[50];
    hash_insert(hf, 77, "Persistente");
    
    hash_close(hf);
    hf = NULL; 

    HashFile hf2 = hash_open(DB_PATH, IDX_PATH);
    TEST_ASSERT_NOT_NULL(hf2);
    
    int found = hash_search(hf2, 77, buffer, 50);
    TEST_ASSERT_EQUAL_INT(1, found);
    TEST_ASSERT_EQUAL_STRING("Persistente", buffer);
    
    hash_close(hf2);
}

// --- Funcao Principal de Testes ---
int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_InitShouldCreateEmptyHash);
    RUN_TEST(test_InsertAndSearchSuccess);
    RUN_TEST(test_UpdateExistingRecord);
    RUN_TEST(test_SearchShouldReturnZeroForMissingKey);
    RUN_TEST(test_SplitAndDepthIncrease);
    RUN_TEST(test_RemoveRecord);
    RUN_TEST(test_ForAllShouldVisitAllRecords);
    RUN_TEST(test_PersistenceAfterClose);
    
    return UNITY_END();
}