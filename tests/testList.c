#include "unity/unity.h"
#include "estr_dados/Include_list/list.h"
#include <stdio.h>

void setUp(void) {}
void tearDown(void) {}

void test_ListCreation(void) {
    Lista l = createList();
    TEST_ASSERT_NOT_NULL(l);
    TEST_ASSERT_EQUAL_INT(0, length(l));
    killList(l, NULL);
}

void test_ListInsertAndLength(void) {
    Lista l = createList();
    int val1 = 10, val2 = 20;
    
    Posic p1 = insert(l, &val1);
    TEST_ASSERT_NOT_NULL(p1);
    TEST_ASSERT_EQUAL_INT(1, length(l));
    
    Posic p2 = insert(l, &val2);
    TEST_ASSERT_NOT_NULL(p2);
    TEST_ASSERT_EQUAL_INT(2, length(l));
    
    killList(l, NULL);
}

void test_ListGetAndTraversal(void) {
    Lista l = createList();
    int val1 = 10, val2 = 20, val3 = 30;
    insert(l, &val1);
    insert(l, &val2);
    insert(l, &val3);
    
    Posic p = getFirst(l);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(10, *((int*)get(l, p)));
    
    p = getNext(l, p);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(20, *((int*)get(l, p)));
    
    p = getNext(l, p);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_EQUAL_INT(30, *((int*)get(l, p)));
    
    p = getNext(l, p);
    TEST_ASSERT_NULL(p);
    
    killList(l, NULL);
}

void test_ListRemovePosic(void) {
    Lista l = createList();
    int val1 = 10, val2 = 20, val3 = 30;
    Posic p1 = insert(l, &val1);
    Posic p2 = insert(l, &val2);
    insert(l, &val3);
    
    // Remove do meio
    int* rem = (int*)removePosic(l, p2);
    TEST_ASSERT_EQUAL_INT(20, *rem);
    TEST_ASSERT_EQUAL_INT(2, length(l));
    
    // Remove do inicio
    rem = (int*)removePosic(l, p1);
    TEST_ASSERT_EQUAL_INT(10, *rem);
    TEST_ASSERT_EQUAL_INT(1, length(l));
    
    killList(l, NULL);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ListCreation);
    RUN_TEST(test_ListInsertAndLength);
    RUN_TEST(test_ListGetAndTraversal);
    RUN_TEST(test_ListRemovePosic);
    return UNITY_END();
}
